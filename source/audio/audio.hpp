#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <memory>
#include "core/types.hpp"

namespace dib
{

class Audio
{
 public:
  Audio();
  ~Audio();

 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};
}

#endif//AUDIO_HPP_
