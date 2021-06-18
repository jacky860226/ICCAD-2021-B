#include "cell_move_router/Router/Router.hpp"
#include "cell_move_router/CoordinateCodec.hpp"
#include <iostream> // Test

namespace cell_move_router {
namespace Router {
bool Router::netCmp(const Input::Processed::Net *A,
                    const Input::Processed::Net *B) {
  return A->getWeight() < B->getWeight();
}

void Router::localRoute(const Input::Processed::Net *NetPtr) {

  auto MinRoutingLayConstraint = NetPtr->getMinRoutingLayConstraint();
  int MinLayerIdx = MinRoutingLayConstraint->getIdx();

  const auto &InputPtr = GridManagerPtr->getInputPtr();
  int RowBeginIdx = InputPtr->getRowBeginIdx();
  int ColBeginIdx = InputPtr->getColBeginIdx();
  int RowEndIdx = InputPtr->getRowEndIdx();
  int ColEndIdx = InputPtr->getColEndIdx();
  int LayerSz = InputPtr->getLayers().size();

  int MaxR = RowBeginIdx, MinR = RowEndIdx;
  int MaxC = ColBeginIdx, MinC = ColEndIdx;

  for (const auto &Pin : NetPtr->getPins()) {
    int Row = Pin.getInst()->getGGridRowIdx();
    int Col = Pin.getInst()->getGGridColIdx();
    MinR = std::min(MinR, Row);
    MaxR = std::max(MaxR, Row);
    MinC = std::min(MinC, Col);
    MaxC = std::max(MaxC, Col);
  }

  MinR = std::max(MinR - 5, RowBeginIdx);
  MaxR = std::min(MaxR + 5, RowEndIdx);
  MinC = std::max(MinC - 5, ColBeginIdx);
  MaxC = std::min(MaxC + 5, ColEndIdx);

  steiner_tree::UndirectedGraph<double> G;

  cell_move_router::CoordinateCodec<unsigned long long> Codec(
      {(unsigned long long)(MaxR - MinR + 1),
       (unsigned long long)(MaxC - MinC + 1), (unsigned long long)LayerSz});

  G.setVertixNum(Codec.max());

  // wire
  for (int L = MinLayerIdx; L <= LayerSz; ++L) {
    for (int R = MinR; R <= MaxR; ++R) {
      for (int C = MinC; C <= MaxC; ++C) {
        if (GridManagerPtr->getGrid(R, C, L).getSupply() <= 0)
          continue;
        size_t Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - 1)});
        if (C != MaxC && GridManagerPtr->getGrid(R, C + 1, L).getSupply() > 0) {
          size_t NeiCoord = Codec.encode({(unsigned long long)(R - MinR),
                                          (unsigned long long)(C - MinC + 1),
                                          (unsigned long long)(L - 1)});
          double Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
        if (R != MaxR && GridManagerPtr->getGrid(R + 1, C, L).getSupply() > 0) {
          size_t NeiCoord = Codec.encode({(unsigned long long)(R - MinR + 1),
                                          (unsigned long long)(C - MinC),
                                          (unsigned long long)(L - 1)});
          double Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
      }
    }
  }

  // via
  for (int L = 1; L < LayerSz; ++L) {
    for (int R = MinR; R <= MaxR; ++R) {
      for (int C = MinC; C <= MaxC; ++C) {
        if (GridManagerPtr->getGrid(R, C, L).getSupply() <= 0 ||
            GridManagerPtr->getGrid(R, C, L + 1).getSupply() <= 0)
          continue;
        size_t Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - 1)});
        size_t NeiCoord = Codec.encode({(unsigned long long)(R - MinR),
                                        (unsigned long long)(C - MinC),
                                        (unsigned long long)L});
        double Weight = LayerFactor.at(L) + LayerFactor.at(L + 1);
        G.addEdge(Coord, NeiCoord, Weight);
      }
    }
  }

  std::unordered_set<size_t> Terminals;
  for (const auto &Pin : NetPtr->getPins()) {
    int Row = Pin.getInst()->getGGridRowIdx();
    int Col = Pin.getInst()->getGGridColIdx();
    int Layer = Pin.getMasterPin()->getPinLayer()->getIdx();
    size_t Coord = Codec.encode({(unsigned long long)(Row - MinR),
                                 (unsigned long long)(Col - MinC),
                                 (unsigned long long)(Layer - 1)});
    Terminals.emplace(Coord);
  }

  steiner_tree::Solver<double> solver(G);
  auto Res = solver.solve(Terminals);

  double Cost = 0; // Test
  std::vector<Input::Processed::Route> Routes;
  for (auto &EdgeIdx : *Res) {
    auto &Edge = G.getEdge(EdgeIdx);
    Cost += Edge.cost; // Test
    auto Decode1 = Codec.decode(Edge.v1);
    auto Decode2 = Codec.decode(Edge.v2);
    unsigned long long R1 = Decode1[0] + MinR, R2 = Decode2[0] + MinR;
    unsigned long long C1 = Decode1[1] + MinC, C2 = Decode2[1] + MinC;
    unsigned long long L1 = Decode1[2] + 1, L2 = Decode2[2] + 1;
    Routes.emplace_back(R1, L1, C1, R2, L2, C2, NetPtr);
  }
  std::cerr << Cost << '\n'; // Test
  GridManagerPtr->addNet(NetPtr, std::move(Routes));
}

void Router::route() {
  std::vector<const Input::Processed::Net *> NetPtrs;
  for (const auto &NetRoute : GridManagerPtr->getNetRoutes()) {
    NetPtrs.emplace_back(NetRoute.first);
  }
  sort(NetPtrs.begin(), NetPtrs.end(), netCmp);
  for (auto NetPtr : NetPtrs) {
    GridManagerPtr->removeNet(NetPtr);
    localRoute(NetPtr);
  }
}

} // namespace Router
} // namespace cell_move_router