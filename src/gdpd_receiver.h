#ifndef GDPD_RECEIVER_H
#define GDPD_RECEIVER_H

#include <PdBase.hpp>
#include <godot_cpp/classes/ref.hpp>

namespace gdpd {

class Receiver : public pd::PdReceiver {
	// should have signals receive_bang(str), receive_float(str, float), etc.
	godot::RefCounted *signaller;

public:
	Receiver();
	~Receiver();

	void print(const std::string &p_message) override;

	void receiveBang(const std::string &p_dest) override;
	void receiveFloat(const std::string &p_dest, float p_num) override;
	void receiveSymbol(const std::string &p_dest, const std::string &p_symbol) override;
	// void receiveList(const std::string &dest, const pd::List &list) override;
	// void receiveMessage(const std::string &dest, const std::string &msg, const pd::List &list) override;

	void set_signaller(godot::RefCounted *p_signaller);
};

} //namespace gdpd

#endif