#include <map>
#include <string>
#include <iomanip>

#include "LinearModel.h"

const string pathData = "Data/NewData/years/";

string toString(vector<double> a) {
    stringstream sout;
    sout << fixed << setprecision(2);
    for (auto it = a.begin(); it != a.end(); it++) {
        sout << *it << " ";
    }

    sout << endl;

    return sout.str();
}

string toString(vector<point> a) {
    stringstream sout;
    sout << fixed << setprecision(2);
    for (auto it = a.begin(); it != a.end(); it++) {
        sout << (*it).t << " " << (*it).x << " "
                << (*it).y << " " << (*it).temp << endl;
    }

    return sout.str();
}

vector< vector<point> > transpose(vector< vector<point> > a) {
    vector< vector<point> > res(a[0].size(), vector<point>(a.size()));

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < a[0].size(); j++) {
            res[j][i] = a[i][j];
        }
    }

    return res;
}

vector<double> deviation(vector<point> a, vector<point> b) {
	if (a.size() != b.size()) cout << "Size error" << endl;

	int n = a.size();

    double l1 = 0;
    double l2 = 0;

    for (int i = 0; i < n; i++) {
        l1 += abs(a[i].temp - b[i].temp);
        // cout << abs(a[i].temp - b[i].temp) << endl;
        l2 += (a[i].temp - b[i].temp) * (a[i].temp - b[i].temp);
    }

    l1 /= n;

    l2 = sqrt(l2 / n);

    vector<double> res(2);
    res[0] = l1;
    res[1] = l2;

    return res;
}

void deviationMonths(vector<point> a, vector<point> b, string name1, string name2) {
	cout << endl << "";
}

void printMatrix(vector< vector<point> > X) {
    cout << endl;

    for (unsigned int i = 0; i < X.size(); i++) {
        for (unsigned int j = 0; j < X[0].size(); j++) {
            cout << X[i][j].temp << "  ";
        }
        cout << endl;
    }

    cout << endl;
}

void shift(int n, int decs, vector< vector< vector<point> > > data, vector< vector<point> > real) {

	// Мнк по разницам, строим коэффицинты на двух пердыдущих десятилетиях,
    // поэтому начинаем с третьего десятилетия
    for (int i = 2; i < decs; i++) {
    	vector< vector<point> > shift_next = data[i];
    	vector< vector<point> > shift = data[i-1];
    	vector<point> shift_real = real[i-1];

    	// cout << "Shift size: " << shift.size() << endl;
    	// cout << "Shift0 size: " << shift[0].size() << endl;

    	for (int j = 0; j < shift.size(); j++) {
    		for (int k = 0; k < shift[0].size(); k++) {
				shift_next[j][k].temp -= data[i-1][j][k].temp;
				shift[j][k].temp -= data[i-2][j][k].temp;
    		}
    		shift_real[j].temp -= real[i-2][j].temp;
    	}

    	// cout << "shift: ";
    	// printMatrix(shift);
    	// cout << "shift_real: " << toString(shift_real) << endl;

    	LinearModel lm;
    	lm.fitWithFreeCoefficient(shift, shift_real);

    	vector<point> pred = lm.predict(shift_next);
    	for (int j = 0; j < pred.size(); j++) {
    		pred[j].temp += real[i-1][j].temp;
    	}

    	cout << "Shift MNK " << i*10 + 1971 << "-" << i*10 + 1980 << endl;
    	cout << "Deviation " << i*10 + 1971 << "-" << i*10 + 1980 << " " << deviation(pred, real[i])[0] << endl;

    	vector<point> predTest = lm.predict(shift);
    	for (int j = 0; j < predTest.size(); j++) {
    		predTest[j].temp += real[i-2][j].temp;
    	}
    	cout << "Deviation " << (i-1)*10 + 1971 << "-" << (i-1)*10 + 1980 << " " << deviation(predTest, real[i-1])[0] << endl;
    	cout << "Coefficients: " << toString(lm.coef) << endl;
    	cout << endl;
    }

    // cout << "deviation 1973 and 1972 real " << deviation(real[1], real[2]) << endl;
}

void mnk(int n, int decs, vector< vector< vector<point> > > data, vector< vector<point> > real) {
	stringstream tablel1;
	stringstream tablel2;

	tablel1 << setprecision(2) << endl;
	tablel2 << setprecision(2) << endl; 

	map<string, vector<double> > coefs;

	for (int i = 0; i < decs; i++) {

		LinearModel lm;
		lm.fitWithFreeCoefficient(data[i], real[i]);
		coefs.insert(pair<string, vector<double> >("MNK_" + to_string(i+1), lm.coef));

		// Отклонения на всех остальных периодах L1
		tablel1 << "L1: $MNK_{" << i+1 << "}$ ";
		for (int j = 0; j < decs; j++) {
			tablel1 << "& $" << deviation(lm.predict(data[j]), real[j])[0] << "$ ";
		}
		tablel1 << "\\\\" << endl << "\\hline" << endl;

		// Отклонения на всех остальных периодах L2
		tablel2 << "L2: $MNK_{" << i+1 << "}$ ";
		for (int j = 0; j < decs; j++) {
			tablel2 << "& $" << deviation(lm.predict(data[j]), real[j])[1] << "$ ";
		}
		tablel2 << "\\\\" << endl << "\\hline" << endl;

		// Без свободного коэффициента

		lm.fit(data[i], real[i]);
		coefs.insert(pair<string, vector<double> >("SMNK_" + to_string(i+1), lm.coef));

		// Отклонения на всех остальных периодах L1
		tablel1 << "L1: $SMNK_{" << i+1 << "}$ ";
		for (int j = 0; j < decs; j++) {
			tablel1 << "& $" << deviation(lm.predict(data[j]), real[j])[0] << "$ ";
		}
		tablel1 << "\\\\" << endl << "\\hline" << endl;

		// Отклонения на всех остальных периодах L2
		tablel2 << "L2: $SMNK_{" << i+1 << "}$ ";
		for (int j = 0; j < decs; j++) {
			tablel2 << "& $" << deviation(lm.predict(data[j]), real[j])[1] << "$ ";
		}
		tablel2 << "\\\\" << endl << "\\hline" << endl;

	}

	cout << "MNK L1 table" << tablel1.str();
	cout << endl;
	cout << "MNK L2 table" << tablel2.str();
	cout << endl;

	for (auto it = coefs.begin(); it != coefs.end(); ++it) {
		cout << (*it).first << " : " << toString((*it).second);
	}
}

int main() {
	cout << fixed << setprecision(2);
    int n = 12;

    int years = 40;

    vector< vector< vector<point> > > data(years, vector< vector<point> >(n));
    vector< vector<point> > real(years);

    cout << "Loading data... " << endl;

    for (int i = 0; i < years; i++) {
    	int year = 1971 + i;

    	string pathname = pathData+to_string(year)+"/f";

    	// Приходится считывать из всех файлов одновременно, чтобы пропускать строки,
    	// в которых хотя бы в одном файле стоит missing value
    	ifstream fin[n+1];

    	for (int j = 0; j < n+1; j++) {
 	   		fin[j].open(pathname+to_string(j)+".txt");
 	   		cout << "Open " + pathname+to_string(j)+".txt" << endl;
    	}
    	cout << endl;

    	vector<point> p(n+1);
    	bool goodLine;
    	while(fin[0] >> p[0].t >> p[0].x >> p[0].y >> p[0].temp) {
    		// cout << p.t << " " << p.x << " " << p.y << " " << p.temp << endl;
    		goodLine = (p[0].temp != -9999);

			for (int j = 1; j < n+1; j++) {
				fin[j] >> p[j].t >> p[j].x >> p[j].y >> p[j].temp;
				if (p[j].temp == 1e20) goodLine = false;
			}
    		
    		if (goodLine) {
    			for (int j=0; j<n; j++) {
    				data[i][j].push_back(p[j+1]);
    			}
    			real[i].push_back(p[0]);
    		}
    	}

    	for (int j = 0; j < n+1; j++) {
 	   		fin[j].close();
    	}
    }

    for (int i = 0; i < years; i++) {
    	//Первая координата -- номер  точки, вторая -- номер прогноза
    	data[i]=transpose(data[i]);
    }

    cout << data[0].size() << endl;
    cout << real[0].size() << endl;

    int decs = years / 10;

	// Усредняем значение в каждой точке за 10 лет
	vector< vector< vector<point> > > data_m(decs, data[0]);
    vector< vector<point> > real_m(decs, real[0]);

    for (int f = 0; f < n; f++) {
    	for (int j = 0; j < data_m[0].size(); j++) {
    		for (int i = 0; i < data_m.size(); i++) {
    			double mean = 0;
    			for (int k = 0; k < 10; k++) {
    				mean += data[i+k][j][f].temp;
    			}
    			data_m[i][j][f].temp = mean/10;
    		}
  	 	}
    }

    for (int j = 0; j < real_m[0].size(); j++) {
    	for (int i = 0; i < real_m.size(); i++) {
    		double mean = 0;
    		for (int k = 0; k < 10; k++) {
    			mean += real[i+k][j].temp;
    		}
    		real_m[i][j].temp = mean/10;
    	}
    }

    // Отклонение усредненный прогнозов от усредненных реальных данных

    cout << "L1 table" << endl;
    for (int i = 0; i < decs; i++) {
    	cout << "& " << 1971 + i*10 << "-" << 1980+i*10;
    }	
    cout << "\\\\" <<  endl << "\\hline" << endl;
    for (int f = 0; f < n; f++) {
    	cout << "$f_{" << f + 1 << "}$"; 
    	for (int i = 0; i < decs; i++) {
    		cout << " &" << deviation(transpose(data_m[i])[f],real_m[i])[0];
    	}
    	cout << "\\\\" << endl;
    	cout << "\\hline" << endl;
    }

    cout << endl;

   cout << "L2 table" << endl;
    for (int i = 0; i < decs; i++) {
    	cout << "& " << 1971 + i*10 << "-" << 1980+i*10;
    }	
    cout << "\\\\" <<  endl << "\\hline" << endl;
    for (int f = 0; f < n; f++) {
    	cout << "$f_{" << f + 1 << "}$"; 
    	for (int i = 0; i < decs; i++) {
    		cout << " &" << deviation(transpose(data_m[i])[f],real_m[i])[1];
    	}
    	cout << "\\\\" << endl;
    	cout << "\\hline" << endl;
    }

    cout << endl;


    // cout << "Ради интереса: коэффицинты созданы на 1971 и применены к 1972" << endl;
    // LinearModel l; 
    // l.fitWithFreeCoefficient(data[0], real[0]);
    // vector<point> p0 = l.predict(data[0]);
    // vector<point> p1 = l.predict(data[1]);
    // cout << toString(l.coef);
    // cout << "1971: " << deviation(p0, real[0]) << endl;
    // cout << "1972: " << deviation(p1, real[1]) << endl;
    // cout << endl;

    // shift(n, decs, data_m, real_m);

    mnk(n, decs, data_m, real_m);
}