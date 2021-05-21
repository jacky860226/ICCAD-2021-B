#pragma once
#include "cell_move_router/Input/Processed/Datas.hpp"
#include <unordered_set>
namespace cell_move_router {
namespace Grid {
using CellGrid = std::unordered_set<const Input::Processed::CellInst *>;
} // namespace Grid
} // namespace cell_move_router