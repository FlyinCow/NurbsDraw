#pragma once
#include "math/Vector.h"
#include <utility>
#include <vector>

// 极简 B-spline：只保留核心 eval 功能
class BCurve2 {
public:
    // 构造函数：degree + 控制点 + unique knots + knot multiplicities
    BCurve2(int degree, const std::vector<Vec2d> &vertices,
            const std::vector<double> &knots, const std::vector<int> &mults);

    // 基于 Cox-de Boor 算法求值
    Vec2d eval(double t) const;

    // Getters
    int degree() const {
        return degree_;
    }
    inline const std::vector<Vec2d> &vertices() const noexcept {
        return vertices_;
    }

    inline const std::vector<double> &knots() const noexcept {
        return knots_;
    }
    inline const std::vector<int> &mults() const noexcept {
        return mults_;
    }

    // 获取曲线的参数范围
    double t_min() const {
        return flat_knots_[degree_];
    }
    double t_max() const {
        return flat_knots_[flat_knots_.size() - degree_ - 1];
    }

    inline std::pair<double, double> get_range() const {
        return {t_min(), t_max()};
    }

    void clamp();

private:
    int degree_;
    std::vector<Vec2d> vertices_;
    std::vector<double> knots_;
    std::vector<int> mults_;
    std::vector<double> flat_knots_; // 展开后的knot序列
};

