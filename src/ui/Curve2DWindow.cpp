#include "Curve2DWindow.h"
#include "Canvas.h"
#include "math/Vector.h"
#include <QHBoxLayout>

Curve2Window::Curve2Window(QWidget *parent)
    : QWidget(parent) {
    setWindowTitle("NurbsDraw");
    canvas_ = new Canvas2(this);
    auto *layout = new QHBoxLayout(this);
    layout->addWidget(canvas_);
    setLayout(layout);
    resize(800, 600);
    std::vector<Vec2d> ctrl_points_ = {{0, 0}, {1, 2}, {2, 1}, {3, 3}};
    int degree = 3;
    // Clamped cubic B-spline: flat knots = [0,0,0,0,1,1,1,1]
    std::vector<double> knots = {0.0, 1.0};
    std::vector<int> mults = {degree + 1, degree + 1};
    curves.emplace_back(degree, ctrl_points_, knots, mults);
    canvas_->add_curve(&curves[0]);
}

Curve2Window::~Curve2Window() = default;
