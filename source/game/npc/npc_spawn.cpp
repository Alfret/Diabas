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
    world.GetEntityManager()
      .GetRegistry()
      .get<Moveable>(npc->GetEntity())
      .position = position;
  } else {
    DLOG_ERROR("failed to find npc after adding");
    return;
  }

  auto& network = world.GetNetwork();
  auto& packet_handler = network.GetPacketHandler();
  Packet packet{};
  packet_handler.BuildPacketHeader(packet, PacketHeaderStaticTypes::kNpcSpawn);
  auto mw = packet.GetMemoryWriter();
  const u32 size = 1;
  mw->Write(size);
  mw->Write(type);
  mw->Write(id);
  npc->Store(world.GetEntityManager(), *(*mw));
  mw.Finalize();
  network.PacketBroadcast(packet);
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
}

template<>
void
NpcSpawn<Side::kServer>(World& world, String type_name, Position position)
{
  const auto it = world.GetNpcRegistry().GetNpcTypeNames().find(type_name);
  AlfAssert(it != world.GetNpcRegistry().GetNpcTypeNames().end(),
            "could not find npc type with name {}",
            type_name);
  NpcSpawn<Side::kServer>(world, it->second, position);
}

template<>
void
NpcSpawn<Side::kClient>(World& world, String type_name, Position position)
{
  const auto it = world.GetNpcRegistry().GetNpcTypeNames().find(type_name);
  AlfAssert(it != world.GetNpcRegistry().GetNpcTypeNames().end(),
            "could not find npc type with name {}",
            type_name);
  NpcSpawn<Side::kClient>(world, it->second, position);
}
}
