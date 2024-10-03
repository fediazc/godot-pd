#include "audio_stream_pd.h"
#include "util.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

using namespace godot;

void AudioStreamPD::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mix_rate", "mix_rate"), &AudioStreamPD::set_mix_rate);
	ClassDB::bind_method(D_METHOD("get_mix_rate"), &AudioStreamPD::get_mix_rate);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mix_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_mix_rate", "get_mix_rate");
}

AudioStreamPD::AudioStreamPD() {
	mix_rate = 44100;
}

AudioStreamPD::~AudioStreamPD() {}

Ref<AudioStreamPlayback> AudioStreamPD::_instantiate_playback() const {
	Ref<AudioStreamPlaybackPD> playback;

	playback.instantiate();
	playback->stream = this;

	return playback;
}

void AudioStreamPD::set_mix_rate(int p_mix_rate) {
	mix_rate = p_mix_rate;
}

int AudioStreamPD::get_mix_rate() const {
	return mix_rate;
}

//////////////

void AudioStreamPlaybackPD::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_patch", "filename", "path"), &AudioStreamPlaybackPD::add_patch);
	ClassDB::bind_method(D_METHOD("remove_patch", "filename", "path"), &AudioStreamPlaybackPD::remove_patch);
	ClassDB::bind_method(D_METHOD("send_bang", "dest"), &AudioStreamPlaybackPD::send_bang);
	ClassDB::bind_method(D_METHOD("send_float", "dest", "value"), &AudioStreamPlaybackPD::send_float);
	ClassDB::bind_method(D_METHOD("send_symbol", "dest", "symbol"), &AudioStreamPlaybackPD::send_symbol);
	ClassDB::bind_method(D_METHOD("subscribe", "source"), &AudioStreamPlaybackPD::subscribe);
	ClassDB::bind_method(D_METHOD("unsubscribe", "source"), &AudioStreamPlaybackPD::unsubscribe);
	ClassDB::bind_method(D_METHOD("unsubscribe_all"), &AudioStreamPlaybackPD::unsubscribe_all);

	ADD_SIGNAL(MethodInfo("receive_bang", PropertyInfo(Variant::STRING, "dest")));
	ADD_SIGNAL(MethodInfo("receive_float", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::FLOAT, "num")));
	ADD_SIGNAL(MethodInfo("receive_symbol", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::STRING, "symbol")));
}

AudioStreamPlaybackPD::AudioStreamPlaybackPD() {
	active = false;
	stream = nullptr;
	receiver.set_signaller(this);
	pd.setReceiver(&receiver);
}

AudioStreamPlaybackPD::~AudioStreamPlaybackPD() {
	for (auto p : patches) {
		pd.closePatch(p);
	}

	pd.setReceiver(NULL);
	receiver.set_signaller(nullptr);

	pd.clear();
}

int32_t AudioStreamPlaybackPD::_mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) {
	if (!active) {
		return 0;
	}

	pd.receiveMessages();

	int ticks = p_frame_count / libpd_blocksize();
	float output[1024 * 2 * sizeof(float)];

	ERR_FAIL_COND_V_MSG(!pd.processFloat(ticks, NULL, output), p_frame_count, "Pure Data audio processing failed");

	for (unsigned i = 0; i < p_frame_count; i++) {
		p_dst_buffer->left = output[i * 2];
		p_dst_buffer->right = output[(i * 2) + 1];
		p_dst_buffer++;
	}

	return p_frame_count;
}

float AudioStreamPlaybackPD::_get_stream_sampling_rate() const {
	ERR_FAIL_COND_V_MSG(stream == nullptr, -1, "No audio stream available. Cannot get sample rate.");

	return stream->get_mix_rate();
}

void AudioStreamPlaybackPD::_start(double p_from_pos) {
	ERR_FAIL_COND_MSG(stream == nullptr, "No audio stream available.");

	ERR_FAIL_COND_MSG(!pd.init(0, 2, stream->get_mix_rate(), true), "Pure Data could not be initialized.");

	active = true;

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

void AudioStreamPlaybackPD::subscribe(String p_source) {
	pd.subscribe(std_string_from(p_source));
}

void AudioStreamPlaybackPD::unsubscribe(String p_source) {
	pd.unsubscribe(std_string_from(p_source));
}

void AudioStreamPlaybackPD::unsubscribe_all() {
	pd.unsubscribeAll();
}