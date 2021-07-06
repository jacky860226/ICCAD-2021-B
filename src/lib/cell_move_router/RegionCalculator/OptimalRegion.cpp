#include "cell_move_router/RegionCalculator/OptimalRegion.hpp"
#include <algorithm>
namespace cell_move_router {
namespace RegionCalculator {

std::pair<int, int> OptimalRegion::getMedium(std::vector<int> &V) {
  int L = (V.size() - 1) / 2;
  int R = V.size() / 2;
  std::nth_element(V.begin(), V.begin() + R, V.end());
  std::nth_element(V.begin(), V.begin() + L, V.begin() + R);
  return {V[L], V[R]};
}

std::tuple<int, int, int, int>
OptimalRegion::getRegion(const Input::Processed::Net *Net) {
  std::vector<int> Rows, Cols;
  for (const auto &Pin : Net->getPins()) {
    Rows.emplace_back(Pin.getInst()->getGGridRowIdx());
    Cols.emplace_back(Pin.getInst()->getGGridColIdx());
  }
  auto RowCoord = getMedium(Rows);
  auto ColCoord = getMedium(Cols);
  return {RowCoord.first, RowCoord.second, ColCoord.first, ColCoord.second};
}

} // namespace RegionCalculator
} // namespace cell_move_router