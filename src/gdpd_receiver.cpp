#include "gdpd_receiver.h"
#include "util.hpp"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gdpd;
using namespace godot;

#define CHECK_SIGNALLER ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");

Array Receiver::_godot_array_from(const pd::List &p_list) {
	Array arr;
	for (unsigned i = 0; i < p_list.len(); i++) {
		if (p_list.isFloat(i)) {
			arr.append(p_list.getFloat(i));
		} else {
			arr.append(godot_string_from(p_list.getSymbol(i)));
		}
	}
	return arr;
}

Receiver::Receiver() {
	signaller = nullptr;
}

Receiver::~Receiver() {}

void Receiver::print(const std::string &p_message) {
	UtilityFunctions::print(godot_string_from(p_message));
}

void Receiver::receiveBang(const std::string &p_dest) {
	CHECK_SIGNALLER
	signaller->emit_signal("receive_bang", godot_string_from(p_dest));
}

void Receiver::receiveFloat(const std::string &p_dest, float p_num) {
	CHECK_SIGNALLER
	signaller->emit_signal("receive_float", godot_string_from(p_dest), p_num);
}

void Receiver::receiveSymbol(const std::string &p_dest, const std::string &p_symbol) {
	CHECK_SIGNALLER
	signaller->emit_signal("receive_symbol", godot_string_from(p_dest), godot_string_from(p_symbol));
}

void Receiver::receiveList(const std::string &p_dest, const pd::List &p_list) {
	CHECK_SIGNALLER
	auto arr = _godot_array_from(p_list);

	signaller->emit_signal("receive_list", godot_string_from(p_dest), arr);
}

void Receiver::receiveMessage(const std::string &p_dest, const std::string &p_msg, const pd::List &p_list) {
	CHECK_SIGNALLER
	auto arr = _godot_array_from(p_list);

	signaller->emit_signal("receive_message", godot_string_from(p_dest), godot_string_from(p_msg), arr);
}

void Receiver::receiveNoteOn(const int p_channel, const int p_pitch, const int p_velocity) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_note_on", p_channel, p_pitch, p_velocity);
}

void Receiver::receiveControlChange(const int p_channel, const int p_controller, const int p_value) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_control_change", p_channel, p_controller);
}

void Receiver::receiveProgramChange(const int p_channel, const int p_value) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_program_change", p_channel, p_value);
}

void Receiver::receivePitchBend(const int p_channel, const int p_value) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_pitch_bend", p_channel, p_value);
}

void Receiver::receiveAftertouch(const int p_channel, const int p_value) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_aftertouch", p_channel, p_value);
}

void Receiver::receivePolyAftertouch(const int p_channel, const int p_pitch, const int p_value) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_poly_aftertouch", p_channel, p_pitch, p_value);
}

void Receiver::receiveMidiByte(const int p_port, const int p_byte) {
	CHECK_SIGNALLER

	signaller->emit_signal("receive_midi_byte", p_port, p_byte);
}

void Receiver::set_signaller(godot::RefCounted *p_signaller) {
	signaller = p_signaller;
}
