#pragma once

#include "math/Vector.h"
#include <cassert>
#include <vector>

// corrected spelling: BezierCurve (not Bezeir)
class BezierCurve {
public:
    BezierCurve(const std::vector<Vector> &vertices);
    void set_vertices(const std::vector<Vector> &vertices);
    Vector eval(double t) const;
    auto &&vertex(this auto &&self, int i) {
        assert(i < self.vertices.size());
        assert(i >= 0);
        return self.vertices[i];
    }

private:
    std::vector<Vector> vertices;
};

// utility: uniformly sample points on the curve (segments >= 1)
std::vector<Vector> sample_bezier_uniform(const BezierCurve &curve, int segments);