#pragma once

#include "math/Vector.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QWheelEvent>
#include <QWidget>
#include <functional>
#include <vector>

class QtCanvas : public QWidget {
    Q_OBJECT

public:
    using CurveEvaluator = std::function<Vec3d(double)>;

    explicit QtCanvas(QWidget *parent = nullptr);
    ~QtCanvas() override = default;

    // Control points
    std::vector<Vec3d> &controlPoints() {
        return ctrl_points_;
    }
    const std::vector<Vec3d> &controlPoints() const {
        return ctrl_points_;
    }
    void setControlPoints(const std::vector<Vec3d> &pts);

    // Drawing mode
    bool isDrawing() const {
        return drawing_;
    }
    void setDrawing(bool d);

    // View transform
    QPointF viewOffset() const {
        return view_offset_;
    }
    void setViewOffset(const QPointF &offset);
    float viewScale() const {
        return view_scale_;
    }
    void setViewScale(float scale);

    // Curve evaluators
    void setEvaluator(CurveEvaluator eval);
    void setPreviewEvaluator(CurveEvaluator eval);
    void setInterpolateDegree(int d);

    // Coordinate transforms
    QPointF worldToScreen(const QPointF &w) const;
    QPointF screenToWorld(const QPointF &s) const;

signals:
    void controlPointsChanged();
    void drawingChanged(bool drawing);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    // State
    bool drawing_ = false;
    int dragging_idx_ = -1;
    QPointF view_offset_{0, 0};
    float view_scale_ = 10.0f;
    QPointF mouse_pos_;
    bool hovered_ = false;

    // Data
    std::vector<Vec3d> ctrl_points_;

    // Evaluators
    CurveEvaluator evaluator_;
    CurveEvaluator preview_evaluator_;
    int interpolate_degree_ = 0;

    // Constants
    static constexpr float MIN_SCALE = 0.1f;
    static constexpr float MAX_SCALE = 100.0f;
    static constexpr float ZOOM_FACTOR = 1.1f;
    static constexpr int CURVE_SEGMENTS = 100;
};