#pragma once

#include "math/Vector.h"
#include <cassert>
#include <vector>

class BezeirCurve {
public:
    BezeirCurve(const std::vector<Vector> &vetices);
    void setVertices(const std::vector<Vector> &vertices);
    Vector eval(double t) const;
    auto &&vertex(this auto &&self, int i) {
        assert(i < self.vertices.size());
        assert(i >= 0);
        return self.vertices[i];
    }

private:
    std::vector<Vector> vertices;
};