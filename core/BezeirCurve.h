#pragma once

#include "Vector.h"
#include <vector>

class BezeirCurve {
public:
    BezeirCurve(std::vector<Vector> &vetices);

    Vector eval(double t);

private:
    int degree;
    std::vector<Vector> vertices;
};