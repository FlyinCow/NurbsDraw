#pragma once
#include "math/BCurve.h"
#include <QWidget>
#include <vector>

class Canvas2;

static std::vector<BCurve2> curves;

class Curve2Window : public QWidget {
    Q_OBJECT
public:
    explicit Curve2Window(QWidget *parent = nullptr);
    ~Curve2Window() override;

private:
    Canvas2 *canvas_;
};
