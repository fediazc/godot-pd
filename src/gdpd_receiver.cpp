#include "gdpd_receiver.h"
#include "util.hpp"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gdpd;

Receiver::Receiver() {
	signaller = nullptr;
}

Receiver::~Receiver() {}

void Receiver::print(const std::string &p_message) {
	godot::UtilityFunctions::print(godot_string_from(p_message));
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

void Receiver::set_signaller(godot::RefCounted *p_signaller) {
	signaller = p_signaller;
}
