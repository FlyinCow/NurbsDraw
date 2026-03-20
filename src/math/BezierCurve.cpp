#include "BezierCurve.h"
#include "BernsteinBase.h"
#include "Vector.h"
#include <vector>

BezierCurve::BezierCurve(const std::vector<Vec<>> &vertices) : vertices(vertices) {
}

void BezierCurve::set_vertices(const std::vector<Vec<>> &vertices) {
    this->vertices = vertices;
}

Vec<> BezierCurve::eval(double t) const {
    Vec<> res{};
    int n = vertices.size() - 1;
    for (auto i = 0; i <= n; i++) {
        res += B(i, n)(t) * vertices[i];
    }
    return res;
}

std::vector<Vec<>> sample_bezier_uniform(const BezierCurve &curve, int segments) {
    assert(segments >= 1);
    std::vector<Vec<>> pts;
    pts.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        pts.push_back(curve.eval(t));
    }
    return pts;
}
