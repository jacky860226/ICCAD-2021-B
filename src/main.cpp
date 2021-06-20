#include "GlobalTimer.hpp"
#include "cell_move_router/Grid/GridManager.hpp"
#include "cell_move_router/IOStreamCreator.hpp"
#include "cell_move_router/Input/Processed/Input.hpp"
#include "cell_move_router/Parser.hpp"
#include "cell_move_router/Router/GraphApproxRouter.hpp"
#include <iostream>
#include <istream>

int main(int argc, char **argv) {
  GlobalTimer::initialTimerAndSetTimeLimit(std::chrono::seconds(55 * 60));

  cell_move_router::Parser Parser;
  auto InputStreamPtr =
      cell_move_router::InputStreamCreator().createInputStream(argc, argv);
  auto Input = Parser.parse(*InputStreamPtr);
  InputStreamPtr = nullptr;

  auto ProcessedInput =
      cell_move_router::Input::Processed::Input::createInput(std::move(Input));

  // if (ProcessedInput != nullptr) { // This is for test
  //   auto OutputStreamPtr =
  //       cell_move_router::OutputStreamCreator().createOutputStream(argc,
  //       argv);
  //   ProcessedInput->to_ostream(*OutputStreamPtr);
  // }

  cell_move_router::Grid::GridManager GridManager(ProcessedInput.get());

  cell_move_router::Router::GraphApproxRouter Router(&GridManager);
  Router.rerouteAll();

  auto OutputStreamPtr =
      cell_move_router::OutputStreamCreator().createOutputStream(argc, argv);
  GridManager.output(*OutputStreamPtr);

  auto Timer = GlobalTimer::getInstance();
  // std::cerr << Timer->getDuration<>().count() / 1e9 << " seconds\n";
  if (Timer->overTime()) {
    std::cerr << Timer->getDuration<>().count() / 1e9 << " seconds\n";
    std::cerr << "overtime!!\n";
  }
  return 0;
}