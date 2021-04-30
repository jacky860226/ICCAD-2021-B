#include "cell_move_router/Grid/GridManager.hpp"
namespace cell_move_router {
namespace Grid {
GridManager::GridManager(const Input::Processed::Input *InputPtr)
    : InputPtr(InputPtr), Codec({InputPtr->getRowSize(), InputPtr->getColsize(),
                                 InputPtr->getLayers().size()}) {
  Grids.reserve(Codec.max());
  for (auto Layer : InputPtr->getLayers()) {
    for (size_t R = 0; R < Codec.at(0); ++R) {
      for (size_t C = 0; C < Codec.at(1); ++C) {
        auto Coordinate = Codec.encode({R, C, (unsigned)Layer.getIdx() - 1});
        Grids.emplace_back(Coordinate, Layer.getDefaultSupplyOfOneGGrid());
      }
    }
  }
  for (auto G : InputPtr->getNonDefaultSupplyGGrids()) {
    unsigned R = G.getRowIdx() - InputPtr->getRowBeginIdx();
    unsigned C = G.getColIdx() - InputPtr->getColBeginIdx();
    unsigned L = G.getLayIdx();
    auto Coordinate = Codec.encode({R, C, L});
    Grids[Coordinate].addCapacity(G.getIncrOrDecrValue());
  }
}
} // namespace Grid
} // namespace cell_move_router