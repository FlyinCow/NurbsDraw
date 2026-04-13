#pragma once
#include <QWidget>

class Canvas2;

class Curve2Window : public QWidget {
    Q_OBJECT
public:
    explicit Curve2Window(QWidget* parent = nullptr);
    ~Curve2Window() override;

private:
    Canvas2* canvas_;
};
