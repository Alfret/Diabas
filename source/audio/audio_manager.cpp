#include "audio_manager.hpp"
#include <unordered_map>
#include <dlog.hpp>

#define DR_FLAC_IMPLEMENTATION
#include <extras/dr_flac.h> /* Enables FLAC decoding. */
#define DR_MP3_IMPLEMENTATION
#include <extras/dr_mp3.h> /* Enables MP3 decoding. */
#define DR_WAV_IMPLEMENTATION
#include <extras/dr_wav.h> /* Enables WAV decoding. */

#include <extras/stb_vorbis.c>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace dib {

static void
data_callback(ma_device* pDevice,
              void* pOutput,
              const void* pInput,
              ma_uint32 frameCount)
{
  ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

  (void)pInput;
}

// ============================================================ //
// Impl
// ============================================================ //

class AudioInstance
{
public:
  AudioInstance() = default;

  ~AudioInstance()
  {
    ma_device_uninit(&device_);
    ma_decoder_uninit(&decoder_);
  }

  bool Init(const String& file)
  {
    ma_result result = ma_decoder_init_file(file.GetUTF8(), NULL, &decoder_);
    if (result != MA_SUCCESS) {
      DLOG_ERROR("could not load file [{}]", file);
      return false;
    }

    device_config_ = ma_device_config_init(ma_device_type_playback);
    device_config_.playback.format = decoder_.outputFormat;
    device_config_.playback.channels = decoder_.outputChannels;
    device_config_.sampleRate = decoder_.outputSampleRate;
    device_config_.dataCallback = data_callback;
    device_config_.pUserData = &decoder_;

    if (ma_device_init(NULL, &device_config_, &device_) != MA_SUCCESS) {
      ma_decoder_uninit(&decoder_);
      DLOG_ERROR("could not open playback device");
      return false;
    }
    return true;
  }

  bool Play()
  {
    if (ma_device_start(&device_) != MA_SUCCESS) {
      ma_device_uninit(&device_);
      ma_decoder_uninit(&decoder_);
      DLOG_ERROR("could not start playback device");
      return false;
    }
    return true;
  }

  bool Pause()
  {
    if (ma_device_stop(&device_) != MA_SUCCESS) {
      ma_device_uninit(&device_);
      ma_decoder_uninit(&decoder_);
      return false;
    }
    return true;
  }

  AudioInstance(const AudioInstance& other) = delete;
  AudioInstance& operator=(const AudioInstance& other) = delete;
  AudioInstance(AudioInstance&& other) noexcept = delete;
  AudioInstance& operator=(AudioInstance&& other) noexcept = delete;

private:
  ma_decoder decoder_;
  ma_device_config device_config_;
  ma_device device_;
};

class AudioManager::Impl
{
public:
  Impl() = default;

  ~Impl()
  {
    // TODO make sure they are destructed
  }

  std::optional<AudioId> Add(const String& file)
  {
    const AudioId current_id = counter_++;
    auto [it, ok] =
      instances_.insert({ current_id, std::make_unique<AudioInstance>() });
    if (ok) {
      const bool init_ok = it->second->Init(file);
      if (init_ok) {
        return current_id;
      } else {
        instances_.erase(it);
      }
    }
    return std::nullopt;
  }

  std::optional<bool> Play(const AudioId id) const
  {
    if (auto it = instances_.find(id); it != instances_.end()) {
      return it->second->Play();
    }
    return std::nullopt;
  }

  std::optional<bool> Pause(const AudioId id)
  {
    if (auto it = instances_.find(id); it != instances_.end()) {
      return it->second->Pause();
    }
    return std::nullopt;
  }

private:
  AudioId counter_ = 0;
  std::unordered_map<AudioId, std::unique_ptr<AudioInstance>> instances_{};
};

// ============================================================ //
// Audio Definition
// ============================================================ //

AudioManager::AudioManager()
  : pimpl_(std::make_unique<Impl>())
{}

AudioManager::~AudioManager() {}

std::optional<AudioId>
AudioManager::Add(const String& file) const
{
  return pimpl_->Add(file);
}

std::optional<bool>
AudioManager::Play(const AudioId id) const
{
  return pimpl_->Play(id);
}

std::optional<bool>
AudioManager::Pause(const AudioId id) const
{
  return pimpl_->Pause(id);
}
}
