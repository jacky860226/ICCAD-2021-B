#include <cell_move_router/Input/Processed/Datas.hpp>
#include <tuple>
#include <utility>
namespace cell_move_router {
namespace RegionCalculator {

class RegionCalculator {
private:
  int kth_element(std::vector<int> &, const int, const int, const int);
  std::pair<int, int> getMedium(std::vector<int> &);

public:
  RegionCalculator() {}
  // {BeginRowIdx, EndRowIdx, BeginColIdx, EndColIdx}
  std::tuple<int, int, int, int> getRegion(const Input::Processed::Net *);
};

} // namespace RegionCalculator
} // namespace cell_move_router