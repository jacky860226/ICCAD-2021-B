#pragma once
#include <vector>

namespace FluteWrapper {

struct Branch {
  int X;
  int Y;
  int Neighbor;
  Branch(int X, int Y, int Neighbor) : X(X), Y(Y), Neighbor(Neighbor) {}
};

struct Tree {
  const std::vector<Branch> Branchs;
  const int Length;
  const int Degree;
  Tree(std::vector<Branch> &&Branchs, int Length, int Degree)
      : Branchs(std::move(Branchs)), Length(Length), Degree(Degree) {}
  Tree(Tree &&o)
      : Branchs(std::move(o.Branchs)), Length(o.Length), Degree(o.Degree) {}
};

class FluteWrapper {
  class FluteState;
  FluteState *State;

public:
  FluteWrapper(const char *wirelength_vector_file,
               const char *steiner_tree_file);
  ~FluteWrapper();
  static void exampleTest();
  Tree run(std::vector<int> &X, std::vector<int> &Y);
};

} // namespace FluteWrapper