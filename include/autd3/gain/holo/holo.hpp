#pragma once

#include <ranges>
#include <vector>

#include "autd3/driver/datagram/gain/gain.hpp"
#include "autd3/gain/holo/amplitude.hpp"
#include "autd3/gain/holo/constraint.hpp"

namespace autd3::gain::holo {

template <class R>
concept holo_foci_range = std::ranges::viewable_range<R> && std::same_as<std::ranges::range_value_t<R>, std::pair<driver::Vector3, Amplitude>>;

template <class H>
class Holo : public driver::Gain<H> {
 public:
  explicit Holo(const EmissionConstraint value) : _constraint(value) {}

  void add_focus(driver::Vector3 focus, Amplitude amp) & {
    _foci.emplace_back(std::move(focus));
    _amps.emplace_back(amp);
  }

  [[nodiscard]] H add_focus(driver::Vector3 focus, Amplitude amp) && {
    this->_foci.emplace_back(std::move(focus));
    _amps.emplace_back(amp);
    return std::move(*static_cast<H*>(this));
  }

  template <holo_foci_range R>
  void add_foci_from_iter(R&& iter) & {
    for (auto [focus, amp] : iter) {
      _foci.emplace_back(std::move(focus));
      _amps.emplace_back(amp);
    }
  }

  template <holo_foci_range R>
  [[nodiscard]] H add_foci_from_iter(R&& iter) && {
    for (auto [focus, amp] : iter) {
      _foci.emplace_back(std::move(focus));
      _amps.emplace_back(amp);
    }
    return std::move(*static_cast<H*>(this));
  }

  void with_constraint(const EmissionConstraint value) & { _constraint = value; }
  [[nodiscard]] H with_constraint(const EmissionConstraint value) && {
    _constraint = value;
    return std::move(*static_cast<H*>(this));
  }

  [[nodiscard]] EmissionConstraint constraint() const { return _constraint; }

 protected:
  std::vector<driver::Vector3> _foci;
  std::vector<Amplitude> _amps;
  EmissionConstraint _constraint;
};

}  // namespace autd3::gain::holo