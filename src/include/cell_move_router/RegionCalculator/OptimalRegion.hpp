#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
namespace cell_move_router {
namespace RegionCalculator {

class OptimalRegion : RegionCalculator {
  std::pair<int, int> getMedium(std::vector<int> &V);

public:
  OptimalRegion() {}
  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::Net *Net) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router