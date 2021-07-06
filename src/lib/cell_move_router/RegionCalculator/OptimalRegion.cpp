#include "cell_move_router/RegionCalculator/OptimalRegion.hpp"
#include <algorithm>
namespace cell_move_router {
namespace RegionCalculator {

int OptimalRegion::weightElement(std::vector<std::pair<int, double>> &V, const int L,
                  const int R, double W) {
  int Pivot = rand() % (R - L) + L;
  std::swap(V[Pivot], V[L]);
  int j = L;
  for (int i = L + 1; i < R; ++i) {
    if (V[j].first < V[L].first)
      std::swap(V[++j], V[L]);
  }
  std::swap(V[j], V[L]);
  double Sum = 0;
  for (int i = L; i < j; ++i)
    Sum += V[i].first * V[i].second;
  if (Sum > W)
    return weightElement(V, L, j, W);
  Sum += V[j].first * V[j].second;
  if (Sum > W)
    return V[j].first;
  return weightElement(V, j + 1, R, W - Sum);
}

std::pair<int, int>
OptimalRegion::getWeightedMedium(std::vector<std::pair<int, double>> &V) {
  double Sum = 0;
  for (const auto &P : V)
    Sum += P.first * P.second;
  int Ret = weightElement(V, 0, V.size(), Sum * 0.5);
  return {Ret, Ret};
}

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

std::tuple<int, int, int, int>
OptimalRegion::getWeightedRegion(const Input::Processed::Net *Net) {
  std::vector<std::pair<int, double>> Rows, Cols;
  for (const auto &Pin : Net->getPins()) {
    double Weight = Pin.getMasterPin()->getPinLayer()->getPowerFactor();
    Rows.emplace_back(Pin.getInst()->getGGridRowIdx(), Weight);
    Cols.emplace_back(Pin.getInst()->getGGridColIdx(), Weight);
  }
  auto RowCoord = getWeightedMedium(Rows);
  auto ColCoord = getWeightedMedium(Cols);
  return {RowCoord.first, RowCoord.second, ColCoord.first, ColCoord.second};
}

} // namespace RegionCalculator
} // namespace cell_move_router