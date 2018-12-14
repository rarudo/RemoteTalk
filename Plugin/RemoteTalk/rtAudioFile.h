#pragma once
#include "rtAudioData.h"

namespace rt {

enum class BitrateMode
{
    CBR,
    VBR,
};

struct OggSettings
{
    BitrateMode bitrate_mode = BitrateMode::VBR;
    int target_bitrate = 128 * 1000;
};


bool ExportWave(const AudioData& ad, std::ostream& os);
bool ExportWave(const AudioData& ad, const char* path);
bool ExportOgg(const AudioData& ad, std::ostream& os, const OggSettings& settings = {});
bool ExportOgg(const AudioData& ad, const char* path, const OggSettings& settings = {});

} // namespace rt
