#pragma once
#include "cell_move_router/Input/Raw/Datas.hpp"
#include "cell_move_router/Input/Raw/Input.hpp"
#include <istream>
#include <memory>
#include <tuple>
#include <vector>

namespace cell_move_router {
class Parser {
  unsigned parseMaxCellMove(std::istream &input);
  // <rowBeginIdx> <colBeginIdx> <rowEndIdx> <colEndIdx>
  std::tuple<int, int, int, int> parseGGridBoundaryIdx(std::istream &input);
  std::vector<Input::Raw::Layer> parseLayers(std::istream &input);
  std::vector<Input::Raw::NonDefaultSupplyGGrid>
  parseNonDefaultSupplyGGrids(std::istream &input);
  std::vector<Input::Raw::MasterCell> parseMasterCells(std::istream &input);
  std::vector<Input::Raw::CellInst> parseCellInsts(std::istream &input);
  std::vector<Input::Raw::Net> parseNets(std::istream &input);
  std::vector<Input::Raw::VoltageArea> parseVoltageAreas(std::istream &input);
  std::vector<Input::Raw::Route> parseRoutes(std::istream &input);

public:
  std::unique_ptr<Input::Raw::Input> parse(std::istream &input);
};
} // namespace cell_move_router