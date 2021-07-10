#include "cell_move_router/RegionCalculator/WeightedOptimalRegion.hpp"
#include <algorithm>
namespace cell_move_router {
namespace RegionCalculator {

int WeightedOptimalRegion::weightElement(std::vector<std::pair<int, long long>> &V,
                                         const int L, const int R, long long W) {
  int Pivot = rand() % (R - L) + L;
  std::swap(V[Pivot], V[L]);
  int j = L;
  for (int i = L + 1; i < R; ++i) {
    if (V[i].first < V[L].first)
      std::swap(V[++j], V[i]);
  }
  std::swap(V[j], V[L]);
  long long Sum = 0;
  for (int i = L; i < j; ++i)
    Sum += V[i].second;
  if (Sum > W)
    return weightElement(V, L, j, W);
  Sum += V[j].second;
  if (Sum > W)
    return V[j].first;
  return weightElement(V, j + 1, R, W - Sum);
}

std::pair<int, int> WeightedOptimalRegion::getWeightedMedium(
    std::vector<std::pair<int, long long>> &V) {
  long long Sum = 0;
  for (const auto &P : V)
    Sum += P.second;
  int Ret = weightElement(V, 0, V.size(), Sum / 2);
  return {Ret, Ret};
}

std::tuple<int, int, int, int>
WeightedOptimalRegion::getRegion(const Input::Processed::Net *Net) {
  std::vector<std::pair<int, long long>> Rows, Cols;
  for (const auto &Pin : Net->getPins()) {
    long long Weight = Pin.getMasterPin()->getPinLayer()->getPowerFactor();
    Rows.emplace_back(Pin.getInst()->getGGridRowIdx(), Weight);
    Cols.emplace_back(Pin.getInst()->getGGridColIdx(), Weight);
  }
  auto RowCoord = getWeightedMedium(Rows);
  auto ColCoord = getWeightedMedium(Cols);
  return {RowCoord.first, RowCoord.second, ColCoord.first, ColCoord.second};
}

} // namespace RegionCalculator
} // namespace cell_move_router