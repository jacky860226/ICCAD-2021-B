#include "cell_move_router/Input/Processed/Input.hpp"
#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
namespace cell_move_router {
namespace RegionCalculator {

class OptimalRegion : RegionCalculator {
protected:
  const Input::Processed::Input *InputPtr;

public:
  OptimalRegion(const Input::Processed::Input *InputPtr) : InputPtr(InputPtr) {}
  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::CellInst *Cell) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router