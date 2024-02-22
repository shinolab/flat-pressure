

#pragma once

#include "autd3/exception.hpp"

#define AUTD3_DEF_PROP(PARAM_T, PARAM_NAME)                          \
  [[nodiscard]] PARAM_T PARAM_NAME() const { return _##PARAM_NAME; } \
                                                                     \
 private:                                                            \
  PARAM_T _##PARAM_NAME;                                             \
                                                                     \
 public:

#define AUTD3_DEF_PARAM(T, PARAM_T, PARAM_NAME)                           \
  void with_##PARAM_NAME(const PARAM_T value)& { _##PARAM_NAME = value; } \
  [[nodiscard]] T&& with_##PARAM_NAME(const PARAM_T value)&& {            \
    _##PARAM_NAME = value;                                                \
    return std::move(*this);                                              \
  }                                                                       \
  [[nodiscard]] PARAM_T PARAM_NAME() const { return _##PARAM_NAME; }      \
                                                                          \
 private:                                                                 \
  PARAM_T _##PARAM_NAME;                                                  \
                                                                          \
 public:

#define AUTD3_DEF_PARAM_INTENSITY(T, PARAM_NAME)                                                 \
  void with_##PARAM_NAME(const uint8_t value)& { _##PARAM_NAME = driver::EmitIntensity(value); } \
  [[nodiscard]] T&& with_##PARAM_NAME(const uint8_t value)&& {                                   \
    _##PARAM_NAME = driver::EmitIntensity(value);                                                \
    return std::move(*this);                                                                     \
  }                                                                                              \
  void with_##PARAM_NAME(const driver::EmitIntensity value)& { _##PARAM_NAME = value; }          \
  [[nodiscard]] T&& with_##PARAM_NAME(const driver::EmitIntensity value)&& {                     \
    _##PARAM_NAME = value;                                                                       \
    return std::move(*this);                                                                     \
  }                                                                                              \
                                                                                                 \
  [[nodiscard]] driver::EmitIntensity PARAM_NAME() const { return _##PARAM_NAME; }               \
                                                                                                 \
 private:                                                                                        \
  driver::EmitIntensity _##PARAM_NAME;                                                           \
                                                                                                 \
 public:

namespace autd3::native_methods {

template <class T>
concept result_ptr = requires(T& x) { x.result._0; };

template <result_ptr T>
constexpr auto validate(T res) {
  if (res.result._0 == nullptr) {
    const std::string err(res.err_len, ' ');
    AUTDGetErr(res.err, const_cast<char*>(err.c_str()));
    throw AUTDException(err);
  }
  return res.result;
}

template <typename T = int32_t>
constexpr T validate(ResultI32 res) {
  const auto [result, err_len, err] = res;
  if (result == AUTD3_ERR) {
    const std::string err_str(err_len, ' ');
    AUTDGetErr(err, const_cast<char*>(err_str.c_str()));
    throw AUTDException(err_str);
  }
  return static_cast<T>(result);
}

static inline SamplingConfiguration validate(ResultSamplingConfig res) {
  const auto [result, err_len, err] = res;
  if (result.div == 0) {
    const std::string err_str(err_len, ' ');
    AUTDGetErr(err, const_cast<char*>(err_str.c_str()));
    throw AUTDException(err_str);
  }
  return result;
}

}  // namespace autd3::native_methods
