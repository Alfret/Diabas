#ifndef AUDIO_MANAGER_HPP_
#define AUDIO_MANAGER_HPP_

#include <memory>
#include "core/types.hpp"
#include <optional>

namespace dib
{

using AudioId = u32;

class AudioManager
{
 public:
  AudioManager();

  ~AudioManager();

  /**
   * Add an audio file to the audio manager.
   */
  std::optional<AudioId> Add(const String& file) const;

  /**
   * Play an audio file that is stored in the manager.
   * @retval nullopt When id was not found
   * @retval false When the play action failed.
   * @retval true Success
   */
  std::optional<bool> Play(const AudioId id) const;

  /**
   * Pause an audio file that is stored in the manager. Calling play
   * on the same audio id will cause it to resume from previous position.
   * @retval nullopt When id was not found
   * @retval false When the pause action failed.
   * @retval true Success
   */
  std::optional<bool> Pause(const AudioId id) const;

 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};
}

#endif//AUDIO_MANAGER_HPP_
