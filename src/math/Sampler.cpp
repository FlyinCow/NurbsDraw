#include "Sampler.h"
#include "Vector.h"
#include "concepts.h"
#include <vector>

std::vector<Vec2d> Curve2dSampler::sample(Curve2dProxy c) {
    std::vector<Vec2d> points;
    this->sample_to(c, points);
    return points;
}

int Curve2dSampler::sample_to(Curve2dProxy c, std::vector<Vec2d> &points) {
    points.reserve(this->n_segements);
    auto [low_bound, high_bound] = c->get_range();
    auto step = (high_bound - low_bound) / this->n_segements;
    auto t = low_bound;

    // [TODO]: stablize float point value here
    while (t < high_bound) {
        points.emplace_back(c->eval(t));
        t += step;
    }
    points.emplace_back(c->eval(t > high_bound ? high_bound : t));
    return points.size();
}
