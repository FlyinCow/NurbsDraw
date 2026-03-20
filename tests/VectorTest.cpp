#include "math/Vector.h"
#include <gtest/gtest.h>
// ===== Construction Tests =====

class VectorContruction : public testing::Test {
};

TEST(VectorConstruction, DefaultConstruction) {
    Vec3d v;
    EXPECT_DOUBLE_EQ(v.x(), 0.0);
    EXPECT_DOUBLE_EQ(v.y(), 0.0);
    EXPECT_DOUBLE_EQ(v.z(), 0.0);
}

TEST(VectorConstruction, ComponentConstruction3D) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(VectorConstruction, ComponentConstruction2D) {
    Vec2d v(1.0, 2.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
}

TEST(VectorConstruction, FloatConstruction) {
    Vec3f v(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v.x(), 1.0f);
    EXPECT_FLOAT_EQ(v.y(), 2.0f);
    EXPECT_FLOAT_EQ(v.z(), 3.0f);
}

TEST(VectorConstruction, ArrayConstruction) {
    std::array<double, 3> arr = {1.0, 2.0, 3.0};
    Vec3d v(arr);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

// ===== Accessor Tests =====

TEST(VectorAccessor, ConstAccessor) {
    const Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(VectorAccessor, IndexAccess) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

TEST(VectorAccessor, ConstIndexAccess) {
    const Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

// ===== Arithmetic Tests =====

TEST(VectorArithmetic, Addition) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b(4.0, 5.0, 6.0);
    Vec3d c = a + b;
    EXPECT_DOUBLE_EQ(c.x(), 5.0);
    EXPECT_DOUBLE_EQ(c.y(), 7.0);
    EXPECT_DOUBLE_EQ(c.z(), 9.0);
}

TEST(VectorArithmetic, Subtraction) {
    Vec3d a(4.0, 5.0, 6.0);
    Vec3d b(1.0, 2.0, 3.0);
    Vec3d c = a - b;
    EXPECT_DOUBLE_EQ(c.x(), 3.0);
    EXPECT_DOUBLE_EQ(c.y(), 3.0);
    EXPECT_DOUBLE_EQ(c.z(), 3.0);
}

TEST(VectorArithmetic, UnaryMinus) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b = -a;
    EXPECT_DOUBLE_EQ(b.x(), -1.0);
    EXPECT_DOUBLE_EQ(b.y(), -2.0);
    EXPECT_DOUBLE_EQ(b.z(), -3.0);
}

TEST(VectorArithmetic, ScalarMultiplication) {
    Vec3d v(1.0, 2.0, 3.0);
    Vec3d result = v * 2.0;
    EXPECT_DOUBLE_EQ(result.x(), 2.0);
    EXPECT_DOUBLE_EQ(result.y(), 4.0);
    EXPECT_DOUBLE_EQ(result.z(), 6.0);
}

TEST(VectorArithmetic, ScalarMultiplicationReverse) {
    Vec3d v(1.0, 2.0, 3.0);
    Vec3d result = 2.0 * v;
    EXPECT_DOUBLE_EQ(result.x(), 2.0);
    EXPECT_DOUBLE_EQ(result.y(), 4.0);
    EXPECT_DOUBLE_EQ(result.z(), 6.0);
}

TEST(VectorArithmetic, CompoundAddition) {
    Vec3d v(1.0, 2.0, 3.0);
    v += Vec3d(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(v.x(), 5.0);
    EXPECT_DOUBLE_EQ(v.y(), 7.0);
    EXPECT_DOUBLE_EQ(v.z(), 9.0);
}

TEST(VectorArithmetic, CompoundScalarMultiplication) {
    Vec3d v(1.0, 2.0, 3.0);
    v *= 2.0;
    EXPECT_DOUBLE_EQ(v.x(), 2.0);
    EXPECT_DOUBLE_EQ(v.y(), 4.0);
    EXPECT_DOUBLE_EQ(v.z(), 6.0);
}

// ===== Norm Tests =====

TEST(VectorNorm, SquareNorm) {
    Vec3d v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.square_norm(), 25.0);
}

TEST(VectorNorm, Norm) {
    Vec3d v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.norm(), 5.0);
}

TEST(VectorNorm, Norm3D) {
    Vec3d v(1.0, 2.0, 2.0);
    EXPECT_DOUBLE_EQ(v.norm(), 3.0);
}

TEST(VectorNorm, ZeroNorm) {
    Vec3d v;
    EXPECT_DOUBLE_EQ(v.norm(), 0.0);
}

// ===== Dot Product Tests =====

TEST(VectorDot, Basic) {
    Vec3d a(1.0, 0.0, 0.0);
    Vec3d b(0.0, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(a.dot(b), 0.0);
}

TEST(VectorDot, Orthogonal) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b(-2.0, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(a.dot(b), 0.0);
}

TEST(VectorDot, SelfDot) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.dot(v), 14.0);
}

// ===== Cross Product Tests (3D only) =====

TEST(VectorCross, Basic) {
    Vec3d a(1.0, 0.0, 0.0);
    Vec3d b(0.0, 1.0, 0.0);
    Vec3d c = a.cross(b);
    EXPECT_DOUBLE_EQ(c.x(), 0.0);
    EXPECT_DOUBLE_EQ(c.y(), 0.0);
    EXPECT_DOUBLE_EQ(c.z(), 1.0);
}

TEST(VectorCross, OrthogonalUnitVectors) {
    Vec3d x(1.0, 0.0, 0.0);
    Vec3d y(0.0, 1.0, 0.0);
    Vec3d z(0.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(x.cross(y).dot(z), 1.0);
}

TEST(VectorCross, SelfCrossIsZero) {
    Vec3d v(1.0, 2.0, 3.0);
    Vec3d result = v.cross(v);
    EXPECT_DOUBLE_EQ(result.x(), 0.0);
    EXPECT_DOUBLE_EQ(result.y(), 0.0);
    EXPECT_DOUBLE_EQ(result.z(), 0.0);
}

TEST(VectorCross, Antisymmetric) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b(4.0, 5.0, 6.0);
    Vec3d c1 = a.cross(b);
    Vec3d c2 = b.cross(a);
    EXPECT_DOUBLE_EQ(c1.x(), -c2.x());
    EXPECT_DOUBLE_EQ(c1.y(), -c2.y());
    EXPECT_DOUBLE_EQ(c1.z(), -c2.z());
}

// ===== Normalization Tests =====

TEST(VectorNormalization, Normalized) {
    Vec3d v(3.0, 4.0, 0.0);
    Vec3d n = v.normalized();
    EXPECT_DOUBLE_EQ(n.norm(), 1.0);
    EXPECT_DOUBLE_EQ(n.x(), 0.6);
    EXPECT_DOUBLE_EQ(n.y(), 0.8);
}

TEST(VectorNormalization, ZeroVectorNormalized) {
    Vec3d v;
    Vec3d n = v.normalized();
    EXPECT_TRUE(std::isnan(n.x()));
    EXPECT_TRUE(std::isnan(n.y()));
    EXPECT_TRUE(std::isnan(n.z()));
}

// ===== 2D Specific Tests =====

TEST(Vector2D, NoZAccessor) {
    Vec2d v(1.0, 2.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    // Vec2d has no z()
}

TEST(Vector2D, Norm) {
    Vec2d v(3.0, 4.0);
    EXPECT_DOUBLE_EQ(v.norm(), 5.0);
}

TEST(Vector2D, Dot) {
    Vec2d a(1.0, 2.0);
    Vec2d b(3.0, 4.0);
    EXPECT_DOUBLE_EQ(a.dot(b), 11.0);
}

// ===== Format Tests =====

TEST(VectorFormat, DefaultFormat3D) {
    Vec3d v(1.5, 2.5, 3.5);
    std::string s = std::format("{}", v);
    EXPECT_TRUE(s.find('(') != std::string::npos || s.find('[') != std::string::npos);
}

TEST(VectorFormat, DefaultFormat2D) {
    Vec2d v(1.5, 2.5);
    std::string s = std::format("{}", v);
    EXPECT_TRUE(s.find('(') != std::string::npos || s.find('[') != std::string::npos);
}

// ===== Constexpr Tests =====

TEST(VectorConstexpr, ConstexprConstruction) {
    constexpr Vec3d v(1.0, 2.0, 3.0);
    static_assert(v.x() == 1.0);
    static_assert(v.y() == 2.0);
    static_assert(v.z() == 3.0);
}

TEST(VectorConstexpr, ConstexprDefaultConstruction) {
    constexpr Vec3d v;
    static_assert(v.x() == 0.0);
    static_assert(v.y() == 0.0);
    static_assert(v.z() == 0.0);
}

TEST(VectorConstexpr, ConstexprArithmetic) {
    constexpr Vec3d a(1.0, 2.0, 3.0);
    constexpr Vec3d b(4.0, 5.0, 6.0);
    constexpr Vec3d c = a + b;
    static_assert(c.x() == 5.0);
    static_assert(c.y() == 7.0);
    static_assert(c.z() == 9.0);
}

TEST(VectorConstexpr, ConstexprNorm) {
    constexpr Vec3d v(3.0, 4.0, 0.0);
    // norm() uses std::sqrt which is not constexpr until C++26,
    // so we test square_norm() instead for constexpr context
    constexpr double sn = v.square_norm();
    static_assert(sn == 25.0);
}