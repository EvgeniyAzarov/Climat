#include <map>
#include <string>
#include <iomanip>

#include "LinearModel.h"

const string pathData = "Data/NewData/max/years/";

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

void shift(int n, int decs, vector< vector< vector<point> > > data, vector< vector<point> > real) {

	// Мнк по разницам, строим коэффицинты на двух пердыдущих десятилетиях,
    // поэтому начинаем с третьего десятилетия
    for (int i = 1; i < decs; i++) {
    	vector< vector<point> > shift = data[i];
    	vector<point> shift_real = real[i];

    	// cout << "Shift size: " << shift.size() << endl;
    	// cout << "Shift0 size: " << shift[0].size() << endl;

    	for (int j = 0; j < shift.size(); j++) {
    		for (int k = 0; k < shift[0].size(); k++) {
				shift[j][k].temp -= data[i-1][j][k].temp;
    		}
    		shift_real[j].temp -= real[i-1][j].temp;
    	}

    	// cout << "shift: ";
    	// printMatrix(shift);
    	// cout << "shift_real: " << toString(shift_real) << endl;

    	LinearModel lm;
    	lm.fit(shift, shift_real);

		cout << "Shift MNK " << i << endl;
		cout << "Coefficients: " << toString(lm.coef);

		for (int k = i; k < decs; k++) { 
			vector< vector<point> > shift_next = data[k];

			for (int l = 0; l < shift_next.size(); l++) {
				for (int t = 0; t < shift_next[0].size(); t++) {
					shift_next[l][t].temp -= data[k-1][l][t].temp;
				}
			}

			vector<point> pred = lm.predict(shift_next);
			for (int j = 0; j < pred.size(); j++) {
				pred[j].temp += real[k-1][j].temp;
			}

			cout << "Deviation " << k*10 + 1971 << "-" << k*10 + 1980 << " " 
				<< "l1: " << deviation(pred, real[k])[0]
				<< "l2: " << deviation(pred, real[k])[1]<< endl << endl;

		}
    }
}

void mnk(int n, int decs, vector< vector< vector<point> > > data, vector< vector<point> > real) {
	stringstream tablel1;
	stringstream tablel2;

	tablel1 << setprecision(2) << endl;
	tablel2 << setprecision(2) << endl; 

	map<string, vector<double> > coefs;

	for (int i = 0; i < decs; i++) {

		LinearModel lm;
		lm.fit(data[i], real[i]);
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

		lm.fit(data[i], real[i], false);
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
	cout << endl << endl;
}

void mnkMonths(int n, int decs, vector< vector< vector<point> > > data, vector< vector<point> > real) {
	stringstream tablel1;
	stringstream tablel2;

	tablel1 << setprecision(2) << endl;
	tablel2 << setprecision(2) << endl; 

	int m = 12; // Количество месяцев

	for (int i = 0; i < decs; i++) {

		int m  = data[i].size() / 12; // Количетсво точек (x,y) 

		vector< vector<point> > data_mon(12);
		vector<LinearModel> lm(12);
		vector<LinearModel> lmWithoutFreeCoef(12);

		for (int j = 0; j < 12; j++) {
			vector< vector<point> > data_mon(data[i].begin() + j * m, data[i].begin() + (j + 1) * m);

			vector<point> real_mon(real[i].begin() + j * m, real[i].begin() + (j + 1) * m);

			lm[j].fit(data_mon, real_mon);
			lmWithoutFreeCoef[j].fit(data_mon, real_mon, false);
		}

		// Отклонения на всех остальных периодах 
		tablel1 << "L1: $MNK_{" << i+1 << "}M$ ";
		tablel2 << "L2: $MNK_{" << i+1 << "}M$ ";
		for (int j = 0; j < decs; j++) {
			vector<point> pred;
			for (int k = 0; k < 12; k++) {
				vector< vector<point> > data_j_mon(data[j].begin() + k * m, data[j].begin() + (k + 1) * m);
				vector<point> pred_k = lm[k].predict(data_j_mon);
				pred.insert(pred.end(), pred_k.begin(), pred_k.end());
			}
			tablel1 << "& " << deviation(pred, real[j])[0] << " ";
			tablel2 << "& " << deviation(pred, real[j])[1] << " ";
		}
		tablel1 << "\\\\" << endl << "\\hline" << endl;
		tablel2 << "\\\\" << endl << "\\hline" << endl;

		// Без свободного коэффициента

		tablel1 << "L1: $SMNK_{" << i+1 << "}M$ ";
		tablel2 << "L2: $SMNK_{" << i+1 << "}M$ ";
		for (int j = 0; j < decs; j++) {
			vector<point> pred;
			for (int k = 0; k < 12; k++) {
				vector< vector<point> > data_j_mon(data[j].begin() + k * m, data[j].begin() + (k + 1) * m);
				vector<point> pred_k = lmWithoutFreeCoef[k].predict(data_j_mon);
				pred.insert(pred.end(), pred_k.begin(), pred_k.end());
			}
			tablel1 << "& " << deviation(pred, real[j])[0] << " ";
			tablel2 << "& " << deviation(pred, real[j])[1] << " ";
		}
		tablel1 << "\\\\" << endl << "\\hline" << endl;
		tablel2 << "\\\\" << endl << "\\hline" << endl;

	}

	cout << "MNK M L1 table" << tablel1.str();
	cout << endl;
	cout << "MNK M L2 table" << tablel2.str();
	cout << endl << endl;
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

    cout << "Number of points (t,x,y): " << data[0].size() << endl;

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

    mnk(n, decs, data_m, real_m);

    mnkMonths(n, decs, data_m, real_m);

	shift(n, decs, data_m, real_m);
}
