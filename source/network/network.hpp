#ifndef NETWORK_HPP_
#define NETWORK_HPP_

namespace dib
{
class Network
{
 public:

  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();

 private:

};
}

#endif//NETWORK_HPP_
