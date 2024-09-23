#include "audio_stream_pd.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void AudioStreamPD::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_patch_name"), &AudioStreamPD::get_patch_name);
	ClassDB::bind_method(D_METHOD("set_patch_name", "p_patch_name"), &AudioStreamPD::set_patch_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "patch_name"), "set_patch_name", "get_patch_name");
}

AudioStreamPD::AudioStreamPD() { }

AudioStreamPD::~AudioStreamPD() { }

Ref<AudioStreamPlayback> AudioStreamPD::_instantiate_playback() const {
    Ref<AudioStreamPlaybackPD> playback;

    playback.instantiate();
    playback->stream = this;

    return playback;
}

String AudioStreamPD::get_patch_name() const {
    return String(patch_name.c_str());
}

std::string godot::AudioStreamPD::_get_patch_name() const {
    return patch_name;
}

void AudioStreamPD::set_patch_name(String p_patch_name) {
    patch_name = std::string(p_patch_name.utf8().get_data());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AudioStreamPlaybackPD::_bind_methods() { }

int32_t AudioStreamPlaybackPD::_mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) {
    int ticks = p_frame_count / libpd_blocksize();
    float output[1024 * 2 * sizeof(float)];
    pd.processFloat(ticks, NULL, output);

    for(unsigned i = 0; i < p_frame_count; i++) {
       p_dst_buffer->left = output[i*2];
       p_dst_buffer->right = output[(i*2)+1];
       p_dst_buffer++;
    }

    return p_frame_count;
}

float godot::AudioStreamPlaybackPD::_get_stream_sampling_rate() const {
    return 44100;
}

void godot::AudioStreamPlaybackPD::_start(double p_from_pos) {
    pd.init(0, 2, 44100, true); // TODO: check if init failed
    pd.computeAudio(true);

    patch = pd.openPatch(stream->_get_patch_name(), "./pd");

    begin_resample();
}

AudioStreamPlaybackPD::AudioStreamPlaybackPD() { }

AudioStreamPlaybackPD::~AudioStreamPlaybackPD() {
    pd.closePatch(patch);
    pd.clear();
}
