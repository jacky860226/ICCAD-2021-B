#pragma once
#include "cell_move_router/Grid/GridManager.hpp"

namespace cell_move_router {
namespace Router {
class RouterBase {
  Grid::GridManager *GridManagerPtr;
  const std::vector<double> LayerFactor;
  const std::vector<char> LayerDir;

protected:
  std::vector<double> buildLayerFactor(Grid::GridManager *GridManagerPtr) {
    auto Layers = GridManagerPtr->getInputPtr()->getLayers();
    std::vector<double> Ret(Layers.size() + 1);
    for (const auto &Layer : Layers) {
      Ret.at(Layer.getIdx()) = Layer.getPowerFactor();
    }
    return Ret;
  }

  std::vector<char> buildLayerDir(Grid::GridManager *GridManagerPtr) {
    auto Layers = GridManagerPtr->getInputPtr()->getLayers();
    std::vector<char> Ret(Layers.size() + 1);
    for (const auto &Layer : Layers) {
      Ret.at(Layer.getIdx()) = Layer.getRoutingDir();
    }
    return Ret;
  }

public:
  RouterBase(Grid::GridManager *GridManagerPtr)
      : GridManagerPtr(GridManagerPtr),
        LayerFactor(std::move(buildLayerFactor(GridManagerPtr))),
        LayerDir(std::move(buildLayerDir(GridManagerPtr))) {}

  Grid::GridManager *getGridManager() { return GridManagerPtr; }
  const Grid::GridManager *getGridManager() const { return GridManagerPtr; }
  const std::vector<double> &getLayerFactor() const { return LayerFactor; }
  const std::vector<char> &getLayerDir() const { return LayerDir; }

  virtual std::vector<Input::Processed::Route>
  singleNetRoute(const Input::Processed::Net *Net,
                 const std::vector<Input::Processed::Route> &OriginRoute) = 0;
  virtual void rerouteAll() = 0;
};
} // namespace Router
} // namespace cell_move_router