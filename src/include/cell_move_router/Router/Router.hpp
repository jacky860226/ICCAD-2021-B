#pragma once
#include "Third/SteinerTree2/Solver.hpp"
#include "cell_move_router/Grid/GridManager.hpp"

namespace cell_move_router {
namespace Router {
class Router {
  Grid::GridManager *GridManagerPtr;

  const std::vector<double> LayerFactor;

  static bool netCmp(const Input::Processed::Net &,
                     const Input::Processed::Net &);

  void localRoute(const Input::Processed::Net *);

  std::vector<double> buildLayerFactor() {
    auto Layers = GridManagerPtr->getInputPtr()->getLayers();
    std::vector<double> Ret(Layers.size() + 1);
    for (const auto &Layer : Layers)
      Ret.at(Layer.getIdx()) = Layer.getPowerFactor();
    return LayerFactor;
  }

public:
  Router(Grid::GridManager *GridManagerPtr)
      : GridManagerPtr(GridManagerPtr), LayerFactor(buildLayerFactor()) {}

  void route();
};
} // namespace Router
} // namespace cell_move_router