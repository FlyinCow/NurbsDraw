#pragma once

#include "math/Vector.h"
#include <cassert>
#include <vector>

// corrected spelling: BezierCurve (not Bezeir)
class BezierCurve {
public:
    BezierCurve(const std::vector<Vec<>> &vertices);
    void set_vertices(const std::vector<Vec<>> &vertices);
    Vec<> eval(double t) const;
    auto &&vertex(this auto &&self, int i) {
        assert(i < self.vertices.size());
        assert(i >= 0);
        return self.vertices[i];
    }

private:
    std::vector<Vec<>> vertices;
};

// utility: uniformly sample points on the curve (segments >= 1)
std::vector<Vec<>> sample_bezier_uniform(const BezierCurve &curve, int segments);