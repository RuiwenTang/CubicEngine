#pragma once

#include <cubic/render/types.h>

namespace cubic {

struct Sampler {
  FilterType mag_filter = FilterType::kLinear;
  FilterType min_filter = FilterType::kLinear;

  AddressMode map_u = AddressMode::kRepeat;
  AddressMode map_v = AddressMode::kRepeat;

  bool operator==(const Sampler& other) const {
    return mag_filter == other.mag_filter && min_filter == other.min_filter && map_u == other.map_u &&
           map_v == other.map_v;
  }
};

}  // namespace cubic
