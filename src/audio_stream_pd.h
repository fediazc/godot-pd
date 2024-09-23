#ifndef AUDIO_STREAM_PD_H
#define AUDIO_STREAM_PD_H

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <PdBase.hpp>

namespace godot {

class AudioStreamPD : public AudioStream {
    GDCLASS(AudioStreamPD, AudioStream)

    std::string patch_name;

protected:
    static void _bind_methods();

public:
    Ref<AudioStreamPlayback> _instantiate_playback() const override;
    String get_patch_name() const;
    std::string _get_patch_name() const;
    void set_patch_name(String p_patch_name);

    AudioStreamPD();
    ~AudioStreamPD();
};

class AudioStreamPlaybackPD : public AudioStreamPlaybackResampled {
    GDCLASS(AudioStreamPlaybackPD, AudioStreamPlaybackResampled)
    friend class AudioStreamPD;

    const AudioStreamPD *stream; 
    pd::PdBase pd;
    pd::Patch patch;

protected:
    static void _bind_methods();

public:
    int32_t _mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) override;
    float _get_stream_sampling_rate() const override;
    void _start(double p_from_pos) override;

    AudioStreamPlaybackPD();
    ~AudioStreamPlaybackPD();
};

}

#endif