#ifndef LINEARMODEL_H
#define LINEARMODEL_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct point {
    int t;
    double x;
    double y;
    double temp;
};

class LinearModel {
        private:
            int gauss (vector < vector<double> > a, vector<double> & ans);

        public:
            vector<double> coef;

            void fit(vector< vector<point> > X, vector<point> y, bool freeCoef = true);
            vector<point> predict(vector< vector<point> > input);
};

#endif // LINEARMODEL_H
