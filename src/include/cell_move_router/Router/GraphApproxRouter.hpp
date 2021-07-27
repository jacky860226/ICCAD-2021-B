#pragma once
#include "Third/SteinerTree2/Solver.hpp"
#include "cell_move_router/Router/BondaryBuilder.hpp"
#include "cell_move_router/Router/RouterBase.hpp"

namespace cell_move_router {
namespace Router {

class RoutingGraphManager {
  steiner_tree::UndirectedGraph<long long> G;
  Grid::GridManager *GridManager;
  const RoutingBondaryInfo *BondaryInfo;

public:
  RoutingGraphManager(Grid::GridManager *GridManager)
      : GridManager(GridManager){};
  void setBondaryInfo(const RoutingBondaryInfo *BI) { BondaryInfo = BI; }
  const steiner_tree::UndirectedGraph<long long> &getGraph() const { return G; }
  void createGraph(const std::vector<long long> &LayerFactor,
                   const std::vector<char> &LayerDir);
  std::vector<Input::Processed::Route>
  createFinalRoute(const std::vector<size_t> &Eids, const Input::Processed::Net *Net);
};

class GraphApproxRouter : public RouterBase {
  RoutingGraphManager RGM;

public:
  GraphApproxRouter(Grid::GridManager *GridManagerPtr)
      : RouterBase(GridManagerPtr), RGM(GridManagerPtr) {}

  std::pair<std::vector<Input::Processed::Route>, bool> singleNetRoute(
      const Input::Processed::Net *Net,
      const std::vector<Input::Processed::Route> &OriginRoute) override;
  void rerouteAll() override;
};

} // namespace Router
} // namespace cell_move_router