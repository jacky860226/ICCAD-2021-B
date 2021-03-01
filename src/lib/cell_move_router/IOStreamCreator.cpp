#include "cell_move_router/IOStreamCreator.hpp"
#include <fstream>

namespace cell_move_router {
void IstreamDeletor::operator()(std::istream *IstreamPtr) const {
  if (IstreamPtr && IstreamPtr != &std::cin) {
    delete IstreamPtr;
  }
}
void OstreamDeletor::operator()(std::ostream *OstreamPtr) const {
  if (OstreamPtr && OstreamPtr != &std::cout) {
    delete OstreamPtr;
  }
}

std::unique_ptr<std::istream, IstreamDeletor>
InputStreamCreator::createInputStream(int argc, char **argv) const {
  if (argc > 1) {
    return std::unique_ptr<std::istream, IstreamDeletor>(
        new std::ifstream(argv[1]), IstreamDeletor());
  } else {
    return std::unique_ptr<std::istream, IstreamDeletor>(&std::cin,
                                                         IstreamDeletor());
  }
}

std::unique_ptr<std::ostream, OstreamDeletor>
OutputStreamCreator::createOutputStream(int argc, char **argv) const {
  if (argc > 2) {
    return std::unique_ptr<std::ostream, OstreamDeletor>(
        new std::ofstream(argv[2]), OstreamDeletor());
  } else {
    return std::unique_ptr<std::ostream, OstreamDeletor>(&std::cout,
                                                         OstreamDeletor());
  }
}
} // namespace cell_move_router