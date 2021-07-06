#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
namespace cell_move_router {
namespace RegionCalculator {

class WeightedOptimalRegion : RegionCalculator {
  int weightElement(std::vector<std::pair<int, double>> &V, const int L,
                    const int R, double W);
  std::pair<int, int> getWeightedMedium(std::vector<std::pair<int, double>> &V);

public:
  WeightedOptimalRegion() {}
  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::Net *Net) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router