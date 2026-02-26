#pragma once
#include <cassert>
#include <format>
/**
 3-dimention vector.
 needed:
 - construction from components (x,y,z)
 - default construction (0,0,0)
 - copy construction
 - copy assignment
 - multify by real number
 - dot
 - add
 - inner production
*/
class Vector {
public:
    constexpr Vector(double x, double y, double z) noexcept {
        coord[0] = x;
        coord[1] = y;
        coord[2] = z;
    };
    constexpr Vector() noexcept : Vector(0, 0, 0) {
    }
    Vector(const Vector &other) noexcept;
    Vector &operator=(const Vector &other);

    constexpr auto &&x(this auto &&self) noexcept {
        return self.coord[0];
    }
    constexpr auto &&y(this auto &&self) noexcept {
        return self.coord[1];
    }
    constexpr auto &&z(this auto &&self) noexcept {
        return self.coord[2];
    }

    constexpr auto &&coordinate(this auto &&self) noexcept {
        return self.coord;
    }

    Vector &operator+=(const Vector &other);
    friend Vector operator+(Vector lhs, const Vector &rhs) {
        lhs += rhs;
        return lhs;
    }

    Vector &operator*=(double k);
    friend Vector operator*(Vector v, double k) {
        v *= k;
        return v;
    }
    friend Vector operator*(double k, Vector v) {
        v *= k;
        return v;
    }

    Vector operator-() const;

    friend Vector operator-(Vector lhs, const Vector &rhs) {
        lhs += -rhs;
        return lhs;
    }

    double square_norm() const noexcept;
    double norm() const noexcept;
    double dot(Vector other) const noexcept;

private:
    double coord[3];
};

template <>
struct std::formatter<Vector> {

    enum class __bracket_type {
        None,
        Parenthese,
        Square,
        Curly
    };

    constexpr auto parse(std::format_parse_context &ctx) {
        auto pos = ctx.begin();
        while (pos != ctx.end() && *pos != '}') {
            // TODO: parse specification
            ++pos;
        }
        return pos;
    }
    auto format(const Vector &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "{} {} {}", v.x(), v.y(), v.z());
    }
};