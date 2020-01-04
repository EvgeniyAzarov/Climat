#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <locale>

#include "LinearModel.h"

using namespace std;

const string pathData = "Data/NewData/years/";

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

	for (int i = 0; i < years; i++) {
    	//Первая координата -- номер  точки, вторая -- номер прогноза
    	data[i]=transpose(data[i]);
    	// printMatrix(data[i]);
    }

    // Теперь записываем это все в один большой файл

    ofstream fout(pathData + "temp1971-2010.csv");
    fout << years << endl;
    fout << n << endl;
    fout << data[0].size() << endl;

    for (int i = 0; i < years; i++) {
        for (int j = 0; j < data[0].size(); j++) {
            for (int f = 0; f < n; f++) {
                fout << data[i][j][f].t << " ; "
                    << data[i][j][f].x << " ; "
                    << data[i][j][f].y << " ; "
                    << data[i][j][f].temp << endl;
            }
        }
    }

    for (int i = 0; i < years; i++) {
        for (int j = 0; j < real[0].size(); j++) {
            fout << real[i][j].t << " ; "
                << real[i][j].x << " ; "
                << real[i][j].y << " ; "
                << real[i][j].temp << endl;
        }
    }

    fout.close();
}