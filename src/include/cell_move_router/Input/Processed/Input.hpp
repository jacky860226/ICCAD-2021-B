#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/Input/Raw/Datas.hpp"
#include "cell_move_router/Input/Raw/Input.hpp"
#include <memory>
#include <ostream>
#include <vector>

namespace cell_move_router {
namespace Input {
namespace Processed {
class Input : Util::Outputable {
  std::unique_ptr<Raw::Input> RawInput;

  // const std::vector<MasterCell> MasterCells;
  // const std::vector<CellInst> CellInsts;
  // const std::vector<Net> Nets;
  // const std::vector<VoltageArea> VoltageAreas;
  // const std::vector<Route> Routes;

public:
  Input(std::unique_ptr<Raw::Input> &&RawInput);
  void to_ostream(std::ostream &out) const override;
  unsigned getMaxCellMove() const { return RawInput->getMaxCellMove(); }
  int getRowBeginIdx() const { return RawInput->getRowBeginIdx(); }
  int getColBeginIdx() const { return RawInput->getColBeginIdx(); }
  int getRowEndIdx() const { return RawInput->getRowEndIdx(); }
  int getColEndIdx() const { return RawInput->getColEndIdx(); }
  const std::vector<Raw::Layer> &getLayers() const {
    return RawInput->getLayers();
  }
  const std::vector<Raw::NonDefaultSupplyGGrid> &
  getNonDefaultSupplyGGrids() const {
    return RawInput->getNonDefaultSupplyGGrids();
  }
};
} // namespace Processed
} // namespace Input
} // namespace cell_move_router