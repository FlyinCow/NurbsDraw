#include "ui/Curve2DWindow.h"
#include <QApplication>

// #include <print>
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    Curve2Window window;
    window.show();
    return QGuiApplication::exec();
}
