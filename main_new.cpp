#include <map>
#include <string>
#include <iomanip>

#include "LinearModel.h"

const string pathData = "Data/NewData/";

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

string deviation(vector<point> a, vector<point> b) {
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

    stringstream out;

    out << fixed << setprecision(2);

    out << "l1 = " << l1 << "; " << "l2 = " << l2;

    return out.str();
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

int main() {
	// Количество прогнозов
    // int n;
    // cout << "Number of predictions: ";
    // cin >> n;
    int n = 14;

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

    	// printMatrix(data[i]);
    	// cout << toString(real[i]);
    }

    // cout << data[2][0][0].temp << endl;
    // cout << data[0][0].size() << endl;
    // cout << real[0].size() << endl;
    // cout << real[0][0].temp << endl;


    // cout << deviation(data[0][0], real[0]) << endl;

    for (int i = 0; i < years; i++) {
    	cout << 1971+i << endl;
    	for (int j = 0; j < n; j++) {
    		cout << "f" << j + 1 << ": ";
    		cout << deviation(data[i][j], real[i]);
    		cout << endl;
    	}
    	cout << endl;
    }
    cout << endl;

    for (int i = 0; i < years; i++) {
    	//Первая координата -- номер  точки, вторая -- номер прогноза
    	data[i]=transpose(data[i]);
    	// printMatrix(data[i]);
    }

    // cout << "Ради интереса: коэффицинты созданы на 1971 и применены к 1972" << endl;
    // LinearModel l; 
    // l.fitWithFreeCoefficient(data[0], real[0]);
    // vector<point> p0 = l.predict(data[0]);
    // vector<point> p1 = l.predict(data[1]);
    // cout << toString(l.coef);
    // cout << "1971: " << deviation(p0, real[0]) << endl;
    // cout << "1972: " << deviation(p1, real[1]) << endl;
    // cout << endl;

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
    	lm.fitWithFreeCoefficient(shift, shift_real);

    	vector<point> pred = lm.predict(shift_next);
    	for (int j = 0; j < pred.size(); j++) {
    		pred[j].temp += real[i-1][j].temp;
    	}

    	cout << "Shift MNK " << i + 1971 << endl;
    	cout << "Deviation " << i + 1971 << " " << deviation(pred, real[i]) << endl;

    	vector<point> predTest = lm.predict(shift);
    	for (int j = 0; j < predTest.size(); j++) {
    		predTest[j].temp += real[i-2][j].temp;
    	}
    	cout << "Deviation " << i-1 + 1971 << " " << deviation(predTest, real[i-1]) << endl;
    	cout << "Coefficients: " << toString(lm.coef) << endl;
    	cout << endl;
    }

    // cout << "deviation 1973 and 1972 real " << deviation(real[1], real[2]) << endl;

    cout << "Done." << endl << endl;

}