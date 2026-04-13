#include "QtCanvas.h"
#include <QPainter>

Canvas2::Canvas2(QWidget* parent) : QWidget(parent) {
    // 固定初始 B-spline
    ctrl_points_ = {{0, 0, 0}, {1, 2, 0}, {2, 1, 0}, {3, 3, 0}};
    int degree = 3;
    // Clamped cubic B-spline: flat knots = [0,0,0,0,1,1,1,1]
    std::vector<double> knots = {0.0, 1.0};
    std::vector<int> mults = {degree + 1, degree + 1};
    curve_ = std::make_shared<BCurve>(degree, ctrl_points_, knots, mults);
}

void Canvas2::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    // 绘制控制点（红色）
    painter.setPen(Qt::red);
    for (const auto& p : ctrl_points_) {
        painter.drawEllipse(QPointF(p.x() * 50 + 100, height() - (p.y() * 50 + 100)), 5, 5);
    }

    // 绘制曲线（蓝色）
    painter.setPen(Qt::blue);
    const int segments = 100;
    QPointF prev;
    for (int i = 0; i <= segments; i++) {
        double t = static_cast<double>(i) / segments;
        Vec3d p = curve_->eval(t);
        QPointF pt(p.x() * 50 + 100, height() - (p.y() * 50 + 100));
        if (i > 0) painter.drawLine(prev, pt);
        prev = pt;
    }
}
