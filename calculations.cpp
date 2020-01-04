#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <locale>

#include "LinearModel.h"

using namespace std;

const string pathData = "Data/NewData/years/";
const string pathResult = "Result files/New/";

// Вывод вещественных числе с запятой
class comma : public numpunct<char> {
    public:
        comma () : numpunct<char> () {}
    protected:
    char do_decimal_point() const {
        return ',';
    }
};

vector< vector<point> > transpose(vector< vector<point> > a) {
    vector< vector<point> > res(a[0].size(), vector<point>(a.size()));

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < a[0].size(); j++) {
            res[j][i] = a[i][j];
        }
    }

    return res;
}

int main() {
    cout << fixed << setprecision(2);
    int n = 12;
    int years = 40;

    cout << "Loading data... " << endl;

    // Считываение всех данных из одного файла
    int size;

    ifstream fin(pathData + "temp1971-2010.csv");
    fin >> years >> n >> size;
    string sep;

    vector< vector< vector<point> > > data(years, vector< vector<point> >(size, vector<point>(n)));
    vector< vector<point> > real(years, vector<point>(size));

    for (int i = 0; i < years; i++) {
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
        for (int j = 0; j < real[0].size(); j++) {
            fin >> real[i][j].t >> sep
                >> real[i][j].x >> sep
                >> real[i][j].y >> sep
                >> real[i][j].temp;
        }
    }

    fin.close();

    // Считаем функцию распределения для каждого года в каждом месяце

    ofstream f1, f2;
    f1.open(pathResult + "distribute function.csv");
    locale loccomma(f1.getloc(), new comma); 
    f1.imbue(loccomma);
    f1 << setprecision(2);

    f2.open(pathResult + "mean temperature.csv");
    f2.imbue(loccomma);
    f2 << setprecision(2);

    cout << "real[0].size(): " << real[0].size() << endl;

    f1 << "; ";
    for (int t = -22; t <=23; t++) {
        f1 << "; " << t;
    }
    f1 << endl;

    for (int i = 0; i < years; i++) {
        for (int m = 0; m < 12; m++) {
            f1 << 1971 + i << "; " << m + 1 << "; ";

            // Количетсво точек (x,y)
            int k = real[i].size() / 12;

            for (int t = -22; t <=23; t++) {
                double count = 0;
                for (int p = k * m; p < k * (m + 1); p++) {
                    if (real[i][p].temp < t) {
                        count += 1;
                    }
                }

                f1 << count / k << "; ";
            }

            f1 << endl;

            f2 << 1971 + i << "; " << m + 1 << "; ";

            double mean = 0;

            for (int p = k * m; p < k * (m + 1); p++) {
                mean += real[i][p].temp;    
            }

            f2 << mean / k << endl;
        }
    }

    f1.close();
}