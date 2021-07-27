#include "cell_move_router/Router/GraphApproxRouter.hpp"
#include "cell_move_router/CoordinateCodec.hpp"

namespace cell_move_router {
namespace Router {

void RoutingGraphManager::createGraph(const std::vector<long long> &LayerFactor,
                                      const std::vector<char> &LayerDir) {
  G.clear();
  auto &Codec = BondaryInfo->getCodec();
  G.setVertexNum(Codec.max());

  int MinR = BondaryInfo->getMinR();
  int MinC = BondaryInfo->getMinC();
  int MinL = BondaryInfo->getMinL();

  int MaxR = BondaryInfo->getMaxR();
  int MaxC = BondaryInfo->getMaxC();
  int MaxL = BondaryInfo->getMaxL();

  // wire
  for (int L = MinL; L <= MaxL; ++L) {
    if (LayerDir.at(L) == 'H') {
      for (int R = MinR; R <= MaxR; ++R) {
        for (int C = MinC; C < MaxC; ++C) {
          if (GridManager->getGrid(R, C, L).getSupply() <= 0 ||
              GridManager->getGrid(R, C + 1, L).getSupply() <= 0)
            continue;
          auto Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - MinL)});
          auto NeiCoord = Codec.encode({(unsigned long long)(R - MinR),
                                        (unsigned long long)(C - MinC + 1),
                                        (unsigned long long)(L - MinL)});
          long long Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
      }
    }
    if (LayerDir.at(L) == 'V') {
      for (int R = MinR; R < MaxR; ++R) {
        for (int C = MinC; C <= MaxC; ++C) {
          if (GridManager->getGrid(R, C, L).getSupply() <= 0 ||
              GridManager->getGrid(R + 1, C, L).getSupply() <= 0)
            continue;
          auto Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - MinL)});
          auto NeiCoord = Codec.encode({(unsigned long long)(R - MinR + 1),
                                        (unsigned long long)(C - MinC),
                                        (unsigned long long)(L - MinL)});
          long long Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
      }
    }
  }
  // via
  for (int L = MinL; L < MaxL; ++L) {
    for (int R = MinR; R <= MaxR; ++R) {
      for (int C = MinC; C <= MaxC; ++C) {
        if (GridManager->getGrid(R, C, L).getSupply() <= 0 ||
            GridManager->getGrid(R, C, L + 1).getSupply() <= 0)
          continue;
        auto Coord = Codec.encode({(unsigned long long)(R - MinR),
                                   (unsigned long long)(C - MinC),
                                   (unsigned long long)(L - MinL)});
        auto NeiCoord = Codec.encode({(unsigned long long)(R - MinR),
                                      (unsigned long long)(C - MinC),
                                      (unsigned long long)(L - MinL + 1)});
        long long Weight = LayerFactor.at(L) + LayerFactor.at(L + 1);
        G.addEdge(Coord, NeiCoord, Weight);
      }
    }
  }
}
std::vector<Input::Processed::Route>
RoutingGraphManager::createFinalRoute(const std::vector<size_t> &Eids,
                                      const Input::Processed::Net *Net) {
  auto Route = std::move(BondaryInfo->getRouteUnderMinLayer());
  for (auto &EdgeIdx : Eids) {
    auto &Edge = G.getEdge(EdgeIdx);
    auto Decode1 = BondaryInfo->getCodec().decode(Edge.v1);
    auto Decode2 = BondaryInfo->getCodec().decode(Edge.v2);
    int MinR = BondaryInfo->getMinR();
    int MinC = BondaryInfo->getMinC();
    int MinL = BondaryInfo->getMinL();
    unsigned long long R1 = Decode1[0] + MinR, R2 = Decode2[0] + MinR;
    unsigned long long C1 = Decode1[1] + MinC, C2 = Decode2[1] + MinC;
    unsigned long long L1 = Decode1[2] + MinL, L2 = Decode2[2] + MinL;
    Route.emplace_back(R1, C1, L1, R2, C2, L2, Net);
  }
  return Route;
}

std::pair<std::vector<Input::Processed::Route>, bool>
GraphApproxRouter::singleNetRoute(
    const Input::Processed::Net *Net,
    const std::vector<Input::Processed::Route> &OriginRoute) {
  BondaryBuilder Builder(getGridManager(), Net, OriginRoute);
  Builder.createBondaryInfo();
  if (!Builder.isBondaryInfoExist())
    return {{}, false};
  auto BondaryInfo = Builder.getBondaryInfo();
  RGM.setBondaryInfo(&BondaryInfo);
  RGM.createGraph(getLayerFactor(), getLayerDir());
  const auto &G = RGM.getGraph();
  steiner_tree::Solver<long long> Solver(G);
  auto Res = Solver.solve(BondaryInfo.getTerminals());
  if (!Res)
    return {{}, false};
  auto FinalRoute = RGM.createFinalRoute(*Res, Net);
  return {FinalRoute, true};
}

void GraphApproxRouter::rerouteAll() {
  std::vector<std::pair<const Input::Processed::Net *, long long>> NetPtrs;
  for (const auto &NetRoute : getGridManager()->getNetRoutes()) {
    NetPtrs.emplace_back(NetRoute.first, NetRoute.second.second);
  }
  auto NetCmp =
      [&](const std::pair<const Input::Processed::Net *, long long> &A,
          const std::pair<const Input::Processed::Net *, long long> &B) {
        return A.second < B.second;
      };
  std::sort(NetPtrs.begin(), NetPtrs.end(), NetCmp);
  for (auto &P : NetPtrs) {
    auto NetPtr = P.first;
    auto &OriginRoute = getGridManager()->getNetRoutes()[NetPtr];
    getGridManager()->removeNet(NetPtr);
    auto Routes = singleNetRoute(NetPtr, OriginRoute.first).first;
    Input::Processed::Route::reduceRouteSegments(Routes);
    auto Cost = getGridManager()->getRouteCost(NetPtr, Routes);
    if (Cost < OriginRoute.second) {
      OriginRoute = {std::move(Routes), Cost};
    }
    getGridManager()->addNet(NetPtr);
  }
}

} // namespace Router
} // namespace cell_move_router