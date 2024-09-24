#ifndef AUDIO_STREAM_PD_H
#define AUDIO_STREAM_PD_H

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <PdBase.hpp>

namespace godot {

class AudioStreamPD : public AudioStream {
    GDCLASS(AudioStreamPD, AudioStream)

protected:
    static void _bind_methods();

public:
    Ref<AudioStreamPlayback> _instantiate_playback() const override;

    AudioStreamPD();
    ~AudioStreamPD();
};

class AudioStreamPlaybackPD : public AudioStreamPlaybackResampled {
    GDCLASS(AudioStreamPlaybackPD, AudioStreamPlaybackResampled)
    friend class AudioStreamPD;

    const AudioStreamPD *stream; 
    pd::PdBase pd;
    std::vector<pd::Patch> patches;

protected:
    static void _bind_methods();

public:
    int32_t _mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) override;
    float _get_stream_sampling_rate() const override;
    void _start(double p_from_pos) override;
    int add_patch(String p_filename, String p_path);
    void remove_patch(String p_filename, String p_path);
    void send_bang(String p_dest);
    void send_float(String p_dest, float p_value);
    void send_symbol(String p_dest, String p_symbol);

    AudioStreamPlaybackPD();
    ~AudioStreamPlaybackPD();
};

}

#endif