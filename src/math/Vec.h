#pragma once
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <cmath>

template <size_t Dim = 3, std::floating_point T = double>
    requires(Dim == 2 || Dim == 3)
class Vec {
public:
    using elem_type = T;
    template <std::convertible_to<T>... Args>
        requires(sizeof...(Args) == Dim)
    constexpr Vec(Args... args) noexcept : coord{static_cast<T>(args)...} {
    }

    explicit Vec(std::array<T, Dim> coord) noexcept : coord{coord} {
    }

    constexpr Vec() noexcept : coord{} {
    }

    constexpr Vec &operator=(Vec &&) = default;
    constexpr Vec &operator=(const Vec &) = default;
    constexpr Vec(const Vec &) = default;
    constexpr Vec(Vec &&) = default;

    constexpr auto &&x(this auto &&self) noexcept {
        return self.coord[0];
    }

    constexpr auto &&y(this auto &&self) noexcept {
        return self.coord[1];
    }

    constexpr auto &&z(this auto &&self) noexcept
        requires(Dim == 3)
    {
        return self.coord[2];
    }

    // [WARN]: unchecked accsess
    constexpr auto &&operator[](this auto &&self, size_t i) noexcept {
        return self.coord[i];
    }

    constexpr Vec &operator+=(const Vec &other) {
        for (size_t i = 0; i < Dim; i++) {
            coord[i] += other.coord[i];
        }
        return *this;
    }

    constexpr friend Vec operator+(Vec lhs, const Vec &rhs) {
        lhs += rhs;
        return lhs;
    }

    constexpr Vec &operator*=(T k) {
        for (size_t i = 0; i < Dim; i++) {
            coord[i] *= k;
        }
        return *this;
    }
    constexpr friend Vec operator*(Vec v, T k) {
        v *= k;
        return v;
    }
    constexpr friend Vec operator*(T k, Vec v) {
        v *= k;
        return v;
    }

    constexpr Vec &operator/=(T k) {
        for (size_t i = 0; i < Dim; i++) {
            coord[i] /= k;
        }
        return *this;
    }
    constexpr friend Vec operator/(Vec v, T k) {
        v /= k;
        return v;
    }

    constexpr Vec operator-() const {
        Vec result{};
        for (size_t i = 0; i < Dim; i++) {
            result[i] = -coord[i];
        }
        return result;
    }

    constexpr friend Vec operator-(Vec lhs, const Vec &rhs) {
        lhs += -rhs;
        return lhs;
    }

    constexpr T square_norm() const noexcept {
        return dot(*this);
    }

    constexpr T norm() const noexcept {
        return std::sqrt(square_norm());
    }

    constexpr T dot(Vec other) const noexcept {
        T res{};
        for (size_t i = 0; i < Dim; i++) {
            res += coord[i] * other[i];
        }
        return res;
    }

    // [WARN]: could overflow for big value
    constexpr Vec normalized() const {
        return *this / this->norm();
    }

    // [WARN]: could overflow for big value
    constexpr void normalize() {
        *this /= this->norm();
    }

    constexpr Vec cross(Vec other) const noexcept
        requires(Dim == 3)
    {
        return Vec{
            coord[1] * other[2] - coord[2] * other[1],
            coord[2] * other[0] - coord[0] * other[2],
            coord[0] * other[1] - coord[1] * other[0]};
    }

private:
    std::array<T, Dim> coord;
};

using Vec3d = Vec<>;
using Vec3f = Vec<3, float>;
using Vec2d = Vec<2>;
using Vec2f = Vec<2, float>;

template <size_t Dim, class T>
    requires(Dim == 2 || Dim == 3)
struct std::formatter<Vec<Dim, T>> {
    enum class BracketType { None,
                             Parenthesis,
                             Square,
                             Curly };
    enum class SepType { Space,
                         Comma };

    BracketType bracket = BracketType::Parenthesis;
    SepType sep = SepType::Space;
    int precision = 8;

    constexpr auto parse(std::format_parse_context &ctx) {
        auto pos = ctx.begin();
        while (pos != ctx.end() && *pos != '}') {
            switch (*pos) {
            case 'p':
                bracket = BracketType::Parenthesis;
                break;
            case 's':
                bracket = BracketType::Square;
                break;
            case 'c':
                bracket = BracketType::Curly;
                break;
            case 'n':
                bracket = BracketType::None;
                break;
            case ',':
                sep = SepType::Comma;
                break;
            case 'g': {
                ++pos;
                if (pos != ctx.end() && std::isdigit(*pos)) {
                    int p = *pos - '0';
                    if (p > 0)
                        precision = p;
                }
                break;
            }
            }
            ++pos;
        }
        return pos;
    }

    auto format(const Vec<Dim, T> &v, std::format_context &ctx) const {
        std::format_context::iterator out = ctx.out();

        // opening bracket
        switch (bracket) {
        case BracketType::Parenthesis:
            out = std::format_to(out, "(");
            break;
        case BracketType::Square:
            out = std::format_to(out, "[");
            break;
        case BracketType::Curly:
            out = std::format_to(out, "{{");
            break;
        case BracketType::None:
            break;
        }

        // format each component using std::to_string with precision emulation
        // We output a fixed number of decimal places based on precision setting
        auto write_val = [&](T val, bool is_last) {
            // Format with precision by using a format string based on precision value
            switch (precision) {
            case 1: out = std::format_to(out, "{:.1g}", val); break;
            case 2: out = std::format_to(out, "{:.2g}", val); break;
            case 3: out = std::format_to(out, "{:.3g}", val); break;
            case 4: out = std::format_to(out, "{:.4g}", val); break;
            case 5: out = std::format_to(out, "{:.5g}", val); break;
            case 6: out = std::format_to(out, "{:.6g}", val); break;
            case 7: out = std::format_to(out, "{:.7g}", val); break;
            case 8: out = std::format_to(out, "{:.8g}", val); break;
            case 9: out = std::format_to(out, "{:.9g}", val); break;
            default: out = std::format_to(out, "{:.8g}", val); break;
            }
            if (!is_last) {
                out = std::format_to(out, "{} ", sep == SepType::Comma ? ", " : " ");
            }
        };

        if constexpr (Dim == 3) {
            write_val(v.x(), false);
            write_val(v.y(), false);
            write_val(v.z(), true);
        } else {
            write_val(v.x(), false);
            write_val(v.y(), true);
        }

        // closing bracket
        switch (bracket) {
        case BracketType::Parenthesis:
            out = std::format_to(out, ")");
            break;
        case BracketType::Square:
            out = std::format_to(out, "]");
            break;
        case BracketType::Curly:
            out = std::format_to(out, "}}");
            break;
        case BracketType::None:
            break;
        }

        return out;
    }
};