#pragma once
#include "math/Sampler.h"
#include "math/concepts.h"
#include <proxy/proxy.h>
#include <qpainter.h>

namespace __detail {
PRO_DEF_MEM_DISPATCH(MemDrawOn, draw_on);
struct Drawable : pro::facade_builder                            //
                  ::add_convention<MemDrawOn, void(QPainter &)>  //
                  ::support_copy<pro::constraint_level::trivial> //
                  ::build {};
} // namespace __detail

using DrawableProxy = pro::proxy<__detail::Drawable>;

class CommonCurve2Drawer {

public:
    CommonCurve2Drawer() = delete;
    CommonCurve2Drawer(Curve2Proxy c);
    void draw_on(QPainter &painter);

private:
    Curve2Proxy curve;
    SamplerProxy sampler;
};