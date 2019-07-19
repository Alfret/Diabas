#include "npc_spawn.hpp"
#include "game/world.hpp"
#include <dlog.hpp>

namespace dib::game {

template<>
void
NpcSpawn<Side::kServer>(World& world,
                        const NpcType type,
                        const Position position)
{
  auto& npc_registry = world.GetNpcRegistry();
  NpcID id = NpcIDGenerateUnique();
  npc_registry.Add(world.GetEntityManager(), id, type);
  Npc* npc = npc_registry.Get(id);
  if (npc != nullptr) {
    world.GetEntityManager().GetRegistry().get<Moveable>(npc->GetEntity()).position = position;
    DLOG_VERBOSE("spawned a npc");
  } else {
    DLOG_ERROR("failed to find npc after adding");
    return;
  }

  auto& network = world.GetNetwork();
  auto& packet_handler = network.GetPacketHandler();
  Packet packet{};
  packet_handler.BuildPacketHeader(packet, PacketHeaderStaticTypes::kNpcSpawn);
  auto mw = packet.GetMemoryWriter();
  mw->Write(type);
  mw->Write(id);
  npc->Store(world.GetEntityManager(), *(*mw));
  mw.Finalize();
  network.PacketBroadcast(packet);
  DLOG_VERBOSE("broadcasted spawned npc");
}

template<>
void
NpcSpawn<Side::kClient>(World& world,
                        const NpcType type,
                        const Position position)
{
  auto& network = world.GetNetwork();
  auto& packet_handler = network.GetPacketHandler();

  Packet packet{};
  packet_handler.BuildPacketHeader(packet, PacketHeaderStaticTypes::kNpcSpawn);
  auto mw = packet.GetMemoryWriter();
  mw->Write(type);
  mw->Write(position.x);
  mw->Write(position.y);
  mw.Finalize();
  network.PacketBroadcast(packet);
  DLOG_VERBOSE("sent spawn request");
}

}
