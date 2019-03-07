#include <cstdlib>

#include "LinearModel.h"

vector< vector<double> > LinearModel::transpose(vector< vector<double> > X) {

    vector< vector<double> > Xt(X[0].size(), vector<double>(X.size()));

    for (unsigned int i = 0; i < X[0].size(); i++) {
        for (unsigned int j = 0; j < X.size(); j++) {
            Xt[i][j] = X[j][i];
        }
    }

    return Xt;
}

double LinearModel::scalarMult(vector<double> x, vector<double> y) {
    if (x.size() != y.size()) {
        cout << endl << "Ошибка размерности векторов" <<endl;
        exit(1);
    }

    double res = 0;

    for (unsigned int i = 0; i < x.size(); i++) {
        res += (x[i] * y[i]);
    }

    return res;
}

vector< vector<double> > LinearModel::mult(vector< vector<double> > A,
        vector< vector<double> > B) {

    if (A[0].size() != B.size()) {
        cout << endl << "Ошибка размеров матриц" <<endl;
        exit(1);
    }

    B = transpose(B);

    vector< vector<double> > AB(A.size(), vector<double>(B.size()));

    for (unsigned int i = 0; i < A.size(); i++) {
        for (unsigned int j = 0; j < B.size(); j++) {
            AB[i][j] = scalarMult(A[i], B[j]);
        }
    }

    return AB;
}

vector<double> LinearModel::mult(vector< vector<double> > A,
                                 vector<double> b) {

    if (A[0].size() != b.size()) {
        cout << endl << "Ошибка размеров матриц" <<endl;
        exit(1);
    }

    vector<double> Ab(A.size());

    for (unsigned int i = 0; i < A.size(); i++) {
        Ab[i] = scalarMult(A[i], b);
    }

    return Ab;
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

void LinearModel::fit(vector< vector<point> > Xp,
                      vector<point> yp) {

    vector< vector<double> > X(Xp.size(), vector<double>(Xp[0].size()));
    vector<double> y(yp.size());

    for (int i = 0; i < (int) Xp.size(); i++) {
        for (int j = 0; j < (int) Xp[0].size(); j++) {
            X[i][j] = Xp[i][j].temp;
        }
        y[i] = yp[i].temp;

        // Добавляем свободный член регрессии
        X[i].push_back(1);
    }

    y = mult(transpose(X), y);
    X = mult(transpose(X), X);

    for (int i = 0; i < (int) X.size(); i++) {
        X[i].push_back(y[i]);
    }

    gauss(X, coef);
}

vector<point> LinearModel::predict(vector< vector<point> > input) {

    vector<point> res(input.size());

    for (int i = 0; i < (int) input.size(); i++) {

        res[i] = input[i][0];

        // Сразу добавляем константу
        res[i].temp = coef[input[0].size()];

        for (int j = 0; j < (int) input[0].size(); j++) {
            res[i].temp += input[i][j].temp * coef[j];
        }
    }

    return res;
}
