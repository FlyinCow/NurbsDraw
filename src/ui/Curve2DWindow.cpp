#include "Curve2DWindow.h"
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>

Curve2DWindow::Curve2DWindow(QWidget *parent)
    : QWidget(parent) {
    setWindowTitle("2D Curves");
    resize(1024, 768);

    // Create widgets
    draw_button_ = new QPushButton("Draw", this);
    draw_button_->setCheckable(true);

    bezier_radio_ = new QRadioButton("Bezier", this);
    bspline_radio_ = new QRadioButton("B-spline", this);
    bspline_radio_->setChecked(true);

    degree_spin_ = new QSpinBox(this);
    degree_spin_->setRange(1, 10);
    degree_spin_->setValue(3);

    nurbs_check_ = new QCheckBox("NURBS", this);
    nurbs_check_->setEnabled(false);

    draw_mode_combo_ = new QComboBox(this);
    draw_mode_combo_->addItem("Draw by Ctrl Pt");
    draw_mode_combo_->addItem("Draw by Interpolate");

    canvas_ = new QtCanvas(this);
    coords_list_ = new QListWidget(this);

    // Layout - left panel (controls), center (canvas), right (coords)
    QHBoxLayout *main_layout = new QHBoxLayout(this);

    // Left control panel
    QGroupBox *control_group = new QGroupBox("Controls", this);
    QVBoxLayout *control_layout = new QVBoxLayout(control_group);

    control_layout->addWidget(draw_button_);
    control_layout->addSpacing(10);
    control_layout->addWidget(new QLabel("Curve Type:"));
    control_layout->addWidget(bezier_radio_);
    control_layout->addWidget(bspline_radio_);
    control_layout->addSpacing(10);
    control_layout->addWidget(new QLabel("Degree:"));
    control_layout->addWidget(degree_spin_);
    control_layout->addSpacing(10);
    control_layout->addWidget(nurbs_check_);
    control_layout->addSpacing(10);
    control_layout->addWidget(new QLabel("Draw Mode:"));
    control_layout->addWidget(draw_mode_combo_);

    control_layout->addStretch();

    // Right panel (coords)
    QGroupBox *coords_group = new QGroupBox("Coordinates", this);
    QVBoxLayout *coords_layout = new QVBoxLayout(coords_group);
    coords_layout->addWidget(coords_list_);

    main_layout->addWidget(control_group, 1);
    main_layout->addWidget(canvas_, 3);
    main_layout->addWidget(coords_group, 1);

    // Connections
    connect(draw_button_, &QPushButton::toggled, this, &Curve2DWindow::onDrawToggled);
    connect(bezier_radio_, &QRadioButton::toggled, this, &Curve2DWindow::onCurveTypeChanged);
    connect(degree_spin_, QOverload<int>::of(&QSpinBox::valueChanged), this, &Curve2DWindow::onDegreeChanged);
    connect(nurbs_check_, &QCheckBox::toggled, this, &Curve2DWindow::onNurbsToggled);
    connect(draw_mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Curve2DWindow::onDrawModeChanged);
    connect(canvas_, &QtCanvas::controlPointsChanged, this, &Curve2DWindow::onControlPointsChanged);

    // Setup evaluators
    auto curve_evaluator = [this](double t) -> Vec3d {
        if (!cached_curve_ || canvas_->controlPoints().size() < static_cast<size_t>(degree_ + 1)) {
            return Vec3d{};
        }
        return cached_curve_->eval(t);
    };

    auto preview_evaluator = [this](double t) -> Vec3d {
        const auto &ctrl_points = canvas_->controlPoints();
        if (ctrl_points.size() < 2)
            return Vec3d{};

        if (draw_mode_ == 0) {
            int preview_degree = std::min(degree_, static_cast<int>(ctrl_points.size()) - 1);
            if (preview_degree < 1)
                preview_degree = 1;

            int n = static_cast<int>(ctrl_points.size()) - 1;
            int num_unique = n - preview_degree + 2;
            if (num_unique < 2)
                num_unique = 2;

            std::vector<double> unique_knots(num_unique);
            std::vector<int> mults(num_unique);
            for (int i = 0; i < num_unique; i++) {
                unique_knots[i] = static_cast<double>(i) / (num_unique - 1);
            }
            mults[0] = preview_degree + 1;
            mults[num_unique - 1] = preview_degree + 1;
            for (int i = 1; i < num_unique - 1; i++)
                mults[i] = 1;

            BCurve preview_curve(preview_degree, ctrl_points, unique_knots, mults);
            return preview_curve.eval(t);
        }
        return Vec3d{};
    };

    canvas_->setPreviewEvaluator(preview_evaluator);
    canvas_->setEvaluator(curve_evaluator);
}

void Curve2DWindow::onDrawToggled(bool checked) {
    canvas_->setDrawing(checked);
    draw_button_->setText(checked ? "Stop" : "Draw");
    if (checked) {
        canvas_->controlPoints().clear();
        cached_curve_.reset();
        cached_ctrl_points_.clear();
        if (is_nurbs_)
            weights_.clear();
    }
    updateCoordsList();
}

void Curve2DWindow::onCurveTypeChanged() {
    if (bezier_radio_->isChecked()) {
        curve_type_ = 0;
        nurbs_check_->setEnabled(false);
        draw_mode_combo_->setEnabled(false);
    } else {
        curve_type_ = 1;
        nurbs_check_->setEnabled(true);
        draw_mode_combo_->setEnabled(true);
    }
    rebuildCurve();
}

void Curve2DWindow::onDegreeChanged(int value) {
    degree_ = value;
    rebuildCurve();
}

void Curve2DWindow::onNurbsToggled(bool checked) {
    is_nurbs_ = checked;
    rebuildCurve();
}

void Curve2DWindow::onDrawModeChanged() {
    draw_mode_ = draw_mode_combo_->currentIndex();
    if (draw_mode_ == 1) {
        canvas_->setInterpolateDegree(degree_);
    } else {
        canvas_->setInterpolateDegree(0);
    }
    rebuildCurve();
}

void Curve2DWindow::onControlPointsChanged() {
    rebuildCurve();
    updateCoordsList();
}

void Curve2DWindow::rebuildCurve() {
    const auto &ctrl_points = canvas_->controlPoints();

    // Update weights
    if (is_nurbs_ && (int)weights_.size() != (int)ctrl_points.size()) {
        weights_.resize(ctrl_points.size(), 1.0);
    }

    // Check if rebuild needed
    bool need_rebuild = cached_degree_ != degree_ ||
                        cached_ctrl_points_.size() != ctrl_points.size();
    if (!need_rebuild && !cached_ctrl_points_.empty()) {
        for (size_t i = 0; i < ctrl_points.size(); i++) {
            if (cached_ctrl_points_[i].x() != ctrl_points[i].x() ||
                cached_ctrl_points_[i].y() != ctrl_points[i].y() ||
                cached_ctrl_points_[i].z() != ctrl_points[i].z()) {
                need_rebuild = true;
                break;
            }
        }
    }

    if (need_rebuild && (int)ctrl_points.size() >= degree_ + 1) {
        int n = static_cast<int>(ctrl_points.size()) - 1;
        int num_unique = n - degree_ + 2;
        if (num_unique < 2)
            num_unique = 2;

        std::vector<double> unique_knots(num_unique);
        std::vector<int> mults(num_unique);
        for (int i = 0; i < num_unique; i++) {
            unique_knots[i] = static_cast<double>(i) / (num_unique - 1);
        }
        mults[0] = degree_ + 1;
        mults[num_unique - 1] = degree_ + 1;
        for (int i = 1; i < num_unique - 1; i++)
            mults[i] = 1;

        if (is_nurbs_ && !weights_.empty()) {
            cached_curve_ = std::make_shared<BCurve>(degree_, ctrl_points, weights_, unique_knots, mults);
        } else {
            cached_curve_ = std::make_shared<BCurve>(degree_, ctrl_points, unique_knots, mults);
        }

        cached_ctrl_points_ = ctrl_points;
        cached_degree_ = degree_;
    }
}

void Curve2DWindow::updateCoordsList() {
    coords_list_->clear();
    const auto &pts = canvas_->controlPoints();
    for (size_t i = 0; i < pts.size(); i++) {
        coords_list_->addItem(QString("%1: %2, %3")
                                  .arg(i)
                                  .arg(pts[i].x(), 0, 'f', 1)
                                  .arg(pts[i].y(), 0, 'f', 1));
    }
}