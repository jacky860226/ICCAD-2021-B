#pragma once
#include "Third/SteinerTree2/Solver.hpp"
#include "cell_move_router/Router/RouterBase.hpp"

namespace cell_move_router {
namespace Router {

class RoutingGraphManager {
  steiner_tree::UndirectedGraph<long long> G;
  cell_move_router::CoordinateCodec<unsigned long long> Codec;
  Grid::GridManager *GridManager;
  const Input::Processed::Net *Net;
  std::unordered_map<size_t, int> PinMinLayer;
  std::unordered_set<size_t> Terminals;
  int MinLayerIdx, MaxLayerIdx;
  int MaxR, MinR;
  int MaxC, MinC;

public:
  RoutingGraphManager() : Codec({0, 0, 0}){};
  const steiner_tree::UndirectedGraph<long long> &getGraph() const { return G; }
  const std::unordered_set<size_t> &getTerminals() const { return Terminals; }
  void setGraphInfo(Grid::GridManager *_GridManager,
                    const Input::Processed::Net *_Net,
                    const std::vector<Input::Processed::Route> &OriginRoute);
  std::vector<Input::Processed::Route> createTerminalsAndRouteUnderMinLayer();
  void createGraph(const std::vector<long long> &LayerFactor,
                   const std::vector<char> &LayerDir);
  std::vector<Input::Processed::Route>
  createFinalRoute(const std::vector<size_t> &Eids,
                   std::vector<Input::Processed::Route> &&RouteUnderMinLayer);
};

class GraphApproxRouter : public RouterBase {
  RoutingGraphManager RGM;

public:
  GraphApproxRouter(Grid::GridManager *GridManagerPtr)
      : RouterBase(GridManagerPtr) {}

  std::pair<std::vector<Input::Processed::Route>, bool> singleNetRoute(
      const Input::Processed::Net *Net,
      const std::vector<Input::Processed::Route> &OriginRoute) override;
  void rerouteAll() override;
};

} // namespace Router
} // namespace cell_move_router