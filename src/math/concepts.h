#pragma once
#include "math/Vec.h"
#include "proxy/v4/proxy.h"
#include "proxy/v4/proxy_macros.h"
#include <proxy/proxy.h>
#include <utility>
#include <vector>

namespace __detail {

PRO_DEF_MEM_DISPATCH(MemEval, eval);
PRO_DEF_MEM_DISPATCH(MemGetRange, get_range);

struct Curve3 : pro::facade_builder                                                  //
                ::add_convention<MemEval, Vec3d(double) const>                       //
                ::add_convention<MemGetRange, std::pair<double, double>(void) const> //
                ::build {};

struct Curve2 : pro::facade_builder                                                  //
                ::add_convention<MemEval, Vec2d(double) const>                       //
                ::add_convention<MemGetRange, std::pair<double, double>(void) const> //
                ::support_copy<pro::constraint_level::nontrivial>                    //
                ::build {};
} // namespace __detail

using Curve3Proxy = pro::proxy<__detail::Curve3>;

using Curve2Proxy = pro::proxy<__detail::Curve2>;

namespace __detail {
PRO_DEF_MEM_DISPATCH(MemGetCtlPts, get_control_points);

struct ControlCurve2 : pro::facade_builder                                                //
                       ::add_convention<MemGetCtlPts, const std::vector<Vec2d> &() const> //
                       ::support_copy<pro::constraint_level::nontrivial>                  //
                       ::build {};

} // namespace __detail

using ControlCurve2Proxy = pro::proxy<__detail::ControlCurve2>;