#pragma once
#include "math/Vector.h"
#include <vector>

// 极简 B-spline：只保留核心 eval 功能
class BCurve {
public:
    // 构造函数：degree + 控制点 + unique knots + knot multiplicities
    BCurve(int degree, const std::vector<Vec3d>& vertices,
           const std::vector<double>& knots, const std::vector<int>& mults);

    // 基于 Cox-de Boor 算法求值
    Vec3d eval(double t) const;

    // Getters
    int degree() const { return degree_; }
    const std::vector<Vec3d>& vertices() const { return vertices_; }
    const std::vector<double>& knots() const { return knots_; }
    const std::vector<int>& mults() const { return mults_; }

    // 获取曲线的参数范围
    double t_min() const { return flat_knots_[degree_]; }
    double t_max() const { return flat_knots_[flat_knots_.size() - degree_ - 1]; }

private:
    int degree_;
    std::vector<Vec3d> vertices_;
    std::vector<double> knots_;
    std::vector<int> mults_;
    std::vector<double> flat_knots_;  // 展开后的knot序列
};

// 生成clamped knot向量（端点重复degree+1次）
std::vector<double> generate_curve_knots_clamped(int degree, int num_control_points);

// 均匀采样曲线上点
std::vector<Vec3d> sample_bcurve_uniform(const BCurve& curve, int segments);
