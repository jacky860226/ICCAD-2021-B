#include "cell_move_router/RawData/Datas.hpp"
namespace cell_move_router {
namespace RawData {
void Layer::to_ostream(std::ostream &out) const {}
void NonDefaultSupplyGGrid::to_ostream(std::ostream &out) const {}
void MasterCell::to_ostream(std::ostream &out) const {}
void CellInst::to_ostream(std::ostream &out) const {}
void Net::to_ostream(std::ostream &out) const {}
void VoltageArea::to_ostream(std::ostream &out) const {}
void Route::to_ostream(std::ostream &out) const {}
} // namespace RawData
} // namespace cell_move_router