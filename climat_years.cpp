#include <map>
#include <string>
#include <iomanip>
#include <numeric>

#include "LinearModel.h"

const string pathData = "Data/NewData/mean/years/";

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

vector< vector<double> > deviationMonths(vector<point> a, vector<point> b, string name1, string name2) {
	vector< vector<double> > res(12, vector<double>(2));

	int m = a.size() / 12;

	for (int i = 0; i < 12; i++) {
		res[i] = deviation(vector<point>(a.begin() + i * m, a.begin() + (i + 1) * m),
								vector<point>(b.begin() + i * m, b.begin() + (i + 1) * m));
	}

	return res;
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

void shift(int n, int years, vector< vector< vector<point> > > data, vector< vector<point> > real,
															vector<double> coef = vector<double>(0)) {
	// Мнк по разницам, строим коэффицинты на двух пердыдущих годах,
    // поэтому начинаем с третьего года
    for (int i = 2; i < years; i++) {
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
		if (inner_product(coef.begin(), coef.end(), coef.begin(), 0) == 0) {
			lm.fit(shift, shift_real);
		} else {
			lm.coef = coef;
		}

    	vector<point> pred = lm.predict(shift_next);
    	for (int j = 0; j < pred.size(); j++) {
    		pred[j].temp += real[i-1][j].temp;
    	}

    	cout << "Shift MNK " << i + 1971 << endl;
    	cout << "Deviation " << i + 1971 << " l1: " << deviation(pred, real[i])[0] << endl;

    	vector<point> predTest = lm.predict(shift);
    	for (int j = 0; j < predTest.size(); j++) {
    		predTest[j].temp += real[i-2][j].temp;
    	}
    	cout << "Deviation " << i-1 + 1971 << " l1: " << deviation(predTest, real[i-1])[0] << endl;
    	cout << "Coefficients: " << toString(lm.coef) << endl;
    	cout << endl;
    }

    // cout << "deviation 1973 and 1972 real " << deviation(real[1], real[2]) << endl;
}

int main() {
	cout << fixed << setprecision(2);
    int n = 12;
    int years = 40;
    int size;
    
	cout << "Loading data." << flush;

    ifstream fin(pathData + "temp1971-2010.csv");
    fin >> years >> n >> size;
    string sep;

    vector< vector< vector<point> > > data(years, vector< vector<point> >(size, vector<point>(n)));
    vector< vector<point> > real(years, vector<point>(size));

    for (int i = 0; i < years; i++) {
		cout << "." << flush;
        for (int j = 0; j < size; j++) {
            for (int f = 0; f < n; f++) {
                fin >> data[i][j][f].t >> sep
                    >> data[i][j][f].x >> sep
                    >> data[i][j][f].y >> sep
                    >> data[i][j][f].temp;
            }
        }
    }

    for (int i = 0; i < years; i++) {
		cout << "." << flush;
        for (int j = 0; j < real[0].size(); j++) {
            fin >> real[i][j].t >> sep
                >> real[i][j].x >> sep
                >> real[i][j].y >> sep
                >> real[i][j].temp;
        }
    }
	cout << "Done" << endl;
    fin.close();

    // cout << deviation(data[0][0], real[0]) << endl;

	// Отклонения всех f-ок от реальных данных
	vector< vector<double> > dev_l1(years, vector<double>(n));
	vector< vector<double> > dev_l2(years, vector<double>(n));
	for (int f = 0; f < n; f++) { 
    	for (int i = 0; i < years; i++) {
    		dev_l1[i][f] = deviation(transpose(data[i])[f],real[i])[0];
			dev_l2[i][f] = deviation(transpose(data[i])[f],real[i])[1];
    	}
    }

	// Теперь все это красиво выводим в виде теховской таблицы

    cout << "L1 table" << endl;
    for (int f = 0; f < n; f++) {
		cout << "& $f_{" << f + 1 << "}$"; 
    }	
    cout << "\\\\" <<  endl << "\\hline" << endl;
    for (int i = 0; i < years; i++) {
    	cout << 1971 + i; 
    	for (int f = 0; f < n; f++) {
    		cout << " &" << dev_l1[i][f];
    	}
    	cout << "\\\\" << endl;
    	cout << "\\hline" << endl;
    }

    cout << endl;

   	cout << "L2 table" << endl;
    for (int f = 0; f < n; f++) {
		cout << "& $f_{" << f + 1 << "}$"; 
    }	
    cout << "\\\\" <<  endl << "\\hline" << endl;
    for (int i = 0; i < years; i++) {
    	cout << 1971 + i; 
    	for (int f = 0; f < n; f++) {
    		cout << " &" << dev_l2[i][f];
    	}
    	cout << "\\\\" << endl;
    	cout << "\\hline" << endl;
    }

    cout << endl;

    shift(n,years,data,real);

	// БЛОК БЕСПРЕДЕЛА ----------------------------------
	// Берем хорошие (подозрительно) коэффициенты для десятилетий и нагло применаяем к отдельным годам

	LinearModel lm;

	lm.coef = {0.44, -0.23, -0.09, -0.28, 0.21, -0.06, 0.09, -0.07, -0.09, 0.35, -0.19, 0.01, 0.04};
	cout << "Shift 1 coefs: " << toString(lm.coef);
	for (int i = 1; i < years; i++) {
		vector< vector<point> > shift = data[i];
		vector<point> shift_real = real[i];

    	for (int j = 0; j < shift.size(); j++) {
    		for (int k = 0; k < shift[0].size(); k++) {
				shift[j][k].temp -= data[i-1][j][k].temp;
    		}
    	}

		vector<point> pred = lm.predict(shift);

		for (int j = 0; j < shift.size(); j++) {			
			pred[j].temp += real[i-1][j].temp;
    	}

		cout << i + 1971 << " " << deviation(pred, real[i])[0] << endl;
	}
	cout << endl;

	lm.coef = {0.44, -0.23, 0, -0.28, 0.21, 0, 0, 0, 0, 0.35, -0.19, 0, 0};
	cout << "Hand-made coefs: " << toString(lm.coef);
	for (int i = 1; i < years; i++) {
		vector< vector<point> > shift = data[i];
		vector<point> shift_real = real[i];

    	for (int j = 0; j < shift.size(); j++) {
    		for (int k = 0; k < shift[0].size(); k++) {
				shift[j][k].temp -= data[i-1][j][k].temp;
    		}
    	}

		vector<point> pred = lm.predict(shift);

		for (int j = 0; j < shift.size(); j++) {			
			pred[j].temp += real[i-1][j].temp;
    	}

		cout << i + 1971 << " " << deviation(pred, real[i])[0] << endl;
	}
	cout << endl;

	lm.coef = {0.06, 0.30, 0.33, -0.01, 0.10, 0.68, 0.35, -0.64, 0.28, -0.36, -0.03, -0.10, 0.38};
	cout << "MNK3 coefs: " << toString(lm.coef);
	for (int i = 0; i < years; i++) {
		cout << i + 1971 << " " << deviation(lm.predict(data[i]), real[i])[0] << endl;
	}
	cout << endl;

	cout << endl << endl << "Shift1 decades coefs: " << endl;
	shift(n, years, data, real, 
		vector<double>({0.46, -0.53, -0.01, -0.24, 0.33, -0.29, 0.19, 0.06, 0.22, 0.26, -0.36, 0.10, -0.08 }));
}