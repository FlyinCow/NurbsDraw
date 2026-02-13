#include "core/Vector.h"
#include <cmath>
#include <cstring>

Vector::Vector(const Vector &other) noexcept {
    std::memcpy(coord, other.coord, sizeof(double) * 3);
}

Vector &Vector::operator=(const Vector &other) {
    if (this == &other) {
        return *this;
    }
    std::memcpy(coord, other.coord, sizeof(double) * 3);
    return *this;
}

Vector &Vector::operator+=(const Vector &other) {
    for (int i = 0; i < 3; i++) {
        coord[i] += other.coord[i];
    }
    return *this;
}

Vector Vector::operator-() const {
    return Vector(-x(), -y(), -z());
}

Vector &Vector::operator*=(double k) {
    for (int i = 0; i < 3; i++) {
        coord[i] *= k;
    }
    return *this;
}

double Vector::dot(Vector other) const noexcept {
    return x() * other.x() + y() * other.y() + z() * other.z();
}

double Vector::squareNorm() const noexcept {
    return dot(*this);
}

double Vector::Norm() const noexcept {
    return std::sqrt(squareNorm());
}