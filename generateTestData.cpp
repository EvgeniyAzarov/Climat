#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

int main() {
    int n;
    cout << "Enter number of predictions: ";
    cin >> n;

    vector<double> values(n + 1);


    ofstream fout;
    ifstream fin;

    cout << "Enter values for 61-90: ";
    for (int i = 0; i <= n; i++) {
        double value;
        cin >> values[i];
    }

    for (int i = 0; i <= n; i++) {
        cout << values[i] << " ";
    }
    cout << endl;

    fin.open("Data/10RCM 61-90/0.txt");
    for (int i = 0; i <= n; i++) {
        fout.open("TestData/10RCM 61-90/" + to_string(i) + ".txt");
        if (fout.is_open()) {
            cout << "TestData/10RCM 61-90/" + to_string(i) + ".txt" << endl;
        }

        int t;
        double x, y, temp;

        while (fin >> t >> x >> y >> temp) {
            fout << t << " " << x << " " << y << " " << values[i] << endl;
        }

        fout.close();

        fin.clear();
        fin.seekg(0);
    }
    fin.close();


    cout << "Enter values for 91-10: ";
    for (int i = 0; i <= n; i++) {
        double value;
        cin >> values[i];
    }

    fin.open("Data/10RCM 91-10/0.txt");
    for (int i = 0; i <= n; i++) {
        fout.open("TestData/10RCM 91-10/" + to_string(i) + ".txt");
        cout << "TestData/10RCM 91-10/" + to_string(i) + ".txt" << endl;

        int t;
        double x, y, temp;

        while (fin >> t >> x >> y >> temp) {
            fout << t << " " << x << " " << y << " " << values[i] << endl;
        }

        fout.close();
    }
    fin.close();
}
