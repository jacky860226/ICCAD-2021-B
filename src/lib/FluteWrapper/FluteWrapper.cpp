#include "FluteWrapper/FluteWrapper.hpp"
#include "Third/Flute3/flute.h"
#include <cassert>
#include <iostream>
#include <string>

namespace FluteWrapper {

struct FluteWrapper::FluteState {
  Flute::FluteState *State;
  FluteState(Flute::FluteState *State) : State(State) {}
  ~FluteState() { Flute::flute_free(State); }
};

FluteWrapper::FluteWrapper(const char *wirelength_vector_file,
                           const char *steiner_tree_file)
    : State(new FluteState(
          Flute::flute_init(wirelength_vector_file, steiner_tree_file))) {}

FluteWrapper::~FluteWrapper() { delete State; }

void FluteWrapper::exampleTest() {
  std::string FluteTablePath = "src/include/Third/Flute3/";
  std::string POWV9_path = FluteTablePath + FLUTE_POWVFILE;
  std::string PORT9_path = FluteTablePath + FLUTE_PORTFILE;
  FluteWrapper Wrapper(POWV9_path.c_str(), PORT9_path.c_str());

  std::vector<int> X = {2, 8, 6, 5, 3, 4};
  std::vector<int> Y = {3, 2, 6, 4, 5, 2};

  auto FluteTree = Wrapper.run(X, Y);

  std::cerr << "FLUTE wirelength = " << FluteTree.Length << '\n';
  std::cerr << "FLUTE degree = " << FluteTree.Degree << '\n';
  for (size_t i = 0; i < FluteTree.Branchs.size(); ++i)
    std::cerr << i << " - (" << FluteTree.Branchs[i].X << ','
              << FluteTree.Branchs[i].Y << ") .. "
              << FluteTree.Branchs[i].Neighbor << '\n';
}

Tree FluteWrapper::run(std::vector<int> &X, std::vector<int> &Y) {
  auto Size = X.size();
  assert(Size == Y.size() && Size > 0);
  auto FluteTree =
      Flute::flute(State->State, X.size(), X.data(), Y.data(), FLUTE_ACCURACY);
  std::vector<Branch> Branchs;
  auto BranchsSize = FluteTree.deg * 2 - 2;
  Branchs.reserve(BranchsSize);
  for (int i = 0; i < BranchsSize; ++i) {
    Branchs.emplace_back(FluteTree.branch[i].x, FluteTree.branch[i].y,
                         FluteTree.branch[i].n);
  }
  Flute::free_tree(State->State, FluteTree);
  return Tree(std::move(Branchs), FluteTree.length, FluteTree.deg);
}

} // namespace FluteWrapper