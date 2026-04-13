#include "ui/Curve2DWindow.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Curve2Window window;
    window.show();
    return QApplication::exec();
}
