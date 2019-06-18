#ifndef NETWORK_HPP_
#define NETWORK_HPP_
#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <variant>

namespace dib {

enum class NetworkMode : bool
{
  kClient,
  kServer
};

/**
 * Use either InitNetwork & ShutdownNetwork in your main function,
 * or construct an instance of this class and let RAII do the work for you.
 */
class Network
{
public:
  Network();

  ~Network();

  // ============================================================ //

  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();
};

}

#endif // NETWORK_HPP_
