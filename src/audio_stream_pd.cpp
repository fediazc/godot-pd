#include "audio_stream_pd.h"
#include "util.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

using namespace godot;

void AudioStreamPD::_bind_methods() {}

AudioStreamPD::AudioStreamPD() {}

AudioStreamPD::~AudioStreamPD() {}

Ref<AudioStreamPlayback> AudioStreamPD::_instantiate_playback() const {
	Ref<AudioStreamPlaybackPD> playback;

	playback.instantiate();
	playback->stream = this;

	return playback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AudioStreamPlaybackPD::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_patch", "filename", "path"), &AudioStreamPlaybackPD::add_patch);
	ClassDB::bind_method(D_METHOD("remove_patch", "filename", "path"), &AudioStreamPlaybackPD::remove_patch);
	ClassDB::bind_method(D_METHOD("send_bang", "dest"), &AudioStreamPlaybackPD::send_bang);
	ClassDB::bind_method(D_METHOD("send_float", "dest", "value"), &AudioStreamPlaybackPD::send_float);
	ClassDB::bind_method(D_METHOD("send_symbol", "dest", "symbol"), &AudioStreamPlaybackPD::send_symbol);
}

AudioStreamPlaybackPD::AudioStreamPlaybackPD() {
	if (!pd.init(0, 2, 44100)) {
		ERR_PRINT("Pure Data could not be initialized.");
	}
}

AudioStreamPlaybackPD::~AudioStreamPlaybackPD() {
	for (auto p : patches) {
		pd.closePatch(p);
	}
	// TODO check for multi-instance support, if enabled, PdBase::clear() should probably be called here
}

int32_t AudioStreamPlaybackPD::_mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) {
	int ticks = p_frame_count / libpd_blocksize();
	float output[1024 * 2 * sizeof(float)];

	if (!pd.isInited()) {
		ERR_PRINT_ONCE("Pure Data is not initialized. Cannot process audio.");
		return p_frame_count;
	}

	ERR_FAIL_COND_V_MSG(!pd.processFloat(ticks, NULL, output), p_frame_count, "Pure Data audio processing failed");

	for (unsigned i = 0; i < p_frame_count; i++) {
		p_dst_buffer->left = output[i * 2];
		p_dst_buffer->right = output[(i * 2) + 1];
		p_dst_buffer++;
	}

	return p_frame_count;
}

float AudioStreamPlaybackPD::_get_stream_sampling_rate() const {
	return 44100;
}

void AudioStreamPlaybackPD::_start(double p_from_pos) {
	if (!pd.isInited()) {
		ERR_PRINT("Pure Data is not initialized. Cannot start playback.");
		return;
	}
	pd.computeAudio(true);
	begin_resample();
}

int AudioStreamPlaybackPD::add_patch(String p_filename, String p_path) {
	auto patch = pd.openPatch(std_string_from(p_filename), std_string_from(p_path));
	ERR_FAIL_COND_V_MSG(!patch.isValid(), -1, "Patch located at " + p_path + "/" + p_filename + " was not found or could not be opened.");

	patches.push_back(patch);

	return patch.dollarZero();
}

void AudioStreamPlaybackPD::remove_patch(String p_filename, String p_path) {
	auto remove_end = std::remove_if(patches.begin(), patches.end(), [&](pd::Patch p) {
		if (p.filename() == std_string_from(p_filename) && p.path() == std_string_from(p_path)) {
			pd.closePatch(p);
			return true;
		}
		return false;
	});

	patches.erase(remove_end, patches.end());
}

void AudioStreamPlaybackPD::send_bang(String p_dest) {
	pd.sendBang(std_string_from(p_dest));
}

void AudioStreamPlaybackPD::send_float(String p_dest, float p_value) {
	pd.sendFloat(std_string_from(p_dest), p_value);
}

void AudioStreamPlaybackPD::send_symbol(String p_dest, String p_symbol) {
	pd.sendSymbol(std_string_from(p_dest), std_string_from(p_symbol));
}
