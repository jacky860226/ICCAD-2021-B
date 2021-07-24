#include "GlobalTimer.hpp"
#include "cell_move_router/IOStreamCreator.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include "cell_move_router/Parser.hpp"
#include "cell_move_router/Solver.hpp"
#include <iostream>

namespace {
std::unique_ptr<cell_move_router::Input::Processed::Input>
readInput(int argc, char **argv) {
  cell_move_router::Parser Parser;
  auto InputStreamPtr =
      cell_move_router::InputStreamCreator().createInputStream(argc, argv);
  auto Input = Parser.parse(*InputStreamPtr);
  return cell_move_router::Input::Processed::Input::createInput(
      std::move(Input));
}

void writeOutput(cell_move_router::Solver &Solver, int argc, char **argv) {
  auto OutputStreamPtr =
      cell_move_router::OutputStreamCreator().createOutputStream(argc, argv);
  Solver.getGridManager().to_ostream(*OutputStreamPtr);
}
} // namespace

int main(int argc, char **argv) {
  GlobalTimer::initialTimerAndSetTimeLimit(std::chrono::seconds(55 * 60));

  auto Input = readInput(argc, argv);

  cell_move_router::Solver Solver(Input.get());
  Solver.solve();

  writeOutput(Solver, argc, argv);

  auto Timer = GlobalTimer::getInstance();
  std::cerr << Timer->getDuration<>().count() / 1e9 << " seconds\n";
  if (Timer->overTime()) {
    std::cerr << "overtime!!\n";
  }
  return 0;
}