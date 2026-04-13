#include "Curve2DWindow.h"
#include "QtCanvas.h"
#include <QHBoxLayout>

Curve2Window::Curve2Window(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("NurbsDraw");
    canvas_ = new Canvas(this);
    auto* layout = new QHBoxLayout(this);
    layout->addWidget(canvas_);
    setLayout(layout);
    resize(800, 600);
}

Curve2Window::~Curve2Window() = default;
