#pragma once
#include "Util/BaseType.hpp"
#include <ostream>
namespace cell_move_router {
namespace RawData {
// TODO: implement the raw datas
class Layer : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class NonDefaultSupplyGGrid : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class MasterCell : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class CellInst : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class Net : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class VoltageArea : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
class Route : Util::Outputable {
public:
  void to_ostream(std::ostream &out) const override;
};
} // namespace RawData
} // namespace cell_move_router