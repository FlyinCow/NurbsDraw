#pragma once
#include "Vector.h"

class Point {
public:
    constexpr Point(double x, double y, double z) noexcept : position(x, y, z) {};
    constexpr Point() noexcept : Point(0, 0, 0) {};

    constexpr auto &&x(this auto &&self) noexcept {
        return self.position.x();
    }

    constexpr auto &&y(this auto &&self) noexcept {
        return self.position.x();
    }

    constexpr auto &&z(this auto &&self) noexcept {
        return self.position.x();
    }

private:
    Vector position;
};