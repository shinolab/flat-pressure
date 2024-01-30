#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "autd3/driver/datagram/gain.hpp"
#include "autd3/driver/geometry/geometry.hpp"
#include "autd3/exception.hpp"
#include "autd3/gain/cache.hpp"
#include "autd3/gain/transform.hpp"
#include "autd3/native_methods.hpp"

namespace autd3::gain {

template <class F>
concept gain_group_f = requires(F f, const driver::geometry::Device& dev, const driver::geometry::Transducer& tr) {
  typename std::invoke_result_t<F, const driver::geometry::Device&, const driver::geometry::Transducer&>::value_type;
};

template <gain_group_f F>
class Group final : public driver::Gain, public IntoCache<Group<F>>, public IntoTransform<Group<F>> {
 public:
  using key_type = typename std::invoke_result_t<F, const driver::geometry::Device&, const driver::geometry::Transducer&>::value_type;

  explicit Group(F f) : _f(std::move(f)) {}

  /**
   * @brief Set gain
   *
   * @tparam G Gain
   * @param key Key
   * @param gain Gain
   */
  template <driver::gain G>
  void set(const key_type key, G&& gain) & {
    _map[key] = std::make_shared<std::remove_reference_t<G>>(std::forward<G>(gain));
  }

  /**
   * @brief Set gain
   *
   * @tparam G Gain
   * @param key Key
   * @param gain Gain
   */
  template <driver::gain G>
  Group&& set(const key_type key, G&& gain) && {
    _map[key] = std::make_shared<std::remove_reference_t<G>>(std::forward<G>(gain));
    return std::move(*this);
  }

  [[nodiscard]] native_methods::GainPtr gain_ptr(const driver::geometry::Geometry& geometry) const override {
    std::unordered_map<key_type, int32_t> keymap;

    auto view = geometry.devices() | std::views::transform([](const driver::geometry::Device& dev) { return static_cast<uint32_t>(dev.idx()); });
    const std::vector<uint32_t> device_indices(view.begin(), view.end());

    auto map = native_methods::AUTDGainGroupCreateMap(device_indices.data(), static_cast<uint32_t>(device_indices.size()));
    int32_t k = 0;
    for (const auto& dev : geometry.devices()) {
      std::vector<int32_t> m;
      m.reserve(dev.num_transducers());
      std::for_each(dev.cbegin(), dev.cend(), [this, &dev, &m, &keymap, &k](const auto& tr) {
        if (auto key = this->_f(dev, tr); key.has_value()) {
          if (!keymap.contains(key.value())) {
            keymap[key.value()] = k++;
          }
          m.emplace_back(keymap[key.value()]);
        } else {
          m.emplace_back(-1);
        }
      });
      map = AUTDGainGroupMapSet(map, static_cast<uint32_t>(dev.idx()), m.data());
    }
    std::vector<int32_t> gain_keys;
    std::vector<native_methods::GainPtr> gain_ptrs;
    for (auto& kv : _map) {
      if (!keymap.contains(kv.first)) throw AUTDException("Unknown group key");
      gain_keys.emplace_back(keymap[kv.first]);
      gain_ptrs.emplace_back(kv.second->gain_ptr(geometry));
    }

    return AUTDGainGroup(map, gain_keys.data(), gain_ptrs.data(), static_cast<uint32_t>(gain_keys.size()));
  }

 private:
  F _f;
  std::unordered_map<key_type, std::shared_ptr<Gain>> _map;
};

}  // namespace autd3::gain
