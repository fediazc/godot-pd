#include "audio_stream_pd.h"
#include "util.hpp"
#include <algorithm>
#include <filesystem>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

using namespace godot;
namespace fs = std::filesystem;

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
#ifdef PDINSTANCE
	Ref<AudioStreamPlaybackPD> playback;

	playback.instantiate();
#else
	auto playback = Ref<AudioStreamPlaybackPD>(Engine::get_singleton()->get_singleton("MainPlaybackPD"));
#endif

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
	ClassDB::bind_method(D_METHOD("open_patch", "path"), &AudioStreamPlaybackPD::open_patch);
	ClassDB::bind_method(D_METHOD("close_patch", "path"), &AudioStreamPlaybackPD::close_patch);
	ClassDB::bind_method(D_METHOD("close_patch_id", "dollar_zero"), &AudioStreamPlaybackPD::close_patch_id);
	ClassDB::bind_method(D_METHOD("send_bang", "dest"), &AudioStreamPlaybackPD::send_bang);
	ClassDB::bind_method(D_METHOD("send_float", "dest", "value"), &AudioStreamPlaybackPD::send_float);
	ClassDB::bind_method(D_METHOD("send_symbol", "dest", "symbol"), &AudioStreamPlaybackPD::send_symbol);
	ClassDB::bind_method(D_METHOD("send_list", "dest", "list"), &AudioStreamPlaybackPD::send_list);
	ClassDB::bind_method(D_METHOD("send_message", "dest", "type", "values"), &AudioStreamPlaybackPD::send_message);
	ClassDB::bind_method(D_METHOD("send_note_on", "channel", "pitch", "velocity"), &AudioStreamPlaybackPD::send_note_on, DEFVAL(64));
	ClassDB::bind_method(D_METHOD("send_note_off", "channel", "pitch"), &AudioStreamPlaybackPD::send_note_off);
	ClassDB::bind_method(D_METHOD("send_control_change", "channel", "controller", "value"), &AudioStreamPlaybackPD::send_control_change);
	ClassDB::bind_method(D_METHOD("send_program_change", "channel", "value"), &AudioStreamPlaybackPD::send_program_change);
	ClassDB::bind_method(D_METHOD("send_pitch_bend", "channel", "value"), &AudioStreamPlaybackPD::send_pitch_bend);
	ClassDB::bind_method(D_METHOD("send_aftertouch", "channel", "value"), &AudioStreamPlaybackPD::send_aftertouch);
	ClassDB::bind_method(D_METHOD("send_poly_aftertouch", "channel", "pitch", "value"), &AudioStreamPlaybackPD::send_poly_aftertouch);
	ClassDB::bind_method(D_METHOD("send_midi_byte", "port", "value"), &AudioStreamPlaybackPD::send_midi_byte);
	ClassDB::bind_method(D_METHOD("send_sysex", "port", "value"), &AudioStreamPlaybackPD::send_sysex);
	ClassDB::bind_method(D_METHOD("send_sys_realtime", "port", "value"), &AudioStreamPlaybackPD::send_sys_realtime);
	ClassDB::bind_method(D_METHOD("subscribe", "source"), &AudioStreamPlaybackPD::subscribe);
	ClassDB::bind_method(D_METHOD("unsubscribe", "source"), &AudioStreamPlaybackPD::unsubscribe);
	ClassDB::bind_method(D_METHOD("unsubscribe_all"), &AudioStreamPlaybackPD::unsubscribe_all);
	ClassDB::bind_method(D_METHOD("get_array_size", "name"), &AudioStreamPlaybackPD::get_array_size);
	ClassDB::bind_method(D_METHOD("resize_array", "name", "size"), &AudioStreamPlaybackPD::resize_array);
	ClassDB::bind_method(D_METHOD("read_array", "name", "read_len", "offset"), &AudioStreamPlaybackPD::read_array, DEFVAL(-1), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("write_array", "name", "source", "write_len", "offset"), &AudioStreamPlaybackPD::write_array, DEFVAL(-1), DEFVAL(0));

	ADD_SIGNAL(MethodInfo("receive_bang", PropertyInfo(Variant::STRING, "dest")));
	ADD_SIGNAL(MethodInfo("receive_float", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::FLOAT, "num")));
	ADD_SIGNAL(MethodInfo("receive_symbol", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::STRING, "symbol")));
	ADD_SIGNAL(MethodInfo("receive_list", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::ARRAY, "list")));
	ADD_SIGNAL(MethodInfo("receive_message", PropertyInfo(Variant::STRING, "dest"), PropertyInfo(Variant::STRING, "msg"), PropertyInfo(Variant::ARRAY, "list")));
	ADD_SIGNAL(MethodInfo("receive_note_on", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "pitch"), PropertyInfo(Variant::INT, "velocity")));
	ADD_SIGNAL(MethodInfo("receive_control_change", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "controller"), PropertyInfo(Variant::INT, "value")));
	ADD_SIGNAL(MethodInfo("receive_program_change", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "value")));
	ADD_SIGNAL(MethodInfo("receive_pitch_bend", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "value")));
	ADD_SIGNAL(MethodInfo("receive_aftertouch", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "value")));
	ADD_SIGNAL(MethodInfo("receive_poly_aftertouch", PropertyInfo(Variant::INT, "channel"), PropertyInfo(Variant::INT, "pitch"), PropertyInfo(Variant::INT, "value")));
	ADD_SIGNAL(MethodInfo("receive_midi_byte", PropertyInfo(Variant::INT, "port"), PropertyInfo(Variant::INT, "value")));
}

pd::List AudioStreamPlaybackPD::_pd_list_from(const Array &p_arr) {
	pd::List list;
	for (int64_t i = 0; i < p_arr.size(); i++) {
		switch (const Variant val = p_arr[i]; val.get_type()) {
			case Variant::FLOAT:
			case Variant::INT:
				list.addFloat(float(val));
				break;
			case Variant::STRING:
				list.addSymbol(std_string_from(String(val)));
				break;
			default:
				ERR_FAIL_V_MSG(pd::List(), "Value of type \"" + Variant::get_type_name(val.get_type()) + "\" (at index " + godot_string_from(std::to_string(i)) + ") cannot be added to a Pure Data list.");
		}
	}
	return list;
}

AudioStreamPlaybackPD::AudioStreamPlaybackPD() {
	active = false;
	stream = nullptr;
	receiver.set_signaller(this);
	pd.setReceiver(&receiver);
	pd.setMidiReceiver(&receiver);
}

AudioStreamPlaybackPD::~AudioStreamPlaybackPD() {
	for (auto p : patches) {
		pd.closePatch(p);
	}

	pd.setReceiver(NULL);
	pd.setMidiReceiver(NULL);
	receiver.set_signaller(nullptr);

	pd.clear();
}

int32_t AudioStreamPlaybackPD::_mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) {
	if (!active) {
		return 0;
	}

	pd.receiveMessages();
	pd.receiveMidi();

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

int AudioStreamPlaybackPD::open_patch(String p_path) {
	fs::path path = std_string_from(p_path);
	auto filename = path.filename().string();
	auto dir = path.parent_path().string();

	ERR_FAIL_COND_V_MSG(filename.empty(), -1, "No filename included in path" + p_path);

	if (dir.empty()) {
		dir = ".";
	}

	auto patch = pd.openPatch(filename, dir);
	ERR_FAIL_COND_V_MSG(!patch.isValid(), -1, "Patch located at " + godot_string_from(dir) + "/" + godot_string_from(filename) + " was not found or could not be opened.");

	patches.push_back(patch);

	return patch.dollarZero();
}

void AudioStreamPlaybackPD::close_patch(String p_path) {
	fs::path path = std_string_from(p_path);
	auto filename = path.filename().string();
	auto dir = path.parent_path().string();

	ERR_FAIL_COND_MSG(filename.empty(), "No filename included in path" + p_path);

	if (dir.empty()) {
		dir = ".";
	}

	auto remove_end = std::remove_if(patches.begin(), patches.end(), [&](pd::Patch p) {
		if (p.filename() == filename && p.path() == dir) {
			pd.closePatch(p);
			return true;
		}
		return false;
	});

	patches.erase(remove_end, patches.end());
}

void AudioStreamPlaybackPD::close_patch_id(int p_dollar_zero) {
	auto remove_end = std::remove_if(patches.begin(), patches.end(), [&](pd::Patch p) {
		if (p.dollarZero() == p_dollar_zero) {
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

void AudioStreamPlaybackPD::send_list(String p_dest, Array p_arr) {
	auto list = _pd_list_from(p_arr);

	ERR_FAIL_COND_MSG(list.len() == 0 && !p_arr.is_empty(), "Failed to create list.");

	pd.sendList(std_string_from(p_dest), list);
}

void AudioStreamPlaybackPD::send_message(String p_dest, String p_msg, Array p_arr) {
	auto list = _pd_list_from(p_arr);

	ERR_FAIL_COND_MSG(list.len() == 0 && !p_arr.is_empty(), "Failed to create list.");

	pd.sendMessage(std_string_from(p_dest), std_string_from(p_msg), list);
}

void AudioStreamPlaybackPD::send_note_on(int p_channel, int p_pitch, int p_velocity) {
	pd.sendNoteOn(p_channel, p_pitch, p_velocity);
}

void AudioStreamPlaybackPD::send_note_off(int p_channel, int p_pitch) {
	pd.sendNoteOn(p_channel, p_pitch, 0);
}

void AudioStreamPlaybackPD::send_control_change(int p_channel, int p_controller, int p_value) {
	pd.sendControlChange(p_channel, p_controller, p_value);
}

void AudioStreamPlaybackPD::send_program_change(int p_channel, int p_value) {
	pd.sendProgramChange(p_channel, p_value);
}
void AudioStreamPlaybackPD::send_pitch_bend(int p_channel, int p_value) {
	pd.sendPitchBend(p_channel, p_value);
}
void AudioStreamPlaybackPD::send_aftertouch(int p_channel, int p_value) {
	pd.sendAftertouch(p_channel, p_value);
}
void AudioStreamPlaybackPD::send_poly_aftertouch(int p_channel, int p_pitch, int p_value) {
	pd.sendPolyAftertouch(p_channel, p_pitch, p_value);
}

void AudioStreamPlaybackPD::send_midi_byte(int p_port, int p_value) {
	pd.sendMidiByte(p_port, p_value);
}

void AudioStreamPlaybackPD::send_sysex(int p_port, int p_value) {
	pd.sendSysex(p_port, p_value);
}

void AudioStreamPlaybackPD::send_sys_realtime(int p_port, int p_value) {
	pd.sendSysRealTime(p_port, p_value);
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

int AudioStreamPlaybackPD::get_array_size(String p_name) {
	return libpd_arraysize(std_string_from(p_name).c_str());
}

void AudioStreamPlaybackPD::resize_array(String p_name, int64_t p_size) {
	ERR_FAIL_COND_MSG(get_array_size(p_name) < 0, "Array \"" + p_name + "\" was not be found.");
	ERR_FAIL_COND_MSG(!pd.resizeArray(std_string_from(p_name), p_size), "Array resize failed.");
}

Array AudioStreamPlaybackPD::read_array(String p_name, int p_read_len, int p_offset) {
	ERR_FAIL_COND_V_MSG(p_offset < 0, Array(), "Cannot read from an array from a negative offset (" + godot_string_from(std::to_string(p_offset)) + ")");

	auto arr_len = get_array_size(p_name);

	ERR_FAIL_COND_V_MSG(arr_len < 0, Array(), "Array \"" + p_name + "\" was not be found.");

	auto true_read_len = p_read_len < 0 ? arr_len : p_read_len;
	ERR_FAIL_COND_V_MSG(true_read_len + p_offset > arr_len, Array(), "Read exceeds bound of the Pure Data array. (Requested to read " + godot_string_from(std::to_string(true_read_len)) + " elements at an offset of " + godot_string_from(std::to_string(p_offset)) + " from an array of size " + godot_string_from(std::to_string(arr_len)) + ").");

	std::vector<float> vec;
	ERR_FAIL_COND_V_MSG(!pd.readArray(std_string_from(p_name), vec, p_read_len, p_offset), Array(), "Read from array failed.");

	Array arr;
	for (auto val : vec) {
		arr.append(val);
	}

	return arr;
}

void AudioStreamPlaybackPD::write_array(String p_name, TypedArray<float> p_source, int p_write_len, int p_offset) {
	ERR_FAIL_COND_MSG(p_offset < 0, "Cannot write to an array from a negative offset (" + godot_string_from(std::to_string(p_offset)) + ").");

	auto arr_len = get_array_size(p_name);

	ERR_FAIL_COND_MSG(arr_len < 0, "Array \"" + p_name + "\" was not be found.");

	std::vector<float> vec;
	for (int64_t i = 0; i < p_source.size(); i++) {
		vec.push_back(float(p_source[i]));
	}

	auto true_write_len = p_write_len < 0 ? arr_len : p_write_len;
	ERR_FAIL_COND_MSG(true_write_len > p_source.size(), "Not enough elements in the given source array (Requested to write " + godot_string_from(std::to_string(true_write_len)) + " elements, only given " + godot_string_from(std::to_string(p_source.size())) + ").");
	ERR_FAIL_COND_MSG(true_write_len + p_offset > arr_len, "Write exceeds bound of the Pure Data array. (Requested to write " + godot_string_from(std::to_string(true_write_len)) + " elements at an offset of " + godot_string_from(std::to_string(p_offset)) + " to an array of size " + godot_string_from(std::to_string(arr_len)) + ").");

	ERR_FAIL_COND_MSG(!pd.writeArray(std_string_from(p_name), vec, p_write_len, p_offset), "Write to array failed.");
}
