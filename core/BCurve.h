#pragma once
#include "Vector.h"
#include <vector>

class BCurve {
public:
    BCurve(int degree, const std::vector<Vector> &vertices, const std::vector<double> &knots, const std::vector<int> &mults);
    BCurve(int degree, const std::vector<Vector> &vertices, const std::vector<double> weights, const std::vector<double> &knots, const std::vector<int> &mults);

private:
    void update_flat_knots();

private:
    int degree;
    std::vector<Vector> vertices;
    std::vector<double> knots;
    std::vector<double> weights;
    std::vector<int> mults;
    std::vector<double> flat_knots;
    double range[2];
};