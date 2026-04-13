#include "BCurve.h"
#include <cassert>
#include <algorithm>
#include <cmath>

BCurve::BCurve(int degree, const std::vector<Vec3d>& vertices,
               const std::vector<double>& knots, const std::vector<int>& mults)
    : degree_(degree), vertices_(vertices), knots_(knots), mults_(mults) {
    assert(degree > 0);
    assert(vertices.size() >= static_cast<size_t>(degree + 1));
    assert(knots.size() == mults.size());

    // 展开knot序列（按multiplicity展开）
    flat_knots_.clear();
    for (size_t i = 0; i < knots.size(); i++) {
        flat_knots_.insert(flat_knots_.end(), mults[i], knots[i]);
    }
}

// Cox-de Boor 基函数递归
static double basis_function(double t, int i, int p, const std::vector<double>& U) {
    if (p == 0) {
        if (t >= U[i] && t < U[i + 1]) return 1.0;
        return 0.0;
    }
    double left = 0.0, right = 0.0;
    if (U[i + p] != U[i])
        left = (t - U[i]) / (U[i + p] - U[i]) * basis_function(t, i, p - 1, U);
    if (U[i + p + 1] != U[i + 1])
        right = (U[i + p + 1] - t) / (U[i + p + 1] - U[i + 1]) * basis_function(t, i + 1, p - 1, U);
    return left + right;
}

Vec3d BCurve::eval(double t) const {
    // clamp到有效范围
    t = std::clamp(t, t_min(), t_max());
    if (t >= t_max()) return vertices_.back();

    int n = static_cast<int>(vertices_.size()) - 1;

    // 二分查找knot span
    int low = degree_, high = n + 1;
    while (low < high) {
        int mid = (low + high) / 2;
        if (t < flat_knots_[mid]) high = mid;
        else low = mid + 1;
    }
    int span = std::min(low - 1, n);

    // 计算基函数并求和
    Vec3d result{};
    for (int i = 0; i <= degree_; i++) {
        int idx = span - degree_ + i;
        if (idx < 0) idx = 0;
        if (idx > n) idx = n;
        double N = basis_function(t, idx, degree_, flat_knots_);
        result += N * vertices_[idx];
    }
    return result;
}

// clamped knot向量（端点重复degree+1次）
std::vector<double> generate_curve_knots_clamped(int degree, int num_control_points) {
    int m = num_control_points + degree + 1;
    std::vector<double> knots(m);
    for (int i = 0; i <= degree; i++) knots[i] = 0.0;
    for (int i = m - degree - 1; i < m; i++) knots[i] = 1.0;
    return knots;
}

// 均匀采样曲线上点
std::vector<Vec3d> sample_bcurve_uniform(const BCurve& curve, int segments) {
    assert(segments >= 1);
    std::vector<Vec3d> pts;
    pts.reserve(segments + 1);
    double t0 = curve.t_min(), t1 = curve.t_max();
    for (int i = 0; i <= segments; i++) {
        double t = t0 + (t1 - t0) * static_cast<double>(i) / segments;
        pts.push_back(curve.eval(t));
    }
    return pts;
}
