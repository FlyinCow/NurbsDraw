#include "Curve2DWindow.h"
#include "Canvas.h"
#include "Drawable.h"
#include "math/Vec.h"
#include "proxy/v4/proxy.h"
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
    std::vector<double> knots = {0.0, 1.0};
    std::vector<int> mults = {degree + 1, degree + 1};
    curves.emplace_back(degree, ctrl_points_, knots, mults);
    // canvas_->add_drawable(new CommonCurve2Drawer(&curves[0]));
    canvas_->add_drawable(pro::make_proxy<Drawable, CommonCurve2Drawer>(&curves[0]));
    canvas_->add_drawable(pro::make_proxy<Drawable, ControlPointDrawer>(&curves[0]));
}
