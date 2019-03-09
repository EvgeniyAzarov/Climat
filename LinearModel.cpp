#include <cstdlib>

#include "LinearModel.h"

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

        // Сводбодные члены СЛУ
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

vector<point> LinearModel::predict(vector< vector<point> > input) {

    vector<point> res(input.size());

    for (int i = 0; i < (int) input.size(); i++) {

        res[i] = input[i][0];

        // Сразу добавляем константу
        // res[i].temp = coef[input[0].size()];

        for (int j = 0; j < (int) input[0].size(); j++) {
            res[i].temp += input[i][j].temp * coef[j];
        }
    }

    return res;
}
