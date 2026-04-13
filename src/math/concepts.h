#pragma once
#include "math/Vector.h"
#include <proxy/proxy.h>
#include <utility>

namespace __detail {

PRO_DEF_MEM_DISPATCH(MemEval, eval);
PRO_DEF_MEM_DISPATCH(MemGetRange, get_range);

struct Curve3 : pro::facade_builder                                            //
                 ::add_convention<MemEval, Vec3d(double) const>                 //
                 ::add_convention<MemGetRange, std::pair<double, double>(void)> //
                 ::build {};

struct Curve2 : pro::facade_builder                                            //
                 ::add_convention<MemEval, Vec2d(double) const>                 //
                 ::add_convention<MemGetRange, std::pair<double, double>(void)> //
                 ::support_copy<pro::constraint_level::nontrivial>              //
                 ::build {};
} // namespace __detail

using Curve3Proxy = pro::proxy<__detail::Curve3>;

using Curve2Proxy = pro::proxy<__detail::Curve2>;