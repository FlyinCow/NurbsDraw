#include "BezierCurve.h"
#include "BernsteinBase.h"
#include "Vector.h"
#include <vector>

BezierCurve::BezierCurve(const std::vector<Vector> &vertices) : vertices(vertices) {
}

void BezierCurve::setVertices(const std::vector<Vector> &vertices) {
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
