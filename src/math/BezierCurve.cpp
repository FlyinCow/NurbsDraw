#include "BezierCurve.h"
#include "BernsteinBase.h"
#include "Vector.h"
#include <vector>

BezierCurve::BezierCurve(const std::vector<Vector> &vertices) : vertices(vertices) {
}

void BezierCurve::set_vertices(const std::vector<Vector> &vertices) {
    this->vertices = vertices;
}

Vector BezierCurve::eval(double t) const {
    Vector res{};
    int n = vertices.size() - 1;
    for (auto i = 0; i <= n; i++) {
        res += B(i, n)(t) * vertices[i];
    }
    return res;
}

std::vector<Vector> sample_bezier_uniform(const BezierCurve &curve, int segments) {
    assert(segments >= 1);
    std::vector<Vector> pts;
    pts.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        pts.push_back(curve.eval(t));
    }
    return pts;
}
