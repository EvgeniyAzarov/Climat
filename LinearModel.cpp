#include <cstdlib>

#include "LinearModel.h"

void printMatrix(vector< vector<double> > X) {
    cout << endl;

    for (unsigned int i = 0; i < X.size(); i++) {
        for (unsigned int j = 0; j < X[0].size(); j++) {
            cout << X[i][j] << "  ";
        }
        cout << endl;
    }

    cout << endl;
}

int LinearModel::gauss (vector < vector<double> > a, vector<double> & ans) {
    double EPS = 0.000001;

    int n = (int) a.size();
    int m = (int) a[0].size() - 1;

    vector<int> where (m, -1);
    for (int col = 0, row = 0; col < m && row < n; ++col) {
        int sel = row;

        for (int i = row; i < n; ++i) {
            if (abs(a[i][col]) > abs(a[sel][col])) {
                sel = i;
            }
        }

        if (abs(a[sel][col]) < EPS) continue;

        for (int i = col; i <= m; ++i) {
            swap (a[sel][i], a[row][i]);
        }
        where[col] = row;

        for (int i = 0; i < n; ++i)
            if (i != row) {
                double c = a[i][col] / a[row][col];

                for (int j = col; j <= m; ++j) {
                    a[i][j] -= a[row][j] * c;
                }
            }
        row++;
    }

    ans.assign (m, 0);
    for (int i = 0; i < m; ++i) {
        if (where[i] != -1) {
            ans[i] = a[where[i]][m] / a[where[i]][i];
        }
    }

    for (int i = 0; i < n; ++i) {
        double sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += ans[j] * a[i][j];
        }
        if (abs (sum - a[i][m]) > EPS) {
            return 0;
        }
    }

    for (int i = 0; i < m; ++i) {
        if (where[i] == -1) {
            return -1;
        }
    }

    return 1;
}

// ToDo переименовать переменные, в соответствии с нормальными обозначениями
void LinearModel::fit(vector< vector<point> > X,
                      vector<point> y) {

    int n = X[0].size() - 1; // Количество прогнозов - 1
    int m = X.size(); // Количество точек в одном прогнозе

    vector< vector<double> > a(n, vector<double>(n + 1));

    for (int i = 0; i < n; i++) {
        // Коэффициенты СЛУ
        for (int j = 0; j < n; j++) {
            double value = 0;

            for (int k = 0; k < m; k++) {
                value += (X[k][n].temp - X[k][i].temp) *
                            (X[k][n].temp - X[k][j].temp);
            }

            a[i][j] = value;
        }

        // Свободные члены СЛУ
        double value = 0;

        for (int k = 0; k < m; k++) {
            value += (X[k][n].temp - X[k][i].temp)*(X[k][n].temp - y[k].temp);
        }

        a[i][n] = value;

    }

    gauss(a, coef);

    double lastCoef = 1;

    for (const auto& c : coef) lastCoef -= c;

    coef.push_back(lastCoef);
}

void LinearModel::fitWithFreeCoefficient(vector< vector<point> > X,
                      vector<point> y) {

    // Добавляем свободный коэффициент
    point tmp = {0, 0, 0, 1};
    for (int i = 0; i < (int) X.size(); i++) {
        X[i].push_back(tmp);
    }

    int k = X[0].size(); // Количество прогнозов
    int n = X.size(); // Количество точек в одном прогнозе

    vector< vector<double> > a(k, vector<double>(k + 1));

    for (int i = 0; i < k; i++) {

        // Коэффициенты СЛУ
        for (int j = 0; j < k; j++) {
            double value = 0;

            for (int t = 0; t < n; t++) {
                value += X[t][i].temp * X[t][j].temp;
            }

            a[i][j] = value;
        }

        // Сводбодные члены СЛУ
        double value = 0;

        for (int t = 0; t < n; t++) {
            value += X[t][i].temp * y[t].temp;
        }

        a[i][k] = value;
    }

//    printMatrix(a);

    gauss(a, coef);
}

vector<point> LinearModel::predict(vector< vector<point> > input) {

    vector<point> res(input.size());

    for (int i = 0; i < (int) input.size(); i++) {

        res[i] = input[i][0];
        res[i].temp = 0;

        for (int j = 0; j < (int) input[0].size(); j++) {
            res[i].temp += input[i][j].temp * coef[j];
        }

        // Свободный коэффициент
        if (coef.size() > input[0].size()) {
            res[i].temp += coef[input[0].size()];
        }
    }

    return res;
}
