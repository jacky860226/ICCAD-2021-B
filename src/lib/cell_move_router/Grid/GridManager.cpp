#include "cell_move_router/Grid/GridManager.hpp"
#include <iostream> // Debug
#include <unordered_map>
namespace cell_move_router {
namespace Grid {
GridManager::GridManager(const Input::Processed::Input *InputPtr)
    : InputPtr(InputPtr), Codec({InputPtr->getRowSize(), InputPtr->getColsize(),
                                 InputPtr->getLayers().size()}) {
  Grids.reserve(Codec.max());
  for (auto Layer : InputPtr->getLayers()) {
    for (size_t C = 0; C < Codec.at(1); ++C) {
      for (size_t R = 0; R < Codec.at(0); ++R) {
        // assert(R < Codec.at(0) && C < Codec.at(1) && Layer.getIdx() - 1 <
        // Codec.at(2));
        auto Coordinate = Codec.encode({R, C, (unsigned)Layer.getIdx() - 1});
        Grids.emplace_back(Coordinate, Layer.getDefaultSupplyOfOneGGrid());
      }
    }
  }
  for (auto G : InputPtr->getNonDefaultSupplyGGrids()) {
    auto Coordinate =
        coordinateTrans(G.getRowIdx(), G.getColIdx(), G.getLayIdx());
    // assert(Coordinate < Grids.size());
    Grids.at(Coordinate).addCapacity(G.getIncrOrDecrValue());
  }
  CellGrids.resize(InputPtr->getRowSize() * InputPtr->getColsize());
  for (const auto &Cell : InputPtr->getCellInsts()) {
    addCell(&Cell, Cell.getGGridRowIdx(), Cell.getGGridColIdx());
  }
  std::unordered_map<const Input::Processed::Net *,
                     std::vector<Input::Processed::Route>>
      RoutesNet;
  for (auto Route : InputPtr->getRoutes()) {
    RoutesNet[Route.getNetPtr()].emplace_back(Route);
  }
  for (auto NetPair : RoutesNet) {
    addNet(NetPair.first, std::move(NetPair.second));
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
  // assert(Row < Codec.at(0) && Col < Codec.at(1) && Layer < Codec.at(2)); //
  // bug
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

void GridManager::addNet(const Input::Processed::Net *Net,
                         std::vector<Input::Processed::Route> &&Routes) {
  unsigned Len = 0;
  for (auto &Route : Routes) {
    for (int R = Route.getSRowIdx(); R <= Route.getERowIdx(); ++R) {
      for (int C = Route.getSColIdx(); C <= Route.getEColIdx(); ++C) {
        for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
          auto Coordinate = coordinateTrans(R, C, L);
          // assert(Coordinate < Grids.size());
          auto &Grid = Grids.at(Coordinate);
          if (Grid.getNetSet().count(Net))
            continue;
          Grid.addNet(Net);
          Len++;
        }
      }
    }
  }
  NetRoutes.emplace(Net, std::make_pair(std::move(Routes), Len));
}

void GridManager::removeNet(const Input::Processed::Net *Net) {
  auto Iter = NetRoutes.find(Net);
  auto Routes = Iter->second.first;
  NetRoutes.erase(Iter);
  for (auto &Route : Routes) {
    for (int R = Route.getSRowIdx(); R <= Route.getERowIdx(); ++R) {
      for (int C = Route.getSColIdx(); C <= Route.getEColIdx(); ++C) {
        for (int L = Route.getSLayIdx(); L <= Route.getELayIdx(); ++L) {
          auto Coordinate = coordinateTrans(R, C, L);
          // assert(Coordinate < Grids.size());
          auto &Grid = Grids.at(Coordinate);
          Grid.removeNet(Net);
        }
      }
    }
  }
}

void GridManager::addCell(const Input::Processed::CellInst *CellInst,
                          const int Row, const int Col) {
  auto Coordinate = coordinateTrans(Row, Col, 1);
  CellCoordinate.emplace(CellInst, Coordinate);
  // assert(CellGrids.count(Coordinate) == 0);
  CellGrids.at(Coordinate).emplace(CellInst);
  for (const auto &BlkgPair : CellInst->getMasterCell()->getBlkgMap()) {
    auto &Blkg = BlkgPair.second;
    int LayerIdx = Blkg.getBlockageLayer()->getIdx();
    int Demand = Blkg.getDemand();
    auto BlkgCooridnate = coordinateTrans(Row, Col, LayerIdx);
    // assert(BlkgCooridnate < Grids.size());
    Grids.at(BlkgCooridnate).addDemand(Demand);
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
    // assert(BlkgCooridnate < Grids.size());
    Grids.at(BlkgCooridnate).addDemand(-Demand);
  }
}

} // namespace Grid
} // namespace cell_move_router