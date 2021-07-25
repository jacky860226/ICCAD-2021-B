#include "cell_move_router/Parser.hpp"
#include <algorithm>
#include <cassert>

namespace cell_move_router {
unsigned Parser::parseMaxCellMove(std::istream &input) {
  unsigned MaxCellMove = 0;
  input >> MaxCellMove;
  return MaxCellMove;
}
std::tuple<int, int, int, int>
Parser::parseGGridBoundaryIdx(std::istream &input) {
  int RowBeginIdx = 0, ColBeginIdx = 0, RowEndIdx = 0, ColEndIdx = 0;
  input >> RowBeginIdx >> ColBeginIdx >> RowEndIdx >> ColEndIdx;
  return std::make_tuple(RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx);
}

std::vector<Input::Raw::Layer> Parser::parseLayers(std::istream &input) {
  int LayerCount = 0;
  input >> LayerCount;
  std::vector<Input::Raw::Layer> Layers;
  for (int i = 0; i < LayerCount; ++i) {
    std::string buff;
    input >> buff;
    assert(buff == "Lay");
    std::string LayerName;
    int Idx;
    char RoutingDir;
    int DefaultSupplyOfOneGGrid;
    double PowerFactor;
    input >> LayerName >> Idx >> RoutingDir >> DefaultSupplyOfOneGGrid >>
        PowerFactor;
    Layers.emplace_back(std::move(LayerName), Idx, RoutingDir,
                        DefaultSupplyOfOneGGrid, PowerFactor * 100);
  }
  std::sort(
      Layers.begin(), Layers.end(),
      [&](const Input::Raw::Layer &A, const Input::Raw::Layer &B) -> bool {
        return A.getIdx() < B.getIdx();
      });
  return Layers;
}

std::vector<Input::Raw::NonDefaultSupplyGGrid>
Parser::parseNonDefaultSupplyGGrids(std::istream &input) {
  int NumNonDefaultSupplyGGrid = 0;
  input >> NumNonDefaultSupplyGGrid;
  std::vector<Input::Raw::NonDefaultSupplyGGrid> NonDefaultSupplyGGrids;
  for (int i = 0; i < NumNonDefaultSupplyGGrid; ++i) {
    int RowIdx, ColIdx, LayIdx;
    int IncrOrDecrValue;
    input >> RowIdx >> ColIdx >> LayIdx >> IncrOrDecrValue;
    NonDefaultSupplyGGrids.emplace_back(RowIdx, ColIdx, LayIdx,
                                        IncrOrDecrValue);
  }
  return NonDefaultSupplyGGrids;
}

Input::Raw::MasterCell::Pin parsePin(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "Pin");
  std::string PinName;
  std::string PinLayer;
  input >> PinName >> PinLayer;
  return Input::Raw::MasterCell::Pin(std::move(PinName), std::move(PinLayer));
}

Input::Raw::MasterCell::Blkg parseBlkg(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "Blkg");
  std::string BlockageName;
  std::string BlockageLayer;
  int Demand;
  input >> BlockageName >> BlockageLayer >> Demand;
  return Input::Raw::MasterCell::Blkg(std::move(BlockageName),
                                      std::move(BlockageLayer), Demand);
}

std::vector<Input::Raw::MasterCell>
Parser::parseMasterCells(std::istream &input) {
  int NumMasterCell = 0;
  input >> NumMasterCell;
  std::vector<Input::Raw::MasterCell> MasterCells;
  for (int i = 0; i < NumMasterCell; ++i) {
    std::string buff;
    input >> buff;
    assert(buff == "MasterCell");
    std::string MasterCellName;
    int PinCount = 0;
    int BlkgCount = 0;
    input >> MasterCellName >> PinCount >> BlkgCount;
    std::vector<Input::Raw::MasterCell::Pin> Pins;
    std::vector<Input::Raw::MasterCell::Blkg> Blkgs;
    for (int j = 0; j < PinCount; j++) {
      Pins.emplace_back(parsePin(input));
    }
    for (int j = 0; j < BlkgCount; j++) {
      Blkgs.emplace_back(parseBlkg(input));
    }
    MasterCells.emplace_back(std::move(MasterCellName), std::move(Pins),
                             std::move(Blkgs));
  }
  return MasterCells;
}

std::vector<Input::Raw::CellInst> Parser::parseCellInsts(std::istream &input) {
  int CellInstCount;
  input >> CellInstCount;
  std::vector<Input::Raw::CellInst> CellInsts;
  for (int i = 0; i < CellInstCount; ++i) {
    std::string buff;
    input >> buff;
    assert(buff == "CellInst");
    std::string InstName;
    std::string MasterCellName;
    int GGridRowIdx;
    int GGridColIdx;
    std::string MovableCstr;
    input >> InstName >> MasterCellName >> GGridRowIdx >> GGridColIdx >>
        MovableCstr;
    CellInsts.emplace_back(std::move(InstName), std::move(MasterCellName),
                           GGridRowIdx, GGridColIdx, MovableCstr == "Movable");
  }
  return CellInsts;
}

std::vector<Input::Raw::Net> Parser::parseNets(std::istream &input) {
  int NetCount = 0;
  input >> NetCount;
  std::vector<Input::Raw::Net> Nets;
  for (int i = 0; i < NetCount; ++i) {
    std::string buff;
    input >> buff;
    assert(buff == "Net");
    std::string NetName;
    int NumPins;
    std::string MinRoutingLayConstraint;
    double Weight;
    input >> NetName >> NumPins >> MinRoutingLayConstraint >> Weight;
    std::vector<Input::Raw::Net::Pin> Pins;
    for (int i = 0; i < NumPins; ++i) {
      input >> buff;
      assert(buff == "Pin");
      std::string tmp;
      input >> tmp;
      std::string InstName = "";
      std::string MasterPinName = "";
      for (int i = 0, flag = 0; tmp[i]; ++i) {
        if (flag)
          MasterPinName += tmp[i];
        else if (tmp[i] != '/')
          InstName += tmp[i];
        else
          flag = 1;
      }
      Pins.emplace_back(std::move(InstName), std::move(MasterPinName));
    }
    Nets.emplace_back(std::move(NetName), std::move(Pins),
                      std::move(MinRoutingLayConstraint), Weight * 100);
  }
  return Nets;
}

std::vector<Input::Raw::VoltageArea>
Parser::parseVoltageAreas(std::istream &input) {
  int NumVoltageAreas = 0;
  input >> NumVoltageAreas;
  std::vector<Input::Raw::VoltageArea> VoltageAreas;
  for (int i = 0; i < NumVoltageAreas; ++i) {
    std::string buff;
    input >> buff;
    assert(buff == "Name");
    std::string VoltageAreaName;
    input >> VoltageAreaName;
    input >> buff;
    assert(buff == "GGrids");
    int GGridCount = 0;
    input >> GGridCount;
    std::vector<Input::Raw::VoltageArea::GGrid> GGrids;
    for (int j = 0; j < GGridCount; ++j) {
      int GGridRowIdx = 0, GGridColIdx = 0;
      input >> GGridRowIdx >> GGridColIdx;
      GGrids.emplace_back(GGridRowIdx, GGridColIdx);
    }
    input >> buff;
    assert(buff == "Instances");
    int InstanceCount = 0;
    input >> InstanceCount;
    std::vector<Input::Raw::VoltageArea::Instance> Instances;
    for (int j = 0; j < InstanceCount; ++j) {
      std::string InstanceName;
      input >> InstanceName;
      Instances.emplace_back(std::move(InstanceName));
    }
    VoltageAreas.emplace_back(std::move(VoltageAreaName), std::move(GGrids),
                              std::move(Instances));
  }
  return VoltageAreas;
}

std::vector<Input::Raw::Route> Parser::parseRoutes(std::istream &input) {
  int RouteSegmentCount = 0;
  input >> RouteSegmentCount;
  std::vector<Input::Raw::Route> Routes;
  for (int i = 0; i < RouteSegmentCount; ++i) {
    int SRowIdx = 0, SColIdx = 0, SLayIdx = 0;
    int ERowIdx = 0, EColIdx = 0, ELayIdx = 0;
    std::string NetName;
    input >> SRowIdx >> SColIdx >> SLayIdx >> ERowIdx >> EColIdx >> ELayIdx >>
        NetName;
    Routes.emplace_back(SRowIdx, SColIdx, SLayIdx, ERowIdx, EColIdx, ELayIdx,
                        std::move(NetName));
  }
  return Routes;
}

std::unique_ptr<Input::Raw::Input> Parser::parse(std::istream &input) {
  unsigned MaxCellMove = 0;
  int RowBeginIdx = 0, ColBeginIdx = 0, RowEndIdx = 0, ColEndIdx = 0;
  std::vector<Input::Raw::Layer> Layers;
  std::vector<Input::Raw::NonDefaultSupplyGGrid> NonDefaultSupplyGGrids;
  std::vector<Input::Raw::MasterCell> MasterCells;
  std::vector<Input::Raw::CellInst> CellInsts;
  std::vector<Input::Raw::Net> Nets;
  std::vector<Input::Raw::VoltageArea> VoltageAreas;
  std::vector<Input::Raw::Route> Routes;
  std::string buff;
  while (input >> buff) {
    if (buff == "MaxCellMove")
      MaxCellMove = parseMaxCellMove(input);
    else if (buff == "GGridBoundaryIdx")
      std::tie(RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx) =
          parseGGridBoundaryIdx(input);
    else if (buff == "NumLayer")
      Layers = parseLayers(input);
    else if (buff == "NumNonDefaultSupplyGGrid")
      NonDefaultSupplyGGrids = parseNonDefaultSupplyGGrids(input);
    else if (buff == "NumMasterCell")
      MasterCells = parseMasterCells(input);
    else if (buff == "NumCellInst")
      CellInsts = parseCellInsts(input);
    else if (buff == "NumNets")
      Nets = parseNets(input);
    else if (buff == "NumVoltageAreas")
      VoltageAreas = parseVoltageAreas(input);
    else if (buff == "NumRoutes")
      Routes = parseRoutes(input);
    else
      assert(false && "Input Error");
  }

  return std::unique_ptr<Input::Raw::Input>(new Input::Raw::Input(
      MaxCellMove, RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx,
      std::move(Layers), std::move(NonDefaultSupplyGGrids),
      std::move(MasterCells), std::move(CellInsts), std::move(Nets),
      std::move(VoltageAreas), std::move(Routes)));
}
} // namespace cell_move_router