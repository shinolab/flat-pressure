#pragma once

#include <string>

#include "autd3/controller/builder.hpp"
#include "autd3/controller/controller.hpp"
#include "autd3/def.hpp"
#include "autd3/driver/autd3_device.hpp"
#include "autd3/driver/common/drive.hpp"
#include "autd3/driver/common/emit_intensity.hpp"
#include "autd3/driver/common/phase.hpp"
#include "autd3/driver/common/sampling_config.hpp"
#include "autd3/driver/datagram/clear.hpp"
#include "autd3/driver/datagram/datagram.hpp"
#include "autd3/driver/datagram/debug.hpp"
#include "autd3/driver/datagram/force_fan.hpp"
#include "autd3/driver/datagram/phase_filter.hpp"
#include "autd3/driver/datagram/reads_fpga_state.hpp"
#include "autd3/driver/datagram/silencer.hpp"
#include "autd3/driver/datagram/stm/focus.hpp"
#include "autd3/driver/datagram/stm/gain.hpp"
#include "autd3/driver/geometry/device.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/driver/geometry/rotation.hpp"
#include "autd3/driver/geometry/transducer.hpp"
#include "autd3/gain/bessel.hpp"
#include "autd3/gain/focus.hpp"
#include "autd3/gain/gain.hpp"
#include "autd3/gain/group.hpp"
#include "autd3/gain/null.hpp"
#include "autd3/gain/plane.hpp"
#include "autd3/gain/trans_test.hpp"
#include "autd3/gain/uniform.hpp"
#include "autd3/modulation/fourier.hpp"
#include "autd3/modulation/modulation.hpp"
#include "autd3/modulation/sine.hpp"
#include "autd3/modulation/square.hpp"
#include "autd3/modulation/static.hpp"

namespace autd3 {

/**
 * @brief Mathematical constant pi
 */
constexpr double pi = driver::pi;

/**
 * @brief AUTD3 software version
 */
static inline std::string version = "22.0.1";

using driver::AUTD3;
using driver::geometry::deg;
using driver::geometry::Device;
using driver::geometry::EulerAngles;
using driver::geometry::Geometry;
using driver::geometry::rad;
using driver::geometry::Transducer;

using driver::Drive;
using driver::EmitIntensity;
using driver::LoopBehavior;
using driver::Phase;
using gain::Gain;
constexpr driver::UnitPhaseRad phase_rad = driver::rad;
using driver::SamplingConfiguration;
using modulation::Modulation;

using driver::ChangeFocusSTMSegment;
using driver::ChangeGainSegment;
using driver::ChangeGainSTMSegment;
using driver::ChangeModulationSegment;
using driver::Clear;
using driver::ConfigureDebugOutputIdx;
using driver::ConfigureForceFan;
using driver::ConfigurePhaseFilter;
using driver::ConfigureReadsFPGAState;
using driver::ConfigureSilencer;
using driver::ControlPoint;
using driver::FocusSTM;
using driver::GainSTM;
using native_methods::GainSTMMode;

using gain::Bessel;
using gain::Focus;
using gain::Group;
using gain::Null;
using gain::Plane;
using gain::TransducerTest;
using gain::Uniform;

using modulation::SamplingMode;
using modulation::Sine;
using modulation::Square;
using modulation::Static;

using driver::FirmwareInfo;
using driver::FPGAState;

using driver::Quaternion;
using driver::Vector3;

using controller::Controller;
using controller::ControllerBuilder;

using native_methods::TimerStrategy;

using native_methods::Segment;

}  // namespace autd3
