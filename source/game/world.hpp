#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "core/assert.hpp"
#include "network/client.hpp"
#include "network/common.hpp"
#include "network/server.hpp"
#include "network/side.hpp"
#include <memory>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace dib {

// ============================================================ //
// class Declaration
// ============================================================ //

template<Side side>
class World
{

  // ============================================================ //
  // Lifetime
  // ============================================================ //

public:
  World();

  ~World();

  // ============================================================ //
  // Core Methods
  // ============================================================ //

public:
  void Update();

  // ============================================================ //
  // Getters & Setters
  // ============================================================ //

public:
  Side GetSide() { return kSide; }

private:
  Client* GetClient() const { return static_cast<Client*>(base_); }
  Server* GetServer() const { return static_cast<Server*>(base_); }

  NetworkState GetNetworkState() const;

  // ============================================================ //
  // Network Specific
  // ============================================================ //

private:
  void ConnectToServer();

  void StartServer();

 public:
  void NetworkInfo(alflib::String extra) const;

  void Broadcast(alflib::String message) const;

  // ============================================================ //
  // Constants
  // ============================================================ //

public:
  static constexpr Side kSide = side;

  static constexpr u16 kPort = 24812;

  static constexpr s64 kNetTicksPerSec = 32;

  // ============================================================ //
  // Member Variables
  // ============================================================ //

private:
  /**
   * Use GetClient and GetServer to access.
   */
  ISteamNetworkingSocketsCallbacks* base_;
};

// ============================================================ //
// Template Definition
// ============================================================ //

template<Side side>
void
World<side>::ConnectToServer()
{
  DIB_ASSERT(false, "attempting to run non specialized code");
}

template<Side side>
void
World<side>::StartServer()
{
  DIB_ASSERT(false, "attempting to run non specialized code");
}
}

#endif // WORLD_HPP_
