#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "autd3/driver/common/drive.hpp"
#include "autd3/driver/datagram/gain/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain {

template <class F>
concept gain_transform = requires(F f, const driver::geometry::Device& dev, const driver::geometry::Transducer& tr) {
  { f(dev, tr) } -> std::same_as<driver::Drive>;
};

template <class G>
class Gain : public driver::Gain<G> {
 public:
  Gain() = default;

  AUTD3_API [[nodiscard]] virtual std::unordered_map<size_t, std::vector<driver::Drive>> calc(const driver::geometry::Geometry& geometry) const = 0;

  AUTD3_API [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry& geometry) const override {
    const auto drives = calc(geometry);
    return std::accumulate(drives.begin(), drives.end(), native_methods::AUTDGainCustom(),
                           [](const native_methods::GainPtr acc, const std::pair<size_t, std::vector<driver::Drive>>& kv) {
                             return AUTDGainCustomSet(acc, static_cast<uint32_t>(kv.first),
                                                      reinterpret_cast<const native_methods::Drive*>(kv.second.data()),
                                                      static_cast<uint32_t>(kv.second.size()));
                           });
  }

  template <gain_transform Fn>
  AUTD3_API [[nodiscard]] static std::unordered_map<size_t, std::vector<driver::Drive>> transform(const driver::geometry::Geometry& geometry,
                                                                                                  Fn func) {
    std::unordered_map<size_t, std::vector<driver::Drive>> drives_map;
    std::for_each(geometry.devices().begin(), geometry.devices().end(), [&drives_map, &func](const driver::geometry::Device& dev) {
      std::vector<driver::Drive> drives;
      drives.reserve(dev.num_transducers());
      std::transform(dev.cbegin(), dev.cend(), std::back_inserter(drives),
                     [&dev, &drives_map, &func](const driver::geometry::Transducer& tr) { return func(dev, tr); });
      drives_map[dev.idx()] = std::move(drives);
    });
    return drives_map;
  }  // LCOV_EXCL_LINE
};

}  // namespace autd3::gain
