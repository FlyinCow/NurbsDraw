#pragma once
#include "Drawable.h"
#include "math/concepts.h"
#include <QCanvasPainterWidget>
#include <QWidget>
#include <qcanvaspainter.h>
#include <qcanvaspainterwidget.h>
#include <qwidget.h>
#include <vector>

class Canvas2 : public QWidget {
    Q_OBJECT
public:
    explicit Canvas2(QWidget *parent = nullptr);

    void add_curve(Curve2Proxy c);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<DrawableProxy> drawables;
};
