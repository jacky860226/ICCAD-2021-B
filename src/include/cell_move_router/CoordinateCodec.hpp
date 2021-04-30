#pragma once
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <vector>

namespace cell_move_router {
template <class SizeTy = std::size_t>
struct CoordinateCodec : private std::vector<SizeTy> {
  CoordinateCodec(std::initializer_list<SizeTy> L) : std::vector<SizeTy>(L) {}
  const SizeTy &at(std::size_t Id) const { return std::vector<SizeTy>::at(Id); }
  size_t dimension() const { return this->size(); }
  SizeTy max() const {
    SizeTy All = 1;
    for (auto i : *this)
      All *= i;
    return All;
  }
  SizeTy encode(const std::vector<SizeTy> &Coord) const {
    assert(Coord.size() == this->size() && "Dimension error!");
    SizeTy Id = 0;
    for (size_t i = 0; i < this->size(); ++i) {
      assert(Coord.at(i) < this->at(i) && "Coordinate error!");
      SizeTy Cur = Coord.at(i);
      for (size_t j = 0; j < i; ++j)
        Cur *= this->at(j);
      Id += Cur;
    }
    return Id;
  }
  std::vector<SizeTy> decode(SizeTy Id) const {
    assert(Id <= this->max() && "Coordinate error!");
    std::vector<SizeTy> Ans(this->size());
    for (int i = int(this->size()) - 1; i >= 0; --i) {
      SizeTy Cur = 1;
      for (int j = 0; j < i; ++j)
        Cur *= this->at(j);
      Ans.at(i) = Id / Cur;
      Id %= Cur;
    }
    return Ans;
  }
};
} // namespace cell_move_router