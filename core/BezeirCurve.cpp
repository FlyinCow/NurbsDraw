#include "core/BezeirCurve.h"
#include "core/BernsteinBase.h"
#include "core/Vector.h"
#include <vector>

BezeirCurve::BezeirCurve(const std::vector<Vector> &vertices) : vertices(vertices) {
}

void BezeirCurve::setVertices(const std::vector<Vector> &vertices) {
    this->vertices = vertices;
}

Vector BezeirCurve::eval(double t) const {
    Vector res{};
    int n = vertices.size() - 1;
    for (auto i = 0; i <= n; i++) {
        res += B(i, n)(t) * vertices[i];
    }
    return res;
}
