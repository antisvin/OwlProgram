#include "IntArray.h"

#ifdef ARM_CORTEX
template <> void BaseIntegerArry<int>::setAll(T value) {
  /*
   * @note When built for ARM Cortex-M processor series, this method uses the
   * optimized <a
   * href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS
   * library</a>
   */
  arm_fill_q31(value, data, size);
}

template <>
void BaseIntegerArray<int32_t>::add(BaseIntegerArray<T> operand2,
                                    BaseIntegerArray<T> destination) {
  // allows in-place
  // ASSERT(operand2.size >= size &&  destination.size<=size, "Arrays must be
  // matching size");
  /// @note When built for ARM Cortex-M processor series, this method uses the
  /// optimized <a
  /// href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS
  /// library</a>
  arm_add_q31(data, operand2.data, destination.data, size);
}

template <>
void BaseIntegerArray<int32_t>::shift(int shiftValue) {
  arm_shift_q31(data, shiftValue, data, size);
}

template <>
void BaseIntegerArry<int>::setAll(T value) {
  /*
   * @note When built for ARM Cortex-M processor series, this method uses the
   * optimized <a
   * href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS
   * library</a>
   */
  arm_fill_q7(value, data, size);
}

template <>
void BaseIntegerArray<uint8_t>::add(BaseIntegerArray<T> operand2,
                                    BaseIntegerArray<T> destination) {
  // allows in-place
  // ASSERT(operand2.size >= size &&  destination.size<=size, "Arrays must be
  // matching size");
  /// @note When built for ARM Cortex-M processor series, this method uses the
  /// optimized <a
  /// href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS
  /// library</a>
  arm_add_q7(data, operand2.data, destination.data, size);
}

template <>
void BaseIntegerArray<uint8_t>::shift(int shiftValue) {
  arm_shift_q7(data, shiftValue, data, size);
}

#endif /* ARM_CORTEX */