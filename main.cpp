// NurbsDraw - Qt6 Window

#include <QApplication>
#include <QMainWindow>
#include "ui/Curve2DWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("NurbsDraw");
    window.resize(1280, 800);
    window.setCentralWidget(new Curve2DWindow(&window));

    window.show();
    return app.exec();
}
