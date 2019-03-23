#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <map>

#include "LinearModel.h"

const string pathData1 = "Data/10RCM 61-90/";
const string pathData2 = "Data/10RCM 91-10/";

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

void mnkWithFreeCoefficient(vector< vector<point> > data1,
         vector<point> real1,
         string dataName1,
         vector< vector<point> > data2,
         vector<point> real2,
         string dataName2) {

    string mnkWithFreeCoefPath = "Result files/MNK with free coef/";
    ofstream fout;

    LinearModel lm1;
    LinearModel lm2;

    lm1.fitWithFreeCoefficient(data1, real1);
    lm2.fitWithFreeCoefficient(data2, real2);

    vector<point> pred11 = lm1.predict(data1);
    vector<point> pred12 = lm1.predict(data2);

    vector<point> pred22 = lm2.predict(data2);
    vector<point> pred21 = lm2.predict(data1);


    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName1 + "/coef.txt");
    fout << toString(lm1.coef);
    fout.close();

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName2 + "/coef.txt");
    fout << toString(lm2.coef);
    fout.close();

    // МНК на основе первых данных

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
    fout.close();


    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnkWithFreeCoefPath + "Prediction based on " + dataName2 +
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

void mnkMonths(vector< vector<point> > data1,
         vector<point> real1,
         string dataName1,
         vector< vector<point> > data2,
         vector<point> real2,
         string dataName2) {

    string mnkMonthsPath = "Result files/MNK months/";
    ofstream fout;

    vector<LinearModel> lm1(12);
    vector<LinearModel> lm2(12);

    vector<point> pred11;
    vector<point> pred12;

    vector<point> pred22;
    vector<point> pred21;

    int m1 = data1.size() / 12;
    int m2 = data2.size() / 12;

    for (int i = 0; i < 12; i++) {
        vector< vector<point> > data1_i(data1.begin() + i * m1,
                                        data1.begin() + (i + 1) * m1);
        vector<point> real1_i(real1.begin() + i * m1,
                                real1.begin() + (i + 1) * m1);

        vector< vector<point> > data2_i(data2.begin() + i * m2,
                                        data2.begin() + (i + 1) * m2);
        vector<point> real2_i(real2.begin() + i * m2,
                                real2.begin() + (i + 1) * m2);


        lm1[i].fit(data1_i, real1_i);
        lm2[i].fit(data2_i, real2_i);

        vector<point> pred11_i = lm1[i].predict(data1_i);
        vector<point> pred12_i = lm1[i].predict(data2_i);

        vector<point> pred22_i = lm2[i].predict(data2_i);
        vector<point> pred21_i = lm2[i].predict(data1_i);

        pred11.insert(pred11.end(), pred11_i.begin(), pred11_i.end());
        pred12.insert(pred12.end(), pred12_i.begin(), pred12_i.end());
        pred22.insert(pred22.end(), pred22_i.begin(), pred22_i.end());
        pred21.insert(pred21.end(), pred21_i.begin(), pred21_i.end());
    }

    fout.open(mnkMonthsPath + "Prediction based on " + dataName1 + "/coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm1[i].coef) << endl;
    fout.close();

    fout.open(mnkMonthsPath + "Prediction based on " + dataName2 + "/coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm2[i].coef) << endl;
    fout.close();

    // МНК на основе первых данных

    fout.open(mnkMonthsPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnkMonthsPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnkMonthsPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnkMonthsPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnkMonthsPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnkMonthsPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
    fout.close();


    fout.open(mnkMonthsPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnkMonthsPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred21, real1);
    fout.close();

}

void mnkMonthsWithFreeCoefficient(vector< vector<point> > data1,
         vector<point> real1,
         string dataName1,
         vector< vector<point> > data2,
         vector<point> real2,
         string dataName2) {

    string mnkMonthsWithFreeCoefPath = "Result files/MNK months with free coef/";
    ofstream fout;

    vector<LinearModel> lm1(12);
    vector<LinearModel> lm2(12);

    vector<point> pred11;
    vector<point> pred12;

    vector<point> pred22;
    vector<point> pred21;

    int m1 = data1.size() / 12;
    int m2 = data2.size() / 12;

    for (int i = 0; i < 12; i++) {
        vector< vector<point> > data1_i(data1.begin() + i * m1,
                                        data1.begin() + (i + 1) * m1);
        vector<point> real1_i(real1.begin() + i * m1,
                                real1.begin() + (i + 1) * m1);

        vector< vector<point> > data2_i(data2.begin() + i * m2,
                                        data2.begin() + (i + 1) * m2);
        vector<point> real2_i(real2.begin() + i * m2,
                                real2.begin() + (i + 1) * m2);


        lm1[i].fitWithFreeCoefficient(data1_i, real1_i);
        lm2[i].fitWithFreeCoefficient(data2_i, real2_i);

        vector<point> pred11_i = lm1[i].predict(data1_i);
        vector<point> pred12_i = lm1[i].predict(data2_i);

        vector<point> pred22_i = lm2[i].predict(data2_i);
        vector<point> pred21_i = lm2[i].predict(data1_i);

        pred11.insert(pred11.end(), pred11_i.begin(), pred11_i.end());
        pred12.insert(pred12.end(), pred12_i.begin(), pred12_i.end());
        pred22.insert(pred22.end(), pred22_i.begin(), pred22_i.end());
        pred21.insert(pred21.end(), pred21_i.begin(), pred21_i.end());
    }

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName1 + "/coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm1[i].coef) << endl;
    fout.close();

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName2 + "/coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm2[i].coef) << endl;
    fout.close();

    // МНК на основе первых данных

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName1 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
    fout.close();


    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/prediction for " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnkMonthsWithFreeCoefPath + "Prediction based on " + dataName2 +
                "/deviation of prediction for " + dataName1 + ".txt");
    fout << deviation(pred21, real1);
    fout.close();

}

void deltaMnk(vector< vector<point> > data1,
         vector<point> real1,
         vector< vector<point> > data2,
         vector<point> real2) {

    string deltaMnkPath = "Result files/Delta MNK/";
    ofstream fout;

    vector< vector<point> > delta(data2);
    vector<point> deltaReal(real2);

    for (int i = 0; i < delta.size(); i++) {
        for (int j = 0; j < delta[0].size(); j++) {
            delta[i][j].temp -= data1[i][j].temp;
        }

        deltaReal[i].temp -= real1[i].temp;
    }

    LinearModel lm;

    lm.fit(delta, deltaReal);

    vector<point> pred = lm.predict(delta);

    for (int i = 0; i < pred.size(); i++) {
        pred[i].temp += real1[i].temp;
    }

    fout.open(deltaMnkPath + "coef.txt");
    fout << toString(lm.coef) << endl;
    fout.close();

    fout.open(deltaMnkPath + "deviation.txt");
    fout << deviation(pred, real2);
    fout.close();


    fout.open(deltaMnkPath + "prediction.txt");
    fout << toString(pred);
    fout.close();
}

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
            loadPointsFromFile(pathData1 + to_string(i) + ".txt");

        data2[i] =
            loadPointsFromFile(pathData2 + to_string(i) + ".txt");
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

    cout << "Predicting with MNK with free coefficient... ";
    mnkWithFreeCoefficient(data1, real1, "61-90", data2, real2, "91-10");
    cout << "Done." << endl;

    cout << "Predicting with MNK for months... ";
    mnkMonths(data1, real1, "61-90", data2, real2, "91-10");
    cout << "Done" << endl;

    cout << "Predicting with MNK for months with free coefficient... ";
    mnkMonthsWithFreeCoefficient(data1, real1, "61-90", data2, real2, "91-10");
    cout << "Done" << endl;

    cout << "Predicting with delta MNK... ";
    deltaMnk(data1, real1, data2, real2);
    cout << "Done." << endl;
}
