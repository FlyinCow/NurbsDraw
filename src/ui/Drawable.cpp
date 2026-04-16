#include "Drawable.h"
#include "math/Sampler.h"
#include "math/concepts.h"
#include <qpainter.h>
#include <qpoint.h>
#include <vector>

CommonCurve2Drawer::CommonCurve2Drawer(Curve2Proxy c) : curve(c), sampler(new Curve2dSampler()) {
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