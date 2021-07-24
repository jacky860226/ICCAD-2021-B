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
  unsigned Tag;
  std::unordered_set<const Input::Processed::Net *> NetSet;

public:
  Grid(unsigned long long Coordinate, int Capacity)
      : Coordinate(Coordinate), Capacity(Capacity), Demand(0), Tag(0) {}

  unsigned long long getCoordinate() const { return Coordinate; }
  int getSupply() const { return Capacity - Demand; }
  int getDemand() const { return Demand; }
  int getCapacity() const { return Capacity; }
  unsigned &getTag() { return Tag; }
  const std::unordered_set<const Input::Processed::Net *> &getNetSet() const {
    return NetSet;
  }
  bool isOverflow() const { return getSupply() < 0; }
  void addDemand(int Val) { Demand += Val; }
  void addCapacity(int Val) { Capacity += Val; }
  bool addNet(const Input::Processed::Net *N) {
    if (NetSet.count(N))
      return false;
    NetSet.emplace(N);
    return true;
  }
  bool removeNet(const Input::Processed::Net *N) {
    auto It = NetSet.find(N);
    if (It == NetSet.end())
      return false;
    NetSet.erase(It);
    return true;
  }
};
} // namespace Grid
} // namespace cell_move_router