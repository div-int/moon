#include "w65c816s.h"

W65C816S::W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system)
{
	this->bus = bus;
	this->system = system;

	X.word = 0x11223344;

	std::cout << std::hex << X.word << " " << X.half_high << ":" << X.half_low << " " << unsigned(X.byte_padding) << ":" << unsigned(X.byte_extended) << ":" << unsigned(X.byte_high) << ":" << unsigned(X.byte_low) << std::endl;
}

W65C816S::~W65C816S()
{
	// TO DO
}