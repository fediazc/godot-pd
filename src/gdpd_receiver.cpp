#include "gdpd_receiver.h"
#include "util.hpp"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gdpd;
using namespace godot;

godot::Array gdpd::Receiver::_godot_array_from(const pd::List &p_list) {
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
	ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");
	signaller->emit_signal("receive_bang", godot_string_from(p_dest));
}

void Receiver::receiveFloat(const std::string &p_dest, float p_num) {
	ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");
	signaller->emit_signal("receive_float", godot_string_from(p_dest), p_num);
}

void Receiver::receiveSymbol(const std::string &p_dest, const std::string &p_symbol) {
	ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");
	signaller->emit_signal("receive_symbol", godot_string_from(p_dest), godot_string_from(p_symbol));
}

void Receiver::receiveList(const std::string &p_dest, const pd::List &p_list) {
	ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");

	auto arr = _godot_array_from(p_list);

	signaller->emit_signal("receive_list", godot_string_from(p_dest), arr);
}

void gdpd::Receiver::receiveMessage(const std::string &p_dest, const std::string &p_msg, const pd::List &p_list) {
	ERR_FAIL_NULL_MSG(signaller, "No signaller is set. Cannot send signal");

	auto arr = _godot_array_from(p_list);

	signaller->emit_signal("receive_message", godot_string_from(p_dest), godot_string_from(p_msg), arr);
}

void Receiver::set_signaller(godot::RefCounted *p_signaller) {
	signaller = p_signaller;
}
