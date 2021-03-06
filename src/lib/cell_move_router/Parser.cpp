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

// TODO: other parse function

std::unique_ptr<RawData::Input> Parser::parse(std::istream &input) {
  unsigned MaxCellMove = parseMaxCellMove(input);

  int RowBeginIdx = 0, ColBeginIdx = 0, RowEndIdx = 0, ColEndIdx = 0;
  std::tie(RowBeginIdx, ColBeginIdx, RowEndIdx, ColEndIdx) =
      parseGGridBoundaryIdx(input);

  // TODO: call other parse function
  std::vector<RawData::Layer> Layers;
  std::vector<RawData::NonDefaultSupplyGGrid> NonDefaultSupplyGGrids;
  std::vector<RawData::MasterCell> MasterCells;
  std::vector<RawData::CellInst> CellInsts;
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