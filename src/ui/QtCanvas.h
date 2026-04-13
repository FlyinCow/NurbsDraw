#pragma once
#include "math/Vector.h"
#include "math/BCurve.h"
#include <QWidget>
#include <memory>

class Canvas2 : public QWidget {
    Q_OBJECT
public:
    explicit Canvas2(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::shared_ptr<BCurve> curve_;
    std::vector<Vec3d> ctrl_points_;
};
