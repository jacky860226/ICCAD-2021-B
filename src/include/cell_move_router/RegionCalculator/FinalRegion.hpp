#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
#include <tuple>
#include <vector>

namespace cell_move_router {
namespace RegionCalculator {

class FinalRegion : public RegionCalculator {
  const Grid::GridManager *GridManagerPtr;
  const Input::Processed::Input *InputPtr;
  const CoordinateCodec<unsigned long long> Codec;
  std::vector<int> Supply;
  std::vector<int> calSupply();

public:
  FinalRegion(const Grid::GridManager *GridManagerPtr)
      : GridManagerPtr(GridManagerPtr), InputPtr(GridManagerPtr->getInputPtr()),
        Codec({InputPtr->getRowSize(), InputPtr->getColsize()}) {
    calSupply();
  }

  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::Net *Net) override;

  void calFinalRegion();
};

} // namespace RegionCalculator
} // namespace cell_move_router