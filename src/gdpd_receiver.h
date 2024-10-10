#ifndef GDPD_RECEIVER_H
#define GDPD_RECEIVER_H

#include <PdBase.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>

namespace gdpd {

class Receiver : public pd::PdReceiver {
	// should have signals receive_bang(str), receive_float(str, float), etc.
	godot::RefCounted *signaller;

protected:
	static godot::Array _godot_array_from(const pd::List &p_list);

public:
	Receiver();
	~Receiver();

	void print(const std::string &p_message) override;

	void receiveBang(const std::string &p_dest) override;
	void receiveFloat(const std::string &p_dest, float p_num) override;
	void receiveSymbol(const std::string &p_dest, const std::string &p_symbol) override;
	void receiveList(const std::string &p_dest, const pd::List &p_list) override;
	void receiveMessage(const std::string &p_dest, const std::string &p_msg, const pd::List &p_list) override;

	void set_signaller(godot::RefCounted *p_signaller);
};

} //namespace gdpd

#endif