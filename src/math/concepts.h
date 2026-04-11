#pragma once
#include "math/Vector.h"
#include "proxy/v4/proxy.h"
#include "proxy/v4/proxy_macros.h"
#include <proxy/proxy.h>
#include <utility>

namespace __detail {

PRO_DEF_MEM_DISPATCH(MemEval, eval);
PRO_DEF_MEM_DISPATCH(MemGetRange, get_range);

struct Curve3d : pro::facade_builder                                            //
                 ::add_convention<MemEval, Vec3d(double) const>                 //
                 ::add_convention<MemGetRange, std::pair<double, double>(void)> //
                 ::build {};

struct Curve2d : pro::facade_builder                                            //
                 ::add_convention<MemEval, Vec2d(double) const>                 //
                 ::add_convention<MemGetRange, std::pair<double, double>(void)> //
                 ::support_copy<pro::constraint_level::nontrivial>              //
                 ::build {};
} // namespace __detail

using Curve3dProxy = pro::proxy<__detail::Curve3d>;

using Curve2dProxy = pro::proxy<__detail::Curve2d>;