#include "audio.hpp"

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

namespace dib
{

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

class Audio::Impl
{
 public:
  Impl() {
    ma_result result;

    String file("/media/dpool/Music/Other/Ellinia Forest Extended.ogg");


    result = ma_decoder_init_file_vorbis(file.GetUTF8(), NULL, &decoder);
    if (result != MA_SUCCESS) {
      DLOG_ERROR("could not load");
      std::exit(0);
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
      ma_decoder_uninit(&decoder);
      DLOG_ERROR("could not open playback device");
      std::exit(0);
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
      ma_device_uninit(&device);
      ma_decoder_uninit(&decoder);
      DLOG_ERROR("could not start playback device");
      std::exit(0);
    }
  }



  ~Impl() {
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
  }

 private:
   ma_decoder decoder;
   ma_device_config deviceConfig;
   ma_device device;
};

// ============================================================ //

Audio::Audio()
    : pimpl_(std::make_unique<Impl>())
{
}

Audio::~Audio()
{

}

}
