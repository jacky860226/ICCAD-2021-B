#pragma once
#include "cell_move_router/CoordinateCodec.hpp"
#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include <unordered_set>

namespace cell_move_router {
namespace Router {

class RoutingBondaryInfo {
  int MinR, MaxR;
  int MinC, MaxC;
  int MinL, MaxL;
  cell_move_router::CoordinateCodec<unsigned long long> Codec;
  std::unordered_set<size_t> Terminals;
  std::vector<Input::Processed::Route> RouteUnderMinLayer;

public:
  RoutingBondaryInfo(
      int MinR, int MaxR, int MinC, int MaxC, int MinL, int MaxL,
      cell_move_router::CoordinateCodec<unsigned long long> &&Codec,
      std::unordered_set<size_t> &&Terminals,
      std::vector<Input::Processed::Route> &&RouteUnderMinLayer)
      : MinR(MinR), MaxR(MaxR), MinC(MinC), MaxC(MaxC), MinL(MinL), MaxL(MaxL),
        Codec(std::move(Codec)), Terminals(std::move(Terminals)),
        RouteUnderMinLayer(std::move(RouteUnderMinLayer)) {}
  int getMinR() const { return MinR; }
  int getMinC() const { return MinC; }
  int getMinL() const { return MinL; }
  int getMaxR() const { return MaxR; }
  int getMaxC() const { return MaxC; }
  int getMaxL() const { return MaxL; }
  const cell_move_router::CoordinateCodec<unsigned long long> &
  getCodec() const {
    return Codec;
  }
  const std::unordered_set<size_t> &getTerminals() const { return Terminals; }
  const std::vector<Input::Processed::Route> &getRouteUnderMinLayer() const {
    return RouteUnderMinLayer;
  }
  std::vector<Input::Processed::Route> &&getRouteUnderMinLayer() {
    return std::move(RouteUnderMinLayer);
  }
};

class BondaryBuilder {
  Grid::GridManager *GridManager;
  const Input::Processed::Net *Net;
  const std::vector<Input::Processed::Route> &OriginRoute;
  bool BondaryInfoExist;

  int MinR, MaxR;
  int MinC, MaxC;
  int MinL, MaxL;
  cell_move_router::CoordinateCodec<unsigned long long> Codec;
  std::unordered_set<size_t> Terminals;
  std::vector<Input::Processed::Route> RouteUnderMinLayer;

  bool checkPins() const;
  bool checkRoute(const Input::Processed::Route &Route) const;

public:
  BondaryBuilder(Grid::GridManager *GridManager,
                 const Input::Processed::Net *Net,
                 const std::vector<Input::Processed::Route> &OriginRoute)
      : GridManager(GridManager), Net(Net), OriginRoute(OriginRoute),
        BondaryInfoExist(false), Codec({0, 0, 0}) {}
  void createBondaryInfo();
  bool isBondaryInfoExist() const { return BondaryInfoExist; }
  RoutingBondaryInfo getBondaryInfo();
};

} // namespace Router
} // namespace cell_move_router