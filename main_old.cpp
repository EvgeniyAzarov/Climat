#include <map>
#include <iomanip>

#include "LinearModel.h"

// const string pathData1 = "Data/OldData/10RCM 61-90/";
// const string pathData2 = "Data/OldData/10RCM 91-10/";

const string pathData1 = "Data/OldData/10RCM 61-90/";
const string pathData2 = "Data/OldData/10RCM 91-10/";

const string files = "Result files/";

vector<point> loadPointsFromFile(string filename) {

    vector<point> points;

    ifstream fin(filename);

    point p;

    while (fin >> p.t >> p.x >> p.y >> p.temp) {
        // cout << filename << ":   " <<
        //     p.t << " " << p.x << " " << p.y << " " << p.temp << endl;
        points.push_back(p);
    }

    fin.close();

    cout << filename << endl;

    return points;
}

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
    if (a.size() != b.size()) cout << "Size error";

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

    out << fixed << setprecision(2);

    out << "l1: " << l1 << endl << "l2: " << l2 << endl;

    for (int mon = 1; mon <= 12; mon++) {
        out << endl << "Month" << mon << "  l1: " <<
            months_l1[mon - 1] / (n / 12 ) << " \t l2: " <<
            sqrt(months_l2[mon - 1] / (n / 12));
    }

    return out.str();
}

void arithmeticMean(vector< vector<point> > data, vector<point> real, string dataName) {
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
	string dataName2,
    bool freeCoefficient) {

    ofstream fout;

    LinearModel lm1;
    LinearModel lm2;

    if (!freeCoefficient) {
        lm1.fit(data1, real1);
        lm2.fit(data2, real2);
    } else {
        lm1.fitWithFreeCoefficient(data1, real1);
        lm2.fitWithFreeCoefficient(data2, real2);
    }

    vector<point> pred11 = lm1.predict(data1);
    vector<point> pred12 = lm1.predict(data2);

    vector<point> pred22 = lm2.predict(data2);
    vector<point> pred21 = lm2.predict(data1);

    string mnk1 = files + (!freeCoefficient ? "SMNK/SMNK1/" : "MNK/MNK1/");
    string mnk2 = files + (!freeCoefficient ? "SMNK/SMNK2/" : "MNK/MNK2/");

    fout.open(mnk1 + "coef.txt");
    fout << toString(lm1.coef);
    fout.close();

    fout.open(mnk2 + "coef.txt");
    fout << toString(lm2.coef);
    fout.close();

    // МНК на основе первых данных

    fout.open(mnk1 + "prediction " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnk1 + "deviation " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnk1 + "prediction " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnk1 + "deviation " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnk2 + "prediction " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnk2 + "deviation " + dataName1 + ".txt");
    fout << deviation(pred21, real1);
    fout.close();


    fout.open(mnk2 + "prediction " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnk2 + "deviation " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
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

       // cout << x << " " << good / all << endl;
        fout << x << " " << good / all << endl;
    }

    fout.close();
}

void mnkMonths(vector< vector<point> > data1,
    vector<point> real1,
    string dataName1,
    vector< vector<point> > data2,
    vector<point> real2,
    string dataName2,
    bool freeCoefficient) {

    string mnk1 = files + (!freeCoefficient ? "SMNK M/SMNK1 M/" : "MNK M/MNK1 M/");
    string mnk2 = files + (!freeCoefficient ? "SMNK M/SMNK2 M/" : "MNK M/MNK2 M/");
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

        if (!freeCoefficient) {
            lm2[i].fit(data2_i, real2_i);
            lm1[i].fit(data1_i, real1_i);   
        } else {
            lm2[i].fitWithFreeCoefficient(data2_i, real2_i);
            lm1[i].fitWithFreeCoefficient(data1_i, real1_i);
        }

        vector<point> pred11_i = lm1[i].predict(data1_i);
        vector<point> pred12_i = lm1[i].predict(data2_i);

        vector<point> pred22_i = lm2[i].predict(data2_i);
        vector<point> pred21_i = lm2[i].predict(data1_i);

        pred11.insert(pred11.end(), pred11_i.begin(), pred11_i.end());
        pred12.insert(pred12.end(), pred12_i.begin(), pred12_i.end());
        pred22.insert(pred22.end(), pred22_i.begin(), pred22_i.end());
        pred21.insert(pred21.end(), pred21_i.begin(), pred21_i.end());
    }

    fout.open(mnk1 + "coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm1[i].coef) << endl;
    fout.close();

    fout.open(mnk2 + "coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm2[i].coef) << endl;
    fout.close();

    // МНК на основе первых данных

    fout.open(mnk1 + "prediction " + dataName1 + ".txt");
    fout << toString(pred11);
    fout.close();

    fout.open(mnk1 + "deviation " + dataName1 + ".txt");
    fout << deviation(pred11, real1);
    fout.close();


    fout.open(mnk1 + "prediction " + dataName2 + ".txt");
    fout << toString(pred12);
    fout.close();

    fout.open(mnk1 + "deviation " + dataName2 + ".txt");
    fout << deviation(pred12, real2);
    fout.close();

    // МНК на основе вторых данных

    fout.open(mnk2 + "prediction " + dataName1 + ".txt");
    fout << toString(pred21);
    fout.close();

    fout.open(mnk2 + "deviation " + dataName1 + ".txt");
    fout << deviation(pred21, real1);
    fout.close();


    fout.open(mnk2 + "prediction " + dataName2 + ".txt");
    fout << toString(pred22);
    fout.close();

    fout.open(mnk2 + "deviation " + dataName2 + ".txt");
    fout << deviation(pred22, real2);
    fout.close();
}

void deltaMnk(vector< vector<point> > data1,
    vector<point> real1,
    vector< vector<point> > data2,
    vector<point> real2,
    bool freeCoefficient) {

    string deltaMnkPath = (!freeCoefficient ? 
        "Result files/Delta MNK/" : "Result files/Delta MNK with free coef/");
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

    if(!freeCoefficient) {
        lm.fit(delta, deltaReal);
    } else {
        lm.fitWithFreeCoefficient(delta, deltaReal);
    }

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

void deltaMnkMonths(vector< vector<point> > data1,
    vector<point> real1,
    vector< vector<point> > data2,
    vector<point> real2,
    bool freeCoefficient) {

    string deltaMnkMonthsPath = (!freeCoefficient ? 
        "Result files/Delta MNK months/" : "Result files/Delta MNK months with free coef/");
    ofstream fout;

    vector< vector<point> > delta(data2);
    vector<point> deltaReal(real2);

    for (int i = 0; i < delta.size(); i++) {
        for (int j = 0; j < delta[0].size(); j++) {
            delta[i][j].temp -= data1[i][j].temp;
        }

        deltaReal[i].temp -= real1[i].temp;
    }

    vector<LinearModel> lm(12);

    vector<point> pred;

    int m = delta.size() / 12;

    for (int i = 0; i < 12; i++) {
        vector< vector<point> > delta_i(delta.begin() + i * m,
                                        delta.begin() + (i + 1) * m);
        vector<point> real_i(deltaReal.begin() + i * m,
                                deltaReal.begin() + (i + 1) * m);

        if (!freeCoefficient) {
            lm[i].fit(delta_i, real_i);
        } else {
            lm[i].fitWithFreeCoefficient(delta_i, real_i);
        }

        vector<point> pred_i = lm[i].predict(delta_i);

        pred.insert(pred.end(), pred_i.begin(), pred_i.end());
    }

    for (int i = 0; i < pred.size(); i++) {
        pred[i].temp += real1[i].temp;
    }

    fout.open(deltaMnkMonthsPath + "/coef.txt");
    for (int i = 0; i < 12; i++) fout << toString(lm[i].coef) << endl;
    fout.close();

    fout.open(deltaMnkMonthsPath + "deviation.txt");
    fout << deviation(pred, real2);
    fout.close();


    fout.open(deltaMnkMonthsPath + "prediction.txt");
    fout << toString(pred);
    fout.close();
}

void deltaMean(vector< vector<point> > data1,
    vector<point> real1,
    vector< vector<point> > data2,
    vector<point> real2) {

    string deltaMeanPath = "Result files/Delta mean/";
    ofstream fout;

    vector<point> pred(real2);

    for (int i = 0; i < data1.size(); i++) {

        double sum = 0;
        int num = 0;

        for (int j = 0; j < data1[0].size(); j++) {
            sum += data2[i][j].temp - data1[i][j].temp;
            num++;
        }

        // !!! Неявное обнуление
        pred[i].temp = real1[i].temp + (sum / num);
    }

    fout.open(deltaMeanPath + "deviation.txt");
    fout << deviation(pred, real2);
    fout.close();

    fout.open(deltaMeanPath + "prediction.txt");
    fout << toString(pred);
    fout.close();
}

void mnkRegions(vector< vector<point> > data1,
    vector<point> real1,
    string dataName1,
    vector< vector<point> > data2,
    vector<point> real2,
    string dataName2,
    bool freeCoefficient) {

    string mnk1 = files + (!freeCoefficient ? "SMNK R/SMNK1 R/" : "MNK R/MNK1 R/");
    string mnk2 = files + (!freeCoefficient ? "SMNK R/SMNK2 R/" : "MNK R/MNK2 R/");
    ofstream fout;

    ifstream fin;

    // vector< vector<point> > regions(5);
    // vector<string> regionsName = {"center.txt", "east.txt", "north.txt", "south.txt", "west.txt"};
    // for(int i = 0; i < 5; i++) {
    //     fim.open("Data/regions/" + regionsName[i]);

    //     // Тут будем пользоваться тем, что координаты в файлах упорядочены

    //     point p;

    //     while (fin >> p.x >> p.y) {
    //         regions[i].push_back(p);
    //     }

    //     fin.close();
    // }
}

int main() {
    // Количество прогнозов
    // int n;
    // cout << "Number of predictions: ";
    // cin >> n;
    int n = 10;
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

    //Первая координата -- номер  точки, вторая -- номер прогноза

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
    mnk(data1, real1, "61-90", data2, real2, "91-10", false);
    cout << "Done." << endl;

    cout << "Predicting with MNK with free coefficient... ";
    mnk(data1, real1, "61-90", data2, real2, "91-10", true);
    cout << "Done." << endl;

    cout << "Predicting with MNK for months... ";
    mnkMonths(data1, real1, "61-90", data2, real2, "91-10", false);
    cout << "Done" << endl;

    cout << "Predicting with MNK for months with free coefficient... ";
    mnkMonths(data1, real1, "61-90", data2, real2, "91-10", true);
    cout << "Done" << endl;

    cout << "Predicting with delta MNK... ";
    deltaMnk(data1, real1, data2, real2, false);
    cout << "Done." << endl; 

    cout << "Predicting with delta MNK with free coef... ";
    deltaMnk(data1, real1, data2, real2, true);
    cout << "Done." << endl;

    cout << "Predicting with delta MNK months... ";
    deltaMnkMonths(data1, real1, data2, real2, false);
    cout << "Done." << endl;    

    cout << "Predicting with delta MNK months... ";
    deltaMnkMonths(data1, real1, data2, real2, true);
    cout << "Done." << endl;  

    cout << "Predicting with delta mean... ";
    deltaMean(data1, real1, data2, real2);
    cout << "Done." << endl;
}
