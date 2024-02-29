#pragma once

#include <algorithm>
#include <memory>
#include <numeric>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "autd3/driver/common/drive.hpp"
#include "autd3/driver/common/phase.hpp"
#include "autd3/driver/datagram/gain/base.hpp"
#include "autd3/driver/datagram/with_segment.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/native_methods.hpp"
#include "autd3/native_methods/utils.hpp"

namespace autd3::gain {

/**
 * @brief Gain to cache the result of calculation
 */
template <class G>
class Cache final : public driver::GainBase, public driver::IntoDatagramWithSegment<native_methods::GainPtr, Cache<G>> {
 public:
  explicit Cache(G g) : _g(std::move(g)), _cache(std::make_shared<std::unordered_map<size_t, std::vector<driver::Drive>>>()) {}

  Cache() = delete;                              // LCOV_EXCL_LINE
  Cache(const Cache& obj) = default;             // LCOV_EXCL_LINE
  Cache& operator=(const Cache& obj) = default;  // LCOV_EXCL_LINE
  Cache(Cache&& obj) = default;                  // LCOV_EXCL_LINE
  Cache& operator=(Cache&& obj) = default;       // LCOV_EXCL_LINE
  ~Cache() override = default;                   // LCOV_EXCL_LINE

  void init(const driver::geometry::Geometry& geometry) const {
    auto view = geometry.devices() | std::views::transform([](const driver::geometry::Device& dev) { return static_cast<uint32_t>(dev.idx()); });

    if (std::vector<uint32_t> device_indices(view.begin(), view.end());
        _cache->size() != device_indices.size() ||
        std::any_of(device_indices.begin(), device_indices.end(), [this](const uint32_t idx) { return !_cache->contains(idx); })) {
      const auto res = validate(native_methods::AUTDGainCalc(_g.gain_ptr(geometry), geometry.ptr()));
      for (const auto& dev : geometry.devices()) {
        std::vector<driver::Drive> drives;
        drives.resize(dev.num_transducers(), driver::Drive{driver::Phase(0), 0});
        native_methods::AUTDGainCalcGetResult(res, reinterpret_cast<native_methods::Drive*>(drives.data()), static_cast<uint32_t>(dev.idx()));
        _cache->emplace(dev.idx(), std::move(drives));
      }
      native_methods::AUTDGainCalcFreeResult(res);
    }
  }

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry& geometry) const override {
    init(geometry);
    return std::accumulate(geometry.devices().begin(), geometry.devices().end(), native_methods::AUTDGainCustom(),
                           [this](const native_methods::GainPtr acc, const driver::geometry::Device& dev) {
                             return AUTDGainCustomSet(acc, static_cast<uint32_t>(dev.idx()),
                                                      reinterpret_cast<native_methods::Drive*>(_cache->at(dev.idx()).data()),
                                                      static_cast<uint32_t>(_cache->at(dev.idx()).size()));
                           });
  }

  [[nodiscard]] const std::unordered_map<size_t, std::vector<driver::Drive>>& drives() const { return *_cache; }

  [[nodiscard]] const std::vector<driver::Drive>& operator[](const driver::geometry::Device& dev) const { return _cache->at(dev.idx()); }

 private:
  G _g;
  mutable std::shared_ptr<std::unordered_map<size_t, std::vector<driver::Drive>>> _cache;
};
}  // namespace autd3::gain

namespace autd3::driver {
template <class G>
class IntoGainCache {
 public:
  IntoGainCache() = default;                                     // LCOV_EXCL_LINE
  IntoGainCache(const IntoGainCache& obj) = default;             // LCOV_EXCL_LINE
  IntoGainCache& operator=(const IntoGainCache& obj) = default;  // LCOV_EXCL_LINE
  IntoGainCache(IntoGainCache&& obj) = default;                  // LCOV_EXCL_LINE
  IntoGainCache& operator=(IntoGainCache&& obj) = default;       // LCOV_EXCL_LINE
  virtual ~IntoGainCache() = default;                            // LCOV_EXCL_LINE

  [[nodiscard]] gain::Cache<G> with_cache() & { return gain::Cache(*static_cast<G*>(this)); }
  [[nodiscard]] gain::Cache<G> with_cache() && { return gain::Cache(std::move(*static_cast<G*>(this))); }
};

}  // namespace autd3::driver
