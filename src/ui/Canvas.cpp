#include "Canvas.h"
#include "Drawable.h"
#include "math/concepts.h"
#include <QPainter>

Canvas2::Canvas2(QWidget *parent) : QWidget(parent) {
}

void Canvas2::add_curve(Curve2Proxy c) {
    drawables.push_back(new CommonCurve2Drawer(c));
}

void Canvas2::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    painter.save();
    painter.translate(50, height() - 50);
    double scaleX = width() / 8.0;
    double scaleY = height() / 8.0;
    painter.scale(scaleX, -scaleY);

    // Pen width in device pixels (constant regardless of zoom)
    double penWidth = 1.0 / scaleX;
    painter.setPen(QPen(Qt::blue, penWidth));

    for (auto s : drawables) {
        s->draw_on(painter);
    }
    painter.restore();
}
