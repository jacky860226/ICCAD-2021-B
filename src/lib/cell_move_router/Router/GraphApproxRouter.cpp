#include "cell_move_router/Router/GraphApproxRouter.hpp"
#include "cell_move_router/CoordinateCodec.hpp"

namespace cell_move_router {
namespace Router {

void RoutingGraphManager::setGraphInfo(
    Grid::GridManager *_GridManager, const Input::Processed::Net *_Net,
    const std::vector<Input::Processed::Route> &OriginRoute) {
  GridManager = _GridManager;
  Net = _Net;

  const auto &InputPtr = GridManager->getInputPtr();
  int RowBeginIdx = InputPtr->getRowBeginIdx();
  int ColBeginIdx = InputPtr->getColBeginIdx();
  int RowEndIdx = InputPtr->getRowEndIdx();
  int ColEndIdx = InputPtr->getColEndIdx();
  int LayerSz = InputPtr->getLayers().size();
  auto MinRoutingLayConstraint = Net->getMinRoutingLayConstraint();

  MaxR = RowBeginIdx;
  MinR = RowEndIdx;
  MaxC = ColBeginIdx;
  MinC = ColEndIdx;
  MinLayerIdx = LayerSz;
  MaxLayerIdx = MinRoutingLayConstraint->getIdx();

  for (const auto &Pin : Net->getPins()) {
    int Row = Pin.getInst()->getGGridRowIdx();
    int Col = Pin.getInst()->getGGridColIdx();
    int Layer = Pin.getMasterPin()->getPinLayer()->getIdx();
    MinR = std::min(MinR, Row);
    MaxR = std::max(MaxR, Row);
    MinC = std::min(MinC, Col);
    MaxC = std::max(MaxC, Col);
    MinLayerIdx = std::min(MinLayerIdx, Layer);
    MaxLayerIdx = std::max(MaxLayerIdx, Layer);
  }

  for (const auto &Route : OriginRoute) {
    MinR = std::min(MinR, Route.getSRowIdx());
    MaxR = std::max(MaxR, Route.getERowIdx());
    MinC = std::min(MinC, Route.getSColIdx());
    MaxC = std::max(MaxC, Route.getEColIdx());
    MinLayerIdx = std::min(MinLayerIdx, Route.getSLayIdx());
    MaxLayerIdx = std::max(MaxLayerIdx, Route.getELayIdx());
  }

  MinR = std::max(MinR - 5, RowBeginIdx);
  MaxR = std::min(MaxR + 5, RowEndIdx);
  MinC = std::max(MinC - 5, ColBeginIdx);
  MaxC = std::min(MaxC + 5, ColEndIdx);
  MinLayerIdx = std::max(MinLayerIdx - 1, MinRoutingLayConstraint->getIdx());
  MaxLayerIdx = std::min(MaxLayerIdx + 1, LayerSz);

  Codec = cell_move_router::CoordinateCodec<unsigned long long>(
      {(unsigned long long)(MaxR - MinR + 1),
       (unsigned long long)(MaxC - MinC + 1),
       (unsigned long long)(MaxLayerIdx - MinLayerIdx + 1)});
}
std::vector<Input::Processed::Route>
RoutingGraphManager::createTerminalsAndRouteUnderMinLayer() {
  PinMinLayer.clear();
  Terminals.clear();
  for (const auto &Pin : Net->getPins()) {
    int Row = Pin.getInst()->getGGridRowIdx();
    int Col = Pin.getInst()->getGGridColIdx();
    int Layer = Pin.getMasterPin()->getPinLayer()->getIdx();
    size_t Coord = Codec.encode(
        {(unsigned long long)(Row - MinR), (unsigned long long)(Col - MinC),
         (unsigned long long)(std::max(Layer, MinLayerIdx) - MinLayerIdx)});
    Terminals.emplace(Coord);
    if (Layer < MinLayerIdx) {
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
  std::vector<Input::Processed::Route> RouteUnderMinLayer;
  for (const auto &P : PinMinLayer) {
    auto RCL = Codec.decode(P.first);
    unsigned long long R = RCL[0] + MinR;
    unsigned long long C = RCL[1] + MinC;
    unsigned long long L = P.second;
    RouteUnderMinLayer.emplace_back(R, C, L, R, C, MinLayerIdx, Net);
  }
  return RouteUnderMinLayer;
}
void RoutingGraphManager::createGraph(const std::vector<double> &LayerFactor,
                                      const std::vector<char> &LayerDir) {
  G.clear();
  G.setVertexNum(Codec.max());

  // wire
  for (int L = MinLayerIdx; L <= MaxLayerIdx; ++L) {
    for (int R = MinR; R <= MaxR; ++R) {
      for (int C = MinC; C <= MaxC; ++C) {
        if (GridManager->getGrid(R, C, L).getSupply() <= 0)
          continue;
        size_t Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - MinLayerIdx)});
        if (C != MaxC && GridManager->getGrid(R, C + 1, L).getSupply() > 0 &&
            LayerDir.at(L) == 'H') {
          size_t NeiCoord =
              Codec.encode({(unsigned long long)(R - MinR),
                            (unsigned long long)(C - MinC + 1),
                            (unsigned long long)(L - MinLayerIdx)});
          double Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
        if (R != MaxR && GridManager->getGrid(R + 1, C, L).getSupply() > 0 &&
            LayerDir.at(L) == 'V') {
          size_t NeiCoord =
              Codec.encode({(unsigned long long)(R - MinR + 1),
                            (unsigned long long)(C - MinC),
                            (unsigned long long)(L - MinLayerIdx)});
          double Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
      }
    }
  }
  // via
  for (int L = MinLayerIdx; L < MaxLayerIdx; ++L) {
    for (int R = MinR; R <= MaxR; ++R) {
      for (int C = MinC; C <= MaxC; ++C) {
        if (GridManager->getGrid(R, C, L).getSupply() <= 0 ||
            GridManager->getGrid(R, C, L + 1).getSupply() <= 0)
          continue;
        size_t Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - MinLayerIdx)});
        size_t NeiCoord = Codec.encode(
            {(unsigned long long)(R - MinR), (unsigned long long)(C - MinC),
             (unsigned long long)(L - MinLayerIdx + 1)});
        double Weight = LayerFactor.at(L) + LayerFactor.at(L + 1);
        G.addEdge(Coord, NeiCoord, Weight);
      }
    }
  }
}
std::vector<Input::Processed::Route> RoutingGraphManager::createFinalRoute(
    const std::vector<size_t> &Eids,
    std::vector<Input::Processed::Route> &&RouteUnderMinLayer) {
  auto Route = std::move(RouteUnderMinLayer);
  for (auto &EdgeIdx : Eids) {
    auto &Edge = G.getEdge(EdgeIdx);
    auto Decode1 = Codec.decode(Edge.v1);
    auto Decode2 = Codec.decode(Edge.v2);
    unsigned long long R1 = Decode1[0] + MinR, R2 = Decode2[0] + MinR;
    unsigned long long C1 = Decode1[1] + MinC, C2 = Decode2[1] + MinC;
    unsigned long long L1 = Decode1[2] + MinLayerIdx,
                       L2 = Decode2[2] + MinLayerIdx;
    Route.emplace_back(R1, C1, L1, R2, C2, L2, Net);
  }
  return Route;
}

std::vector<Input::Processed::Route> GraphApproxRouter::singleNetRoute(
    const Input::Processed::Net *Net,
    const std::vector<Input::Processed::Route> &OriginRoute) {

  RGM.setGraphInfo(getGridManager(), Net, OriginRoute);
  auto RouteUnderMinLayer =
      std::move(RGM.createTerminalsAndRouteUnderMinLayer());
  RGM.createGraph(getLayerFactor(), getLayerDir());
  const auto &G = RGM.getGraph();
  steiner_tree::Solver<double> solver(G);
  auto Res = solver.solve(RGM.getTerminals());
  assert(Res && "Unconnected!!");
  auto FinalRoute = RGM.createFinalRoute(*Res, std::move(RouteUnderMinLayer));
  return FinalRoute;
}

bool netCmp(const Input::Processed::Net *A, const Input::Processed::Net *B) {
  return A->getWeight() < B->getWeight();
}

void GraphApproxRouter::rerouteAll() {
  std::vector<const Input::Processed::Net *> NetPtrs;
  for (const auto &NetRoute : getGridManager()->getNetRoutes()) {
    NetPtrs.emplace_back(NetRoute.first);
  }
  sort(NetPtrs.begin(), NetPtrs.end(), netCmp);
  for (auto NetPtr : NetPtrs) {
    auto OriginRoute = getGridManager()->getNetRoutes()[NetPtr];
    getGridManager()->removeNet(NetPtr);
    auto Routes = singleNetRoute(NetPtr, OriginRoute.first);
    auto Cost = getGridManager()->getRouteCost(NetPtr, Routes);
    getGridManager()->addNet(NetPtr, std::move(Routes), Cost);
    // TODO: check net cost
    // if (Cost < OriginRoute.second)
    //   getGridManager()->addNet(NetPtr, std::move(Routes), Cost);
    // else
    //   getGridManager()->addNet(NetPtr, std::move(OriginRoute.first),
    //                            OriginRoute.second);
  }
}

} // namespace Router
} // namespace cell_move_router