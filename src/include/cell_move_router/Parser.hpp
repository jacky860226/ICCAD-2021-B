#pragma once
#include "cell_move_router/RawData/Datas.hpp"
#include "cell_move_router/RawData/Input.hpp"
#include <istream>
#include <memory>
#include <tuple>
#include <vector>

namespace cell_move_router {
class Parser {
  unsigned parseMaxCellMove(std::istream &input);
  // <rowBeginIdx> <colBeginIdx> <rowEndIdx> <colEndIdx>
  std::tuple<int, int, int, int> parseGGridBoundaryIdx(std::istream &input);
  std::vector<RawData::Layer> parseLayers(std::istream &input);
  std::vector<RawData::NonDefaultSupplyGGrid>
  parseNonDefaultSupplyGGrids(std::istream &input);
  std::vector<RawData::MasterCell> parseMasterCells(std::istream &input);
  std::vector<RawData::CellInst> parseCellInsts(std::istream &input);
  std::vector<RawData::Net> parseNets(std::istream &input);
  std::vector<RawData::VoltageArea> parseVoltageAreas(std::istream &input);
  std::vector<RawData::Route> parseRoutes(std::istream &input);

public:
  std::unique_ptr<RawData::Input> parse(std::istream &input);
};
} // namespace cell_move_router