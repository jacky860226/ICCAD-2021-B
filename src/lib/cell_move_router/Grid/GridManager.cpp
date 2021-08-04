#include "cell_move_router/Grid/GridManager.hpp"
#include <unordered_map>
namespace cell_move_router {
namespace Grid {
GridManager::GridManager(const Input::Processed::Input *InputPtr)
    : InputPtr(InputPtr), Codec({InputPtr->getRowSize(), InputPtr->getColsize(),
                                 InputPtr->getLayers().size()}),
      Tag(0), CurrentCost(0) {
  Grids.reserve(Codec.max());
  for (auto Layer : InputPtr->getLayers()) {
    for (size_t C = 0; C < Codec.at(1); ++C) {
      for (size_t R = 0; R < Codec.at(0); ++R) {
        assert(R < Codec.at(0) && C < Codec.at(1) &&
               Layer.getIdx() - 1 < int(Codec.at(2)));
        auto Coordinate = Codec.encode({R, C, (unsigned)Layer.getIdx() - 1});
        Grids.emplace_back(Coordinate, Layer.getDefaultSupplyOfOneGGrid());
      }
    }
  }
  for (auto G : InputPtr->getNonDefaultSupplyGGrids()) {
    auto Coordinate =
        coordinateTrans(G.getRowIdx(), G.getColIdx(), G.getLayIdx());
    assert(Coordinate < Grids.size());
    Grids.at(Coordinate).addCapacity(G.getIncrOrDecrValue());
  }
  CellGrids.resize(InputPtr->getRowSize() * InputPtr->getColsize());
  for (const auto &Cell : InputPtr->getCellInsts()) {
    CellVoltageArea[&Cell].clear();
    addCell(&Cell, Cell.getGGridRowIdx(), Cell.getGGridColIdx());
  }
  for (auto &Net : InputPtr->getNets()) {
    NetRoutes[&Net].first.clear();
  }
  for (auto Route : InputPtr->getRoutes()) {
    NetRoutes[Route.getNetPtr()].first.emplace_back(Route);
  }
  for (auto &NetPair : NetRoutes) {
    Input::Processed::Route::reduceRouteSegments(NetPair.second.first);
    NetPair.second.second = getRouteCost(NetPair.first, NetPair.second.first);
    addNet(NetPair.first);
  }
  for (auto &VoltageArea : InputPtr->getVoltageAreas()) {
    for (auto GGrid : VoltageArea.getGGrids()) {
      int Row = GGrid.getGGridRowIdx();
      int Col = GGrid.getGGridColIdx();
      auto Coordinate = coordinateTrans(Row, Col, 1);
      for (auto Cell : VoltageArea.getInstances()) {
        CellVoltageArea[Cell].emplace(Coordinate);
      }
    }
  }
}

unsigned long long GridManager::coordinateTrans(int R, int C, int L) const {
  unsigned Row = R - InputPtr->getRowBeginIdx();
  unsigned Col = C - InputPtr->getColBeginIdx();
  unsigned Layer = L - 1;
  assert(Row < Codec.at(0) && Col < Codec.at(1) && Layer < Codec.at(2));
  return Codec.encode({Row, Col, Layer});
}

std::tuple<int, int, int>
GridManager::coordinateInv(unsigned long long Coordinate) const {
  auto Coordinates = Codec.decode(Coordinate);
  int R = Coordinates[0] + InputPtr->getRowBeginIdx();
  int C = Coordinates[1] + InputPtr->getColBeginIdx();
  int L = Coordinates[2] + 1;
  return std::make_tuple(R, C, L);
}

void GridManager::addNet(const Input::Processed::Net *Net) {
  increaseTag();
  auto tryAddNet = [&](int R, int C, int L) {
    auto Coordinate = coordinateTrans(R, C, L);
    assert(Coordinate < Grids.size());
    auto &Grid = Grids.at(Coordinate);
    if (Grid.getTag() == Tag)
      return;
    Grid.getTag() = Tag;
    bool IsOverflow = Grid.isOverflow();
    Grid.addDemand(1);
    if (!IsOverflow && Grid.isOverflow()) {
      OverflowGrids.emplace(Coordinate);
    }
  };
  for (auto &Pin : Net->getPins()) {
    auto CellPtr = Pin.getInst();
    int R = 0, C = 0;
    std::tie(R, C) = getCellCoordinate(CellPtr);
    int L = Pin.getMasterPin()->getPinLayer()->getIdx();
    tryAddNet(R, C, L);
  }
  auto Iter = NetRoutes.find(Net);
  CurrentCost += Iter->second.second;
  auto &Routes = Iter->second.first;
  for (auto &Route : Routes) {
    for (int R = Route.getSRowIdx(); R <= Route.getERowIdx(); ++R) {
      for (int C = Route.getSColIdx(); C <= Route.getEColIdx(); ++C) {
        for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
          tryAddNet(R, C, L);
        }
      }
    }
  }
}

void GridManager::removeNet(const Input::Processed::Net *Net) {
  increaseTag();
  auto tryRemoveNet = [&](int R, int C, int L) {
    auto Coordinate = coordinateTrans(R, C, L);
    assert(Coordinate < Grids.size());
    auto &Grid = Grids.at(Coordinate);
    if (Grid.getTag() == Tag)
      return;
    Grid.getTag() = Tag;
    bool IsOverflow = Grid.isOverflow();
    Grid.addDemand(-1);
    if (IsOverflow && !Grid.isOverflow()) {
      OverflowGrids.erase(Coordinate);
    }
  };
  for (auto &Pin : Net->getPins()) {
    auto CellPtr = Pin.getInst();
    int R = 0, C = 0;
    std::tie(R, C) = getCellCoordinate(CellPtr);
    int L = Pin.getMasterPin()->getPinLayer()->getIdx();
    tryRemoveNet(R, C, L);
  }
  auto Iter = NetRoutes.find(Net);
  CurrentCost -= Iter->second.second;
  auto &Routes = Iter->second.first;
  for (auto &Route : Routes) {
    for (int R = Route.getSRowIdx(); R <= Route.getERowIdx(); ++R) {
      for (int C = Route.getSColIdx(); C <= Route.getEColIdx(); ++C) {
        for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
          tryRemoveNet(R, C, L);
        }
      }
    }
  }
}

void GridManager::addCell(const Input::Processed::CellInst *CellInst,
                          const int Row, const int Col) {
  auto Coordinate = coordinateTrans(Row, Col, 1);
  CellCoordinate.emplace(CellInst, Coordinate);
  CellGrids.at(Coordinate).emplace(CellInst);
  for (const auto &BlkgPair : CellInst->getMasterCell()->getBlkgMap()) {
    auto &Blkg = BlkgPair.second;
    int LayerIdx = Blkg.getBlockageLayer()->getIdx();
    int Demand = Blkg.getDemand();
    auto BlkgCooridnate = coordinateTrans(Row, Col, LayerIdx);
    assert(BlkgCooridnate < Grids.size());
    Grids.at(BlkgCooridnate).addDemand(Demand);
    if (Grids.at(BlkgCooridnate).isOverflow()) {
      OverflowGrids.emplace(BlkgCooridnate);
    }
  }
}

void GridManager::removeCell(const Input::Processed::CellInst *CellInst) {
  auto Iter = CellCoordinate.find(CellInst);
  auto Coordinate = Iter->second;
  CellCoordinate.erase(Iter);
  int Row, Col;
  std::tie(Row, Col, std::ignore) = coordinateInv(Coordinate);
  CellGrids.at(Coordinate).erase(CellInst);
  for (const auto &BlkgPair : CellInst->getMasterCell()->getBlkgMap()) {
    auto &Blkg = BlkgPair.second;
    int LayerIdx = Blkg.getBlockageLayer()->getIdx();
    int Demand = Blkg.getDemand();
    auto BlkgCooridnate = coordinateTrans(Row, Col, LayerIdx);
    assert(BlkgCooridnate < Grids.size());
    bool IsOverflow = Grids.at(BlkgCooridnate).isOverflow();
    Grids.at(BlkgCooridnate).addDemand(-Demand);
    if (IsOverflow && !Grids.at(BlkgCooridnate).isOverflow()) {
      OverflowGrids.erase(BlkgCooridnate);
    }
  }
}

long long
GridManager::getRouteCost(const Input::Processed::Net *Net,
                          const std::vector<Input::Processed::Route> &Routes) {
  unsigned long long Ans = 0;
  increaseTag();
  auto Calculate = [&](int R, int C, int L) {
    auto Coordinate = coordinateTrans(R, C, L);
    auto &Grid = Grids.at(Coordinate);
    if (Grid.getTag() == Tag)
      return;
    Grid.getTag() = Tag;
    Ans += InputPtr->getLayers()[L - 1].getPowerFactor();
  };
  for (auto &Pin : Net->getPins()) {
    auto CellPtr = Pin.getInst();
    int R = 0, C = 0;
    std::tie(R, C) = getCellCoordinate(CellPtr);
    int L = Pin.getMasterPin()->getPinLayer()->getIdx();
    Calculate(R, C, L);
  }
  for (auto &Route : Routes) {
    for (int R = Route.getSRowIdx(); R <= Route.getERowIdx(); ++R) {
      for (int C = Route.getSColIdx(); C <= Route.getEColIdx(); ++C) {
        for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
          Calculate(R, C, L);
        }
      }
    }
  }
  return Ans * Net->getWeight();
}

void GridManager::to_ostream(std::ostream &out) const {
  std::vector<const std::pair<const Input::Processed::CellInst *const,
                              unsigned long long> *>
      MovedCellInsts;
  for (auto &P : CellCoordinate) {
    if (coordinateTrans(P.first->getGGridRowIdx(), P.first->getGGridColIdx(),
                        1) != P.second) {
      MovedCellInsts.emplace_back(&P);
    }
  }
  out << "NumMovedCellInst " << MovedCellInsts.size() << '\n';
  for (auto P : MovedCellInsts) {
    auto Coord = coordinateInv(P->second);
    out << "CellInst " << P->first->getInstName() << ' ' << std::get<0>(Coord)
        << ' ' << std::get<1>(Coord) << '\n';
  }
  int NumRoute = 0;
  for (const auto &NetRoute : NetRoutes) {
    NumRoute += NetRoute.second.first.size();
  }
  out << "NumRoutes " << NumRoute << '\n';
  for (const auto &NetRoute : NetRoutes) {
    for (const auto Route : NetRoute.second.first) {
      Route.to_ostream(out);
    }
  }
}

} // namespace Grid
} // namespace cell_move_router