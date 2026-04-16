#pragma once

#include "math/Vec.h"
#include "math/concepts.h"
#include "proxy/v4/proxy.h"
#include "proxy/v4/proxy_macros.h"
#include <vector>

namespace __detail {
PRO_DEF_MEM_DISPATCH(MemSample, sample);
PRO_DEF_MEM_DISPATCH(MemSampleTo, sample_to);
struct Sampler : pro::facade_builder                                                   //
                 ::add_convention<MemSample, std::vector<Vec2d>(Curve2Proxy)>          //
                 ::add_convention<MemSampleTo, int(Curve2Proxy, std::vector<Vec2d> &)> //
                 ::support_copy<pro::constraint_level::trivial>                        //
                 ::build {};
} // namespace __detail

using SamplerProxy = pro::proxy<__detail::Sampler>;

class Curve2dSampler {
public:
    std::vector<Vec2d> sample(Curve2Proxy c);
    int sample_to(Curve2Proxy c, std::vector<Vec2d> &points);

private:
    int n_segements = 100;
};