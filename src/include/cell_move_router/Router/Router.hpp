#pragma once
#include "Third/SteinerTree2/Solver.hpp"
#include "cell_move_router/Grid/GridManager.hpp"

namespace cell_move_router {
namespace Router {
class Router {
  Grid::GridManager *GridManagerPtr;

  const std::vector<double> LayerFactor;

  const std::vector<char> LayerDir;

  static bool netCmp(const Input::Processed::Net *,
                     const Input::Processed::Net *);

  void localRoute(const Input::Processed::Net *);

  std::vector<double> buildLayerFactor(Grid::GridManager *GridManagerPtr) {
    auto Layers = GridManagerPtr->getInputPtr()->getLayers();
    std::vector<double> Ret(Layers.size() + 1);
    for (const auto &Layer : Layers) {
      assert(Layer.getIdx() < Ret.size());
      Ret.at(Layer.getIdx()) = Layer.getPowerFactor();
    }
    return Ret;
  }

  std::vector<char> buildLayerDir(Grid::GridManager *GridManagerPtr) {
    auto Layers = GridManagerPtr->getInputPtr()->getLayers();
    std::vector<char> Ret(Layers.size() + 1);
    for (const auto &Layer : Layers) {
      assert(Layer.getIdx() < Ret.size());
      Ret.at(Layer.getIdx()) = Layer.getRoutingDir();
    }
    return Ret;
  }

public:
  Router(Grid::GridManager *GridManagerPtr)
      : GridManagerPtr(GridManagerPtr),
        LayerFactor(buildLayerFactor(GridManagerPtr)),
        LayerDir(buildLayerDir(GridManagerPtr)) {}

  void route();
};
} // namespace Router
} // namespace cell_move_router