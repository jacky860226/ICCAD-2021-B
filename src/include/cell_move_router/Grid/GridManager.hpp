#pragma once
#include "cell_move_router/CoordinateCodec.hpp"
#include "cell_move_router/Grid/CellGrid.hpp"
#include "cell_move_router/Grid/Grid.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
namespace cell_move_router {
namespace Grid {
class GridManager {
private:
  const Input::Processed::Input *InputPtr;
  const CoordinateCodec<unsigned long long> Codec;
  std::vector<Grid> Grids;
  std::vector<CellGrid> CellGrids;
  std::unordered_map<const Input::Processed::CellInst *, unsigned long long>
      CellCoordinate;
  std::unordered_map<const Input::Processed::Net *,
                     std::pair<std::vector<Input::Processed::Route>, unsigned>>
      NetRoutes;
  std::unordered_map<const Input::Processed::CellInst *,
                     std::unordered_set<unsigned long long>>
      CellVoltageArea;

  unsigned long long coordinateTrans(int R, int C, int L);
  std::tuple<int, int, int> coordinateInv(unsigned long long Coordinate);

public:
  GridManager(const Input::Processed::Input *InputPtr);
  void addNet(const Input::Processed::Net *Net,
              std::vector<Input::Processed::Route> &&Routes);
  void removeNet(const Input::Processed::Net *Net);
  void addCell(const Input::Processed::CellInst *CellInst, const int Row,
               const int Col);
  void removeCell(const Input::Processed::CellInst *CellInst);
};
} // namespace Grid
} // namespace cell_move_router