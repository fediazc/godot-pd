#ifndef AUDIO_STREAM_PD_H
#define AUDIO_STREAM_PD_H

#include "gdpd_receiver.h"
#include <PdBase.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

class AudioStreamPD : public AudioStream {
	GDCLASS(AudioStreamPD, AudioStream)

	int mix_rate;

protected:
	static void _bind_methods();

public:
	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	void set_mix_rate(int p_mix_rate);
	int get_mix_rate() const;

	AudioStreamPD();
	~AudioStreamPD();
};

class AudioStreamPlaybackPD : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackPD, AudioStreamPlaybackResampled)
	friend class AudioStreamPD;

	const AudioStreamPD *stream;
	gdpd::Receiver receiver;
	pd::PdBase pd;
	std::vector<pd::Patch> patches;
	bool active;

protected:
	static void _bind_methods();
	static pd::List _pd_list_from(const Array &p_arr);

public:
	int32_t _mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) override;
	float _get_stream_sampling_rate() const override;
	void _start(double p_from_pos) override;
	int add_patch(String p_filename, String p_path);
	void remove_patch(String p_filename, String p_path);
	void send_bang(String p_dest);
	void send_float(String p_dest, float p_value);
	void send_symbol(String p_dest, String p_symbol);
	void send_list(String p_dest, Array p_arr);
	void send_message(String p_dest, String p_msg, Array p_arr);
	void subscribe(String p_source);
	void unsubscribe(String p_source);
	void unsubscribe_all();
	int get_array_size(String p_name);
	void resize_array(String p_name, int64_t size);
	Array read_array(String p_name, int p_read_len, int p_offset);
	void write_array(String p_name, TypedArray<float> p_source, int p_write_len, int p_offset);

	AudioStreamPlaybackPD();
	~AudioStreamPlaybackPD();
};

} //namespace godot

#endif