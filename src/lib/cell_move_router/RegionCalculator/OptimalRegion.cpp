#include <algorithm>
#include <cell_move_router/RegionCalculator/OptimalRegion.hpp>
namespace cell_move_router {
namespace RegionCalculator {

std::pair<int, int> OptimalRegion::getMedium(std::vector<int> &V) {
  int L = (V.size() - 1) >> 1;
  int R = V.size() >> 1;
  std::nth_element(V.begin(), V.begin() + R, V.end());
  std::nth_element(V.begin(), V.begin() + L, V.begin() + R);
  return std::make_pair(V[L], V[R]);
}

std::tuple<int, int, int, int>
OptimalRegion::getRegion(const Input::Processed::Net *NetPtr) {
  std::vector<int> Rows, Cols;
  for (const auto &Pin : NetPtr->getPins()) {
    Rows.emplace_back(Pin.getInst()->getGGridRowIdx());
    Cols.emplace_back(Pin.getInst()->getGGridColIdx());
  }
  auto RowCoord = getMedium(Rows);
  auto ColCoord = getMedium(Cols);
  return std::make_tuple(RowCoord.first, RowCoord.second, ColCoord.first,
                         ColCoord.second);
}

} // namespace RegionCalculator
} // namespace cell_move_router