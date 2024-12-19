// Generates code for every target that this compiler can support.
#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "hwy-add.cpp"  // this file
#include <hwy/foreach_target.h>  // must come before highway.h
#include <hwy/highway.h>

#include <fmt/core.h>

namespace project {
namespace HWY_NAMESPACE {  // required: unique per target

// Can skip hn:: prefixes if already inside hwy::HWY_NAMESPACE.
namespace hn = hwy::HWY_NAMESPACE;

using T = float;

// Alternative to per-function HWY_ATTR: see HWY_BEFORE_NAMESPACE
HWY_ATTR void AddLoop(T* HWY_RESTRICT arr,
                size_t size) {
  const hn::ScalableTag<T> d;
  fmt::print("Lanes: {}\n", hn::Lanes(d));
  for (size_t i = 0; i < size; i += hn::Lanes(d)) {

    auto x = hn::Load(d, arr + i);
    x = hn::Add(x, hn::Set(d, 1.0));
    hn::Store(x, d, arr + i);
  }
}

}  // namespace HWY_NAMESPACE
}  // namespace projec


// The table of pointers to the various implementations in HWY_NAMESPACE must
// be compiled only once (foreach_target #includes this file multiple times).
// HWY_ONCE is true for only one of these 'compilation passes'.
#if HWY_ONCE

namespace project {

// This macro declares a static array used for dynamic dispatch.
HWY_EXPORT(AddLoop);

void CallAddLoop(float* HWY_RESTRICT arr,
                size_t size) {
  // This must reside outside of HWY_NAMESPACE because it references (calls the
  // appropriate one from) the per-target implementations there.
  // For static dispatch, use HWY_STATIC_DISPATCH.
  return HWY_DYNAMIC_DISPATCH(AddLoop)(arr,size);
}

}  // namespace project
#endif  // HWY_ONCE

