#include "cell_move_router/Router/Router.hpp"
#include "cell_move_router/CoordinateCodec.hpp"

namespace cell_move_router {
namespace Router {
bool Router::netCmp(const Input::Processed::Net &,
                    const Input::Processed::Net &) {
  // TODO
}

void Router::localRoute(const Input::Processed::Net *NetPtr) {

  auto MinRoutingLayConstraint = NetPtr->getMinRoutingLayConstraint();
  int MinLayerIdx = MinRoutingLayConstraint->getIdx();

  auto InputPtr = GridManagerPtr->getInputPtr();
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
        size_t Coord = Codec.encode({(unsigned long long)(R - MinR),
                                     (unsigned long long)(C - MinC),
                                     (unsigned long long)(L - 1)});
        if (C != MaxC) {
          size_t NeiCoord = Codec.encode({(unsigned long long)(R - MinR),
                                          (unsigned long long)(C - MinC + 1),
                                          (unsigned long long)(L - 1)});
          double Weight = LayerFactor.at(L) * 2;
          G.addEdge(Coord, NeiCoord, Weight);
        }
        if (R != MaxR) {
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
  double Cost = 0;
  for (auto &EdgeIdx : *Res) {
    Cost += G.getEdge(EdgeIdx).cost;
  }
}

void Router::route() {
  // TODO
}

} // namespace Router
} // namespace cell_move_router