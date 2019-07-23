#include "npc_id.hpp"

namespace dib::game {

NpcID
NpcIDGenerateUnique()
{
  static NpcID id = 0;
  return id++;
}

}
