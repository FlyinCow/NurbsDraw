#pragma once

#include "core/Vector.h"
#include <vector>

class BezeirCurve {
public:
    BezeirCurve(const std::vector<Vector> &vetices);
    void setVertices(const std::vector<Vector> &vertices);
    Vector eval(double t) const;

private:
    std::vector<Vector> vertices;
};