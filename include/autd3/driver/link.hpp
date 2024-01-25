#pragma once

#include "autd3/native_methods.hpp"

namespace autd3::driver {

template <class T>
concept link_builder = requires(T t) {
  typename T::Link;
  { t.ptr() } -> std::same_as<native_methods::LinkBuilderPtr>;
};

}  // namespace autd3::driver
