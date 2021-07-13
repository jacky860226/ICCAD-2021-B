#include "cell_move_router/RegionCalculator/FinalRegion.hpp"
namespace cell_move_router {
namespace RegionCalculator {

std::vector<int> FinalRegion::calSupply() {
  Supply.resize(Codec.max());
  for (int Row = InputPtr->getRowBeginIdx(); Row <= InputPtr->getRowEndIdx();
       ++Row) {
    for (int Col = InputPtr->getColBeginIdx(); Col <= InputPtr->getColEndIdx();
         ++Col) {
      unsigned long long R = Row - InputPtr->getRowBeginIdx();
      unsigned long long C = Col - InputPtr->getColBeginIdx();
      auto Coord = Codec.encode({R, C});
      Supply.at(Coord) = 0;
      for (int Lay = 1; Lay <= InputPtr->getLayers().size(); ++Lay) {
        Supply.at(Coord) += GridManagerPtr->getGrid(Row, Col, Lay).getSupply();
      }
    }
  }
}
// TODO


} // namespace RegionCalculator
} // namespace cell_move_router