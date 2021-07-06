#include "cell_move_router/RegionCalculator/RegionCalculator.hpp"
namespace cell_move_router {
namespace RegionCalculator {

class OptimalRegion : RegionCalculator {
  int weightElement(std::vector<std::pair<int, double>> &V, const int L,
                    const int R, double W);
  std::pair<int, int> getMedium(std::vector<int> &V);
  std::pair<int, int> getWeightedMedium(std::vector<std::pair<int, double>> &V);

public:
  OptimalRegion() {}
  std::tuple<int, int, int, int>
  getRegion(const Input::Processed::Net *Net) override;
  std::tuple<int, int, int, int>
  getWeightedRegion(const Input::Processed::Net *Net) override;
};

} // namespace RegionCalculator
} // namespace cell_move_router