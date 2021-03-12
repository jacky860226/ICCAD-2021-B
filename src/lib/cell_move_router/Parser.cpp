#include "cell_move_router/Parser.hpp"
#include <cassert>

namespace cell_move_router {
unsigned Parser::parseMaxCellMove(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "MaxCellMove");

  unsigned MaxCellMove = 0;
  input >> MaxCellMove;
  return MaxCellMove;
}
std::tuple<int, int, int, int>
Parser::parseGGridBoundaryIdx(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "GGridBoundaryIdx");

  int RowBeginIdx = 0, ColBeginIdx = 0, RowEndIdx = 0, ColEndIdx = 0;
  input >> RowBeginIdx >> ColBeginIdx >> RowEndIdx >> ColEndIdx;
  return std::make_tuple(RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx);
}

std::vector<RawData::Layer> Parser::parseLayers(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "NumLayer");
  int LayerCount = 0;
  input >> LayerCount;
  std::vector<RawData::Layer> Layers;
  for (int i = 0; i < LayerCount; i++) {
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
                        DefaultSupplyOfOneGGrid, PowerFactor);
  }
  return Layers;
}

std::vector<RawData::NonDefaultSupplyGGrid>
Parser::parseNonDefaultSupplyGGrids(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "NumNonDefaultSupplyGGrid");
  int NumNonDefaultSupplyGGrid = 0;
  input >> NumNonDefaultSupplyGGrid;
  std::vector<RawData::NonDefaultSupplyGGrid> NonDefaultSupplyGGrids;
  for (int i = 0; i < NumNonDefaultSupplyGGrid; i++) {
    int RowIdx, ColIdx, LayIdx;
    int IncrOrDecrValue;
    input >> RowIdx >> ColIdx >> LayIdx >> IncrOrDecrValue;
    NonDefaultSupplyGGrids.emplace_back(RowIdx, ColIdx, LayIdx,
                                        IncrOrDecrValue);
  }
  return NonDefaultSupplyGGrids;
}

RawData::MasterCell::Pin parsePin(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "Pin");
  std::string PinName;
  std::string PinLayer;
  input >> PinName >> PinLayer;
  return RawData::MasterCell::Pin(std::move(PinName), std::move(PinLayer));
}

RawData::MasterCell::Blkg parseBlkg(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "Blkg");
  std::string BlockageName;
  std::string BlockageLayer;
  int Demand;
  input >> BlockageName >> BlockageLayer >> Demand;
  return RawData::MasterCell::Blkg(std::move(BlockageName),
                                   std::move(BlockageLayer), Demand);
}

std::vector<RawData::MasterCell> Parser::parseMasterCells(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "NumMasterCell");
  int NumMasterCell = 0;
  input >> NumMasterCell;
  std::vector<RawData::MasterCell> MasterCells;
  for (int i = 0; i < NumMasterCell; i++) {
    input >> buff;
    assert(buff == "MasterCell");
    std::string MasterCellName;
    int PinCount = 0;
    int BlkgCount = 0;
    input >> MasterCellName >> PinCount >> BlkgCount;
    std::vector<RawData::MasterCell::Pin> Pins;
    std::vector<RawData::MasterCell::Blkg> Blkgs;
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

std::vector<RawData::CellInst> Parser::parseCellInsts(std::istream &input) {
  std::string buff;
  input >> buff;
  assert(buff == "NumCellInst");
  int CellInstCount;
  input >> CellInstCount;
  std::vector<RawData::CellInst> CellInsts;
  for (int i = 0; i < CellInstCount; i++) {
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

// TODO: other parse function

std::unique_ptr<RawData::Input> Parser::parse(std::istream &input) {
  unsigned MaxCellMove = parseMaxCellMove(input);

  int RowBeginIdx = 0, ColBeginIdx = 0, RowEndIdx = 0, ColEndIdx = 0;
  std::tie(RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx) =
      parseGGridBoundaryIdx(input);

  std::vector<RawData::Layer> Layers = parseLayers(input);

  std::vector<RawData::NonDefaultSupplyGGrid> NonDefaultSupplyGGrids =
      parseNonDefaultSupplyGGrids(input);

  std::vector<RawData::MasterCell> MasterCells = parseMasterCells(input);

  std::vector<RawData::CellInst> CellInsts = parseCellInsts(input);

  std::vector<RawData::Net> Nets;
  std::vector<RawData::VoltageArea> VoltageAreas;
  std::vector<RawData::Route> Routes;

  return std::unique_ptr<RawData::Input>(new RawData::Input(
      MaxCellMove, RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx,
      std::move(Layers), std::move(NonDefaultSupplyGGrids),
      std::move(MasterCells), std::move(CellInsts), std::move(Nets),
      std::move(VoltageAreas), std::move(Routes)));
}
} // namespace cell_move_router