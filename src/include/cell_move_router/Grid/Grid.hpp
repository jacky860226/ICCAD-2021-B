#pragma once
#include "cell_move_router/Input/Processed/Datas.hpp"
#include <unordered_set>
namespace cell_move_router {
namespace Grid {
class Grid {
private:
  const unsigned long long Coordinate;
  int Capacity;
  int Demand;
  std::unordered_set<const Input::Processed::Net *> NetSet;

public:
  Grid(unsigned long long Coordinate, int Capacity)
      : Coordinate(Coordinate), Capacity(Capacity), Demand(0) {}

  unsigned long long getCoordinate() const { return Coordinate; }
  int getSupply() const { return Capacity - Demand; }
  int getDemand() const { return Demand; }
  int getCapacity() const { return Capacity; }
  const std::unordered_set<const Input::Processed::Net *> &getNetSet() const {
    return NetSet;
  }
  bool isOverflow() const { return getSupply() < 0; }
  void addDemand(int Val) { Demand += Val; }
  void addCapacity(int Val) { Capacity += Val; }
  void addNet(const Input::Processed::Net *N) { NetSet.emplace(N); }
  void removeNet(const Input::Processed::Net *N) { NetSet.erase(N); }
};
} // namespace Grid
} // namespace cell_move_router