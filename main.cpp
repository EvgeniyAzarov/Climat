#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <map>

#include "LinearModel.h"

const string path_data1 = "Data/10RCM 61-90/";
const string path_data2 = "Data/10RCM 91-10/";

vector<point> loadPointsFromFile(string filename) {
    vector<point> points;

    ifstream fin(filename);

    point p;

    while (fin >> p.t >> p.x >> p.y >> p.temp) {
//        cout << filename << ":   " <<
//            p.t << " " << p.x << " " << p.y << " " << p.temp << endl;
        points.push_back(p);
    }

    fin.close();

    cout << filename << endl;

    return points;
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

string toString(vector<double> a) {
    stringstream sout;
    for (auto it = a.begin(); it != a.end(); it++) {
        sout << *it << " ";
    }

    sout << endl;

    return sout.str();
}

string toString(vector<point> a) {
    stringstream sout;
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
    int n = a.size();

    double l1 = 0;
    double l2 = 0;

    vector< double > months_l1(12, 0);
    vector< double > months_l2(12, 0);

    for (int i = 0; i < n; i++) {
        l1 += abs(a[i].temp - b[i].temp);
        l2 += (a[i].temp - b[i].temp) * (a[i].temp - b[i].temp);

        months_l1[a[i].t - 1] += abs(a[i].temp - b[i].temp);
        months_l2[a[i].t - 1] +=
            (a[i].temp - b[i].temp) * (a[i].temp - b[i].temp);
    }

    l1 /= n;

    l2 = sqrt(l2 / n);

    stringstream out;

    out << "l1: " << l1 << endl << "l2: " << l2 << endl;

    for (int mon = 1; mon <= 12; mon++) {
        out << endl << "Month" << mon << "  l1: " <<
            months_l1[mon - 1] / (n / 12 ) << " \t l2: " <<
            sqrt(months_l2[mon - 1] / (n / 12));
    }

    return out.str();
}

void arithmeticMean(vector< vector<point> > data,
         vector<point> real,
         string dataName) {
    ofstream fout;
    string avgPath = "Result files/Arithmetic mean/";

    vector<point> avg(data.size());

    for (int i = 0; i < data.size(); i++) {
        avg[i] = data[i][0];
        avg[i].temp = 0;

        for (int j = 0; j < data[0].size(); j++) {
            avg[i].temp += data[i][j].temp;
        }

        avg[i].temp /= data[0].size();
    }

    fout.open(avgPath + "AMean for " + dataName + ".txt");
    fout << toString(avg);
    fout.close();

    fout.open(avgPath + "deviation " + dataName + ".txt");
    fout << deviation(avg, real);
    fout.close();
}

void mnk(vector< vector<point> > data1,
         vector<point> real1,
         string dataName1,
         vector< vector<point> > data2,
         vector<point> real2,
         string dataName2) {

    string mnkPath = "Result files/MNK/";
    ofstream fout;

    LinearModel lm1;
    LinearModel lm2;

    lm1.fit(data1, real1);
    lm2.fit(data2, real2);

    vector<point> pred11 = lm1.predict(data1);
    vector<point> pred12 = lm1.predict(data2);

    vector<point> pred22 = lm2.predict(data2);
    vector<point> pred21 = lm2.predict(data1);


    fout.open(mnkPath + "Prediction based on " + dataName1 + "/coef.txt");
    fout << toString(lm1.coef);
    fout.close();

    fout.open(mnkPath + "Prediction based on " + dataName2 + "/coef.txt");
    fout << toString(lm2.coef);
    fout.close();

    // МНК на основе первых данных

    fout.open(mnkPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnkPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnkPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnkPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnkPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnkPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
    fout.close();


    fout.open(mnkPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnkPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred21, real1);
    fout.close();

}

void intervals(vector< vector<point> > data, string dataName) {
    string intervalsPath = "Result files/Confidence intervals/";
    ofstream fout;

    //Колебание в каждой точке
    vector<double> wobble(data.size());

    fout.open(intervalsPath + "Wobbles " + dataName + ".txt");

    for (int i = 0; i < (int) wobble.size(); i++) {
        double max = -1;
        double min = 100000;

        for (int j = 0; j < (int) data[0].size(); j++) {
            if (data[i][j].temp > max) max = data[i][j].temp;
            if (data[i][j].temp < min) min = data[i][j].temp;
        }

        wobble[i] = max - min;
        fout << data[i][0].t << " " << data[i][0].x << " "
             << data[i][0].y << " " << wobble[i] << endl;
    }

    // Считаем функцию распределения

    double all = (double) wobble.size();

    fout.open(intervalsPath + "Distribution function " + dataName + ".txt");

    for (double x = 0; x < 15; x += 0.1) {
        int good = 0;

        for (auto it = wobble.begin(); it != wobble.end(); it++) {
            if (*it < x) good++;
        }

//        cout << x << " " << good / all << endl;
        fout << x << " " << good / all << endl;
    }

    fout.close();
}

//TODO добавить МНК по месяцам

//TODO добавить МНК по секторам

int main() {
    // Количество прогнозов
    int n;
    cout << "Number of predictions: ";
    cin >> n;
    n++;

    vector< vector<point> > data1(n);
    vector< vector<point> > data2(n);

    cout << "Loading data... " << endl;

    for (int i = 0; i < n; i++) {
        data1[i] =
            loadPointsFromFile(path_data1 + to_string(i) + ".txt");

        data2[i] =
            loadPointsFromFile(path_data2 + to_string(i) + ".txt");
    }

    vector<point> real1 = data1[0];
    data1.erase(data1.begin());
    data1 = transpose(data1);

    vector<point> real2 = data2[0];
    data2.erase(data2.begin());
    data2 = transpose(data2);

    cout << "Done." << endl << endl;

    cout << "Counting arithmetic mean...";
    arithmeticMean(data1, real1, "61-90");
    arithmeticMean(data2, real2, "91-10");
    cout << "Done." << endl;

    cout << "Counting confidence intervals... ";
    intervals(data1, "61-90");
    intervals(data2, "91-10");
    cout << "Done." << endl;

    cout << "Predicting with MNK... ";
    mnk(data1, real1, "61-90", data2, real2, "91-10");
    cout << "Done." << endl;
}
