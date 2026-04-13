#pragma once

#include "math/Vector.h"
#include "math/concepts.h"
#include <vector>

class Curve2dSampler {
public:
    std::vector<Vec2d> sample(Curve2Proxy c);
    int sample_to(Curve2Proxy c, std::vector<Vec2d> &points);

private:
    int n_segements;
};