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

public:
  Grid(unsigned long long Coordinate, int Capacity)
      : Coordinate(Coordinate), Capacity(Capacity), Demand(0), Tag(0) {}

  unsigned long long getCoordinate() const { return Coordinate; }
  int getSupply() const { return Capacity - Demand; }
  int getDemand() const { return Demand; }
  int getCapacity() const { return Capacity; }
  unsigned &getTag() { return Tag; }
  bool isOverflow() const { return getSupply() < 0; }
  void addDemand(int Val) { Demand += Val; }
  void addCapacity(int Val) { Capacity += Val; }
};
} // namespace Grid
} // namespace cell_move_router