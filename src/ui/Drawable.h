#pragma once
#include "math/Sampler.h"
#include "math/concepts.h"
#include "proxy/v4/proxy_macros.h"
#include <print>
#include <proxy/proxy.h>
#include <qpainter.h>

PRO_DEF_MEM_DISPATCH(MemDrawOn, draw_on);
PRO_DEF_MEM_DISPATCH(MemSetColor, set_color);

struct Drawable : pro::facade_builder                           //
                  ::add_convention<MemDrawOn, void(QPainter &)> //
                  ::build {};

using DrawableProxy = pro::proxy<Drawable>;

class CommonCurve2Drawer {

public:
    CommonCurve2Drawer() = delete;
    CommonCurve2Drawer(Curve2Proxy c) noexcept;
    void draw_on(QPainter &painter);

private:
    Curve2Proxy curve;
    SamplerProxy sampler;
};

class ControlPointDrawer {

public:
    ControlPointDrawer() = delete;
    ControlPointDrawer(ControlCurve2Proxy c) noexcept;
    void draw_on(QPainter &painter);

private:
    ControlCurve2Proxy curve;
};