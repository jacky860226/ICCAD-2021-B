#include "cell_move_router/Solver.hpp"
#include "cell_move_router/Router/GraphApproxRouter.hpp"

namespace cell_move_router {
void Solver::solve() {
  cell_move_router::Router::GraphApproxRouter Router(&GridManager);
  Router.rerouteAll();
}
} // namespace cell_move_router