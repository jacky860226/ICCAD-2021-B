#include "cell_move_router/Router/BondaryBuilder.hpp"

namespace cell_move_router {
namespace Router {

bool BondaryBuilder::checkPins() const {
  for (auto &Pin : Net->getPins()) {
    auto CellPtr = Pin.getInst();
    int R = 0, C = 0;
    std::tie(R, C) = GridManager->getCellCoordinate(CellPtr);
    int L = Pin.getMasterPin()->getPinLayer()->getIdx();
    if (GridManager->getGrid(R, C, L).getSupply() <= 0)
      return false;
  }
  return true;
}

bool BondaryBuilder::checkRoute(const Input::Processed::Route &Route) const {
  int R = Route.getSRowIdx();
  int C = Route.getSColIdx();
  for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
    if (GridManager->getGrid(R, C, L).getSupply() <= 0)
      return false;
  }
  return true;
}

void BondaryBuilder::createBondaryInfo() {
  if (!checkPins()) {
    BondaryInfoExist = false;
    return;
  }
  const auto &InputPtr = GridManager->getInputPtr();
  int RowBeginIdx = InputPtr->getRowBeginIdx();
  int ColBeginIdx = InputPtr->getColBeginIdx();
  int RowEndIdx = InputPtr->getRowEndIdx();
  int ColEndIdx = InputPtr->getColEndIdx();
  int LayerSz = InputPtr->getLayers().size();
  auto MinRoutingLayConstraint = Net->getMinRoutingLayConstraint();

  MinR = RowEndIdx;
  MaxR = RowBeginIdx;
  MinC = ColEndIdx;
  MaxC = ColBeginIdx;
  MinL = LayerSz;
  MaxL = MinRoutingLayConstraint->getIdx();

  for (const auto &Pin : Net->getPins()) {
    int Row = 0, Col = 0;
    std::tie(Row, Col) = GridManager->getCellCoordinate(Pin.getInst());
    int Layer = Pin.getMasterPin()->getPinLayer()->getIdx();
    MinR = std::min(MinR, Row);
    MaxR = std::max(MaxR, Row);
    MinC = std::min(MinC, Col);
    MaxC = std::max(MaxC, Col);
    MinL = std::min(MinL, Layer);
    MaxL = std::max(MaxL, Layer);
  }

  for (const auto &Route : OriginRoute) {
    MinR = std::min(MinR, Route.getSRowIdx());
    MaxR = std::max(MaxR, Route.getERowIdx());
    MinC = std::min(MinC, Route.getSColIdx());
    MaxC = std::max(MaxC, Route.getEColIdx());
    MinL = std::min(MinL, Route.getSLayIdx());
    MaxL = std::max(MaxL, Route.getELayIdx());
  }

  MinR = std::max(MinR - 5, RowBeginIdx);
  MaxR = std::min(MaxR + 5, RowEndIdx);
  MinC = std::max(MinC - 5, ColBeginIdx);
  MaxC = std::min(MaxC + 5, ColEndIdx);
  MinL = std::max(MinL - 1, MinRoutingLayConstraint->getIdx());
  MaxL = std::min(MaxL + 1, LayerSz);

  Codec = cell_move_router::CoordinateCodec<unsigned long long>(
      {(unsigned long long)(MaxR - MinR + 1),
       (unsigned long long)(MaxC - MinC + 1),
       (unsigned long long)(MaxL - MinL + 1)});

  std::unordered_map<size_t, int> PinMinLayer;
  Terminals.clear();
  for (const auto &Pin : Net->getPins()) {
    int Row = 0, Col = 0;
    std::tie(Row, Col) = GridManager->getCellCoordinate(Pin.getInst());
    int Layer = Pin.getMasterPin()->getPinLayer()->getIdx();
    size_t Coord = Codec.encode(
        {(unsigned long long)(Row - MinR), (unsigned long long)(Col - MinC),
         (unsigned long long)(std::max(Layer, MinL) - MinL)});
    Terminals.emplace(Coord);
    if (Layer < MinL) {
      size_t Coord = Codec.encode({(unsigned long long)(Row - MinR),
                                   (unsigned long long)(Col - MinC), 0});
      auto It = PinMinLayer.find(Coord);
      if (It == PinMinLayer.end()) {
        PinMinLayer.emplace(Coord, Layer);
      } else {
        It->second = std::min(It->second, Layer);
      }
    }
  }
  RouteUnderMinLayer.clear();
  for (const auto &P : PinMinLayer) {
    auto RCL = Codec.decode(P.first);
    unsigned long long R = RCL[0] + MinR;
    unsigned long long C = RCL[1] + MinC;
    unsigned long long L = P.second;
    RouteUnderMinLayer.emplace_back(R, C, L, R, C, MinL, Net);
    if (!checkRoute(RouteUnderMinLayer.back())) {
      BondaryInfoExist = false;
      return;
    }
  }
  BondaryInfoExist = true;
}

RoutingBondaryInfo BondaryBuilder::getBondaryInfo() {
  assert(BondaryInfoExist);
  BondaryInfoExist = false;
  return RoutingBondaryInfo(MinR, MaxR, MinC, MaxC, MinL, MaxL,
                            std::move(Codec), std::move(Terminals),
                            std::move(RouteUnderMinLayer));
}

} // namespace Router
} // namespace cell_move_router