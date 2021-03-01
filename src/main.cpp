#include "GlobalTimer.hpp"
#include "cell_move_router/IOStreamCreator.hpp"
#include "cell_move_router/Parser.hpp"
#include <iostream>
#include <istream>

int main(int argc, char **argv) {
  GlobalTimer::initialTimerAndSetTimeLimit(std::chrono::seconds(55 * 60));

  cell_move_router::Parser Parser;
  auto InputStreamPtr =
      cell_move_router::InputStreamCreator().createInputStream(argc, argv);
  auto Input = Parser.parse(*InputStreamPtr);
  InputStreamPtr = nullptr;

  if (Input != nullptr) { // This is for test
    auto OutputStreamPtr =
        cell_move_router::OutputStreamCreator().createOutputStream(argc, argv);
    Input->to_ostream(*OutputStreamPtr);
  }

  auto Timer = GlobalTimer::getInstance();
  std::cerr << Timer->getDuration<>().count() / 1e9 << " seconds\n";
  if (Timer->overTime()) {
    std::cerr << "overtime!!\n";
  }
  return 0;
}