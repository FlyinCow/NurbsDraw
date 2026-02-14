#include "BCurve.h"
#include "Vector.h"
#include <cassert>

BCurve::BCurve(int degree, const std::vector<Vector> &vertices, const std::vector<double> &knots, const std::vector<int> &mults) : degree(degree), vertices(vertices), knots(knots), weights(), mults(mults) {
    assert(knots.size() == mults.size());
    update_flat_knots();
}

BCurve::BCurve(int degree, const std::vector<Vector> &vertices, const std::vector<double> weights, const std::vector<double> &knots, const std::vector<int> &mults) : degree(degree), vertices(vertices), knots(knots), weights(weights), mults(mults) {
    assert(knots.size() == mults.size());
    update_flat_knots();
}

void BCurve::update_flat_knots() {
    if (!flat_knots.empty()) {
        flat_knots.clear();
    }
    for (int i = 0; i < knots.size(); i++) {
        flat_knots.insert(flat_knots.end(), mults[i], knots[i]);
    }
}