#include "QtCanvas.h"
#include "math/BCurve.h"
#include <QApplication>
#include <QtMath>
#include <cmath>

QtCanvas::QtCanvas(QWidget *parent)
    : QWidget(parent) {
    setMinimumSize(400, 400);
    setFocusPolicy(Qt::StrongFocus);
    view_offset_ = QPointF(-100, -100);
}

void QtCanvas::setControlPoints(const std::vector<Vec3d> &pts) {
    ctrl_points_ = pts;
    update();
}

void QtCanvas::setDrawing(bool d) {
    if (drawing_ != d) {
        drawing_ = d;
        if (drawing_) {
            ctrl_points_.clear();
            dragging_idx_ = -1;
        }
        emit drawingChanged(drawing_);
        update();
    }
}

void QtCanvas::setViewOffset(const QPointF &offset) {
    view_offset_ = offset;
    update();
}

void QtCanvas::setViewScale(float scale) {
    view_scale_ = std::clamp(scale, MIN_SCALE, MAX_SCALE);
    update();
}

void QtCanvas::setEvaluator(CurveEvaluator eval) {
    evaluator_ = std::move(eval);
    update();
}

void QtCanvas::setPreviewEvaluator(CurveEvaluator eval) {
    preview_evaluator_ = std::move(eval);
    update();
}

void QtCanvas::setInterpolateDegree(int d) {
    interpolate_degree_ = d;
    update();
}

QPointF QtCanvas::worldToScreen(const QPointF &w) const {
    return QPointF(
        (w.x() - view_offset_.x()) * view_scale_,
        (w.y() - view_offset_.y()) * view_scale_);
}

QPointF QtCanvas::screenToWorld(const QPointF &s) const {
    return QPointF(
        s.x() / view_scale_ + view_offset_.x(),
        s.y() / view_scale_ + view_offset_.y());
}

void QtCanvas::mousePressEvent(QMouseEvent *event) {
    QPointF w = screenToWorld(event->position());

    if (event->button() == Qt::LeftButton) {
        if (drawing_) {
            ctrl_points_.emplace_back(w.x(), w.y(), 0.0);
            emit controlPointsChanged();
            update();
        } else {
            // Try to grab a control point
            const float radius = 6.0f;
            const float radius2 = radius * radius / (view_scale_ * view_scale_);
            for (int i = 0; i < (int)ctrl_points_.size(); ++i) {
                float dx = w.x() - ctrl_points_[i].x();
                float dy = w.y() - ctrl_points_[i].y();
                if (dx * dx + dy * dy < radius2) {
                    dragging_idx_ = i;
                    break;
                }
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (drawing_) {
            setDrawing(false);
        }
    }
}

void QtCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging_idx_ = -1;
    }
}

void QtCanvas::mouseMoveEvent(QMouseEvent *event) {
    mouse_pos_ = event->position();
    hovered_ = true;

    if (dragging_idx_ != -1) {
        QPointF w = screenToWorld(mouse_pos_);
        ctrl_points_[dragging_idx_] = Vec3d(w.x(), w.y(), 0.0);
        emit controlPointsChanged();
        update();
    } else {
        update();
    }
}

void QtCanvas::wheelEvent(QWheelEvent *event) {
    QPointF before = screenToWorld(event->position());
    float factor = std::pow(ZOOM_FACTOR, event->angleDelta().y() / 120.0f);
    view_scale_ = std::clamp(view_scale_ * factor, MIN_SCALE, MAX_SCALE);
    QPointF after = screenToWorld(event->position());
    view_offset_ += before - after;
    update();
}

void QtCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF canvas_rect(0, 0, width(), height());
    painter.fillRect(canvas_rect, QBrush(QColor(30, 30, 30)));
    painter.setPen(QPen(QColor(255, 255, 255), 1.0));
    painter.drawRect(canvas_rect.adjusted(0.5, 0.5, -0.5, -0.5));

    bool use_preview = drawing_ && (preview_evaluator_ || interpolate_degree_ > 0);

    // Draw control points and polygon
    for (int i = 0; i < (int)ctrl_points_.size(); ++i) {
        QPointF screen = worldToScreen(QPointF(ctrl_points_[i].x(), ctrl_points_[i].y()));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 0, 0)));
        painter.drawEllipse(screen, 4.0, 4.0);

        if (i > 0) {
            QPointF prev = worldToScreen(QPointF(ctrl_points_[i - 1].x(), ctrl_points_[i - 1].y()));
            painter.setPen(QPen(QColor(0, 255, 0), 1.0));
            painter.drawLine(prev, screen);
        }
    }

    // Preview point (mouse position) when drawing
    if (drawing_ && hovered_) {
        QPointF w = screenToWorld(mouse_pos_);
        QPointF screen_pt = worldToScreen(w);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 0, 200)));
        painter.drawEllipse(screen_pt, 5.0, 5.0);
    }

    // Draw hint message
    if (drawing_ && !ctrl_points_.empty()) {
        int required_pts = interpolate_degree_ > 0 ? (interpolate_degree_ + 1) : 4;
        QString hint;
        if (interpolate_degree_ > 0) {
            if ((int)ctrl_points_.size() < required_pts) {
                hint = QString("Need %1 pts for degree %2 interpolate")
                           .arg(required_pts)
                           .arg(interpolate_degree_);
            }
        } else {
            if ((int)ctrl_points_.size() < 2) {
                hint = "Click to add control points";
            } else if ((int)ctrl_points_.size() < 4) {
                hint = QString("Add more points (need %1 more for degree 3)")
                           .arg(4 - ctrl_points_.size());
            }
        }
        if (!hint.isEmpty()) {
            painter.setPen(QPen(QColor(255, 255, 255)));
            painter.drawText(10, height() - 10, hint);
        }
    }

    // Evaluation points (including mouse preview)
    std::vector<Vec3d> eval_points = ctrl_points_;
    if (drawing_ && !ctrl_points_.empty()) {
        QPointF w = screenToWorld(mouse_pos_);
        eval_points.emplace_back(w.x(), w.y(), 0.0);
    }

    // Draw curve
    CurveEvaluator active_evaluator = evaluator_;
    if (use_preview && preview_evaluator_) {
        active_evaluator = preview_evaluator_;
    }

    if (use_preview && eval_points.size() >= 2) {
        std::shared_ptr<BCurve> preview_curve;
        QPen curve_pen(QColor(255, 255, 0, 200), 2.0);

        if (interpolate_degree_ > 0 && (int)eval_points.size() >= interpolate_degree_ + 1) {
            // Interpolation mode
            int n = static_cast<int>(eval_points.size());
            for (int seg = 0; seg < n - 1; seg++) {
                Vec3d p0, p1, p2, p3;
                if (seg == 0) {
                    p0 = eval_points[0];
                    p1 = eval_points[0];
                    p2 = eval_points[1];
                    p3 = eval_points[2 % n];
                } else if (seg == n - 2) {
                    p0 = eval_points[seg - 1];
                    p1 = eval_points[seg];
                    p2 = eval_points[seg + 1];
                    p3 = eval_points[seg + 1];
                } else {
                    p0 = eval_points[seg - 1];
                    p1 = eval_points[seg];
                    p2 = eval_points[seg + 1];
                    p3 = eval_points[seg + 2];
                }

                int segs_per_span = CURVE_SEGMENTS / (n - 1);
                QPointF prev_pt;
                for (int i = 1; i <= segs_per_span; i++) {
                    double t = static_cast<double>(i) / segs_per_span;
                    double t2 = t * t, t3 = t2 * t;
                    double b0 = -0.5 * t3 + t2 - 0.5 * t;
                    double b1 = 1.5 * t3 - 2.5 * t2 + 1.0;
                    double b2 = -1.5 * t3 + 2.0 * t2 + 0.5 * t;
                    double b3 = 0.5 * t3 - 0.5 * t2;
                    Vec3d p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
                    QPointF screen = worldToScreen(QPointF(p.x(), p.y()));
                    if (i > 1) {
                        painter.setPen(curve_pen);
                        painter.drawLine(prev_pt, screen);
                    }
                    prev_pt = screen;
                }
            }
        } else {
            // Control point mode
            int preview_degree = std::min(3, static_cast<int>(eval_points.size()) - 1);
            if (preview_degree < 1)
                preview_degree = 1;

            int n = static_cast<int>(eval_points.size()) - 1;
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

            preview_curve = std::make_shared<BCurve>(preview_degree, eval_points, unique_knots, mults);
        }

        if (preview_curve) {
            QPointF prev_pt;
            for (int i = 1; i <= CURVE_SEGMENTS; i++) {
                double t = static_cast<double>(i) / CURVE_SEGMENTS;
                Vec3d p = preview_curve->eval(t);
                QPointF screen = worldToScreen(QPointF(p.x(), p.y()));
                if (i > 1) {
                    painter.setPen(curve_pen);
                    painter.drawLine(prev_pt, screen);
                }
                prev_pt = screen;
            }
        }
    } else if (eval_points.size() >= 2 && evaluator_) {
        QPen curve_pen(QColor(255, 255, 255), 2.0);
        QPointF prev_pt;
        for (int i = 1; i <= CURVE_SEGMENTS; i++) {
            double t = static_cast<double>(i) / CURVE_SEGMENTS;
            Vec3d p = evaluator_(t);
            QPointF screen = worldToScreen(QPointF(p.x(), p.y()));
            if (i > 1) {
                painter.setPen(curve_pen);
                painter.drawLine(prev_pt, screen);
            }
            prev_pt = screen;
        }
    }

    // Draw axes
    QPointF origin_screen = worldToScreen(QPointF(0, 0));
    painter.setPen(QPen(QColor(200, 200, 200), 1.0));
    if (origin_screen.y() >= 0 && origin_screen.y() <= width()) {
        painter.drawLine(0, origin_screen.y(), width(), origin_screen.y());
    }
    if (origin_screen.x() >= 0 && origin_screen.x() <= width()) {
        painter.drawLine(origin_screen.x(), 0, origin_screen.x(), height());
    }
    if (origin_screen.x() >= 0 && origin_screen.x() <= width() &&
        origin_screen.y() >= 0 && origin_screen.y() <= height()) {
        painter.setBrush(QBrush(QColor(255, 255, 0)));
        painter.drawEllipse(origin_screen, 5.0, 5.0);
    }

    // Draw tick marks and labels
    float min_x = view_offset_.x();
    float max_x = view_offset_.x() + width() / view_scale_;
    float min_y = view_offset_.y();
    float max_y = view_offset_.y() + height() / view_scale_;

    float target_pixel_spacing = 80.0f;
    float tick_interval = std::pow(10.0f, std::floor(std::log10(target_pixel_spacing / view_scale_) + 0.5f));
    if (tick_interval < 1.0f)
        tick_interval = 1.0f;

    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    int start_x = static_cast<int>(std::floor(min_x / tick_interval));
    int end_x = static_cast<int>(std::ceil(max_x / tick_interval));
    for (int xi = start_x; xi <= end_x; ++xi) {
        float world_x = xi * tick_interval;
        QPointF pt = worldToScreen(QPointF(world_x, 0.0f));
        if (pt.x() >= 0 && pt.x() <= width()) {
            painter.setPen(QPen(QColor(180, 180, 180), 1.0));
            painter.drawLine(QPointF(pt.x(), origin_screen.y() - 5), QPointF(pt.x(), origin_screen.y() + 5));
            painter.drawText(QPointF(pt.x() + 2, origin_screen.y() + 15), QString::number(static_cast<int>(world_x)));
        }
    }

    int start_y = static_cast<int>(std::floor(min_y / tick_interval));
    int end_y = static_cast<int>(std::ceil(max_y / tick_interval));
    for (int yi = start_y; yi <= end_y; ++yi) {
        float world_y = yi * tick_interval;
        QPointF pt = worldToScreen(QPointF(0.0f, world_y));
        if (pt.y() >= 0 && pt.y() <= height()) {
            painter.setPen(QPen(QColor(180, 180, 180), 1.0));
            painter.drawLine(QPointF(origin_screen.x() - 5, pt.y()), QPointF(origin_screen.x() + 5, pt.y()));
            painter.drawText(QPointF(origin_screen.x() + 8, pt.y() + 4), QString::number(static_cast<int>(world_y)));
        }
    }
}