#include "cell_move_router/Solver.hpp"
#include "cell_move_router/Router/GraphApproxRouter.hpp"
#include <iostream>

namespace cell_move_router {
void Solver::solve() {
  cell_move_router::Router::GraphApproxRouter Router(&GridManager);
  long long OriginalCost = GridManager.getCurrentCost();
  std::cerr << "Original:\n";
  std::cerr << "  CurrentCost: " << OriginalCost * 0.0001 << '\n';
  Router.rerouteAll();
  long long FirstRerouteCost = GridManager.getCurrentCost();
  std::cerr << "After First re-route:\n";
  std::cerr << "  CurrentCost: " << FirstRerouteCost * 0.0001 << '\n';
  std::cerr << "  ReducedCost: " << (OriginalCost - FirstRerouteCost) * 0.0001
            << '\n';
}
} // namespace cell_move_router