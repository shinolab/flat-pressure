#pragma once

#include "autd3.hpp"

template <typename L>
inline void plane_test(autd3::Controller<L>& autd) {
    auto silencer = autd3::ConfigureSilencer::default_();
    autd.send(silencer);

    autd3::modulation::Sine m(150);  // 150Hz AM

    const autd3::Vector3 direction = autd3::Vector3::UnitZ();
    autd3::gain::Plane g(direction);

    autd.send(m, g);
}
