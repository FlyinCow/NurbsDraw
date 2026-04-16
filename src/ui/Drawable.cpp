#include "Drawable.h"
#include "math/Sampler.h"
#include "math/concepts.h"
#include <print>
#include <qpainter.h>
#include <qpoint.h>
#include <vector>

// TODO: fix mem leak of sampler
CommonCurve2Drawer::CommonCurve2Drawer(Curve2Proxy c) noexcept : curve(c), sampler(new Curve2dSampler()) {
}

void CommonCurve2Drawer::draw_on(QPainter &painter) {
    if (!curve || !sampler)
        return;

    std::vector<Vec2d> points;
    sampler->sample_to(curve, points);
    if (points.size() < 2)
        return;

    for (size_t i = 1; i < points.size(); ++i) {
        painter.drawLine(
            QPointF(points[i - 1].x(), points[i - 1].y()),
            QPointF(points[i].x(), points[i].y()));
    }
}

ControlPointDrawer::ControlPointDrawer(ControlCurve2Proxy c) noexcept : curve(c) {
}

void ControlPointDrawer::draw_on(QPainter &painter) {
    if (!curve)
        return;
    auto ctrl_pts = curve->get_control_points();
    for (size_t i = 1; i < ctrl_pts.size(); ++i) {
        painter.drawLine(
            QPointF(ctrl_pts[i - 1].x(), ctrl_pts[i - 1].y()),
            QPointF(ctrl_pts[i].x(), ctrl_pts[i].y()));
    }
    for (auto &p : ctrl_pts) {
        painter.drawEllipse(QPointF(p.x(), p.y()), 2, 2);
        // painter.drawPoint(QPointF(p.x(), p.y()));
    }
}