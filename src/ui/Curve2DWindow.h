#pragma once

#include "QtCanvas.h"
#include "math/BCurve.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <vector>

class Curve2DWindow : public QWidget {
    Q_OBJECT

public:
    explicit Curve2DWindow(QWidget *parent = nullptr);
    ~Curve2DWindow() override = default;

private slots:
    void onDrawToggled(bool checked);
    void onCurveTypeChanged();
    void onDegreeChanged(int value);
    void onNurbsToggled(bool checked);
    void onDrawModeChanged();
    void onControlPointsChanged();

private:
    void rebuildCurve();
    void updateCoordsList();

    // UI elements
    QPushButton *draw_button_;
    QRadioButton *bezier_radio_;
    QRadioButton *bspline_radio_;
    QSpinBox *degree_spin_;
    QCheckBox *nurbs_check_;
    QComboBox *draw_mode_combo_;
    QtCanvas *canvas_;
    QListWidget *coords_list_;

    // State
    int curve_type_ = 1; // 0 = Bezier, 1 = B-spline
    int degree_ = 3;
    bool is_nurbs_ = false;
    int draw_mode_ = 0; // 0 = by ctrl pt, 1 = by interpolate
    std::vector<double> weights_;
    std::shared_ptr<BCurve> cached_curve_;
    std::vector<Vec3d> cached_ctrl_points_;
    int cached_degree_ = -1;
};