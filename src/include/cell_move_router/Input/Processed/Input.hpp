#pragma once
#include "Util/BaseType.hpp"
#include "cell_move_router/Input/Processed/Datas.hpp"
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

  const std::unordered_map<std::string, const Raw::Layer *> LayerMap;

  const std::vector<MasterCell> MasterCells;
  const std::unordered_map<std::string, const MasterCell *> MasterCellMap;

  const std::vector<CellInst> CellInsts;
  const std::unordered_map<std::string, const CellInst *> CellInstMap;

  const std::vector<Net> Nets;
  const std::unordered_map<std::string, const Net *> NetMap;

  const std::vector<VoltageArea> VoltageAreas;
  const std::vector<Route> Routes;

  const std::unordered_map<const CellInst *, std::vector<const Net *>>
      RelativeNetsMap;

  std::unordered_map<std::string, const Raw::Layer *> CreateLayerMap();
  std::unordered_map<std::string, const MasterCell *> CreateMasterCellMap();
  std::unordered_map<std::string, const CellInst *> CreateCellInstMap();
  std::unordered_map<std::string, const Net *> CreateNetMap();

  std::vector<MasterCell> CreateMasterCells();
  std::vector<CellInst> CreateCellInsts();
  std::vector<Net> CreateNets();
  std::vector<VoltageArea> CreateVoltageAreas();
  std::vector<Route> CreateRoutes();

  std::unordered_map<const CellInst *, std::vector<const Net *>>
  CreateRelativeNetsMap();

public:
  Input(std::unique_ptr<Raw::Input> &&RawInput);
  static std::unique_ptr<Input>
  createInput(std::unique_ptr<Raw::Input> &&RawInput) {
    return std::unique_ptr<Input>(new Input(std::move(RawInput)));
  }
  void to_ostream(std::ostream &out) const override;
  unsigned getMaxCellMove() const { return RawInput->getMaxCellMove(); }
  int getRowBeginIdx() const { return RawInput->getRowBeginIdx(); }
  int getColBeginIdx() const { return RawInput->getColBeginIdx(); }
  int getRowEndIdx() const { return RawInput->getRowEndIdx(); }
  int getColEndIdx() const { return RawInput->getColEndIdx(); }
  unsigned int getRowSize() const {
    return getRowEndIdx() - getRowBeginIdx() + 1;
  }
  unsigned int getColsize() const {
    return getColEndIdx() - getColBeginIdx() + 1;
  }
  const std::vector<Raw::Layer> &getLayers() const {
    return RawInput->getLayers();
  }
  const std::vector<Raw::NonDefaultSupplyGGrid> &
  getNonDefaultSupplyGGrids() const {
    return RawInput->getNonDefaultSupplyGGrids();
  }
  const std::vector<MasterCell> &getMasterCells() const { return MasterCells; }
  const std::vector<CellInst> &getCellInsts() const { return CellInsts; }
  const std::vector<Net> &getNets() const { return Nets; }
  const std::vector<VoltageArea> &getVoltageAreas() const {
    return VoltageAreas;
  }
  const std::vector<Route> &getRoutes() const { return Routes; }
  const std::unordered_map<const CellInst *, std::vector<const Net *>> &
  getRelativeNetsMap() const {
    return RelativeNetsMap;
  }
};
} // namespace Processed
} // namespace Input
} // namespace cell_move_router