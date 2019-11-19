#pragma once

#include "bus.h"
#include "olcPixelGameEngine.h"

class W65C816S {
public:
	enum class STATUSBITS
	{
		N = 1 << 7,
		V = 1 << 6,
		M = 1 << 5,
		X = 1 << 4,
		D = 1 << 3,
		I = 1 << 2,
		Z = 1 << 1,
		C = 1 << 0,
		E = 1 << 0,
	};

	typedef std::shared_ptr<W65C816S> SharedPtr;

	typedef union {
		uint32_t word;
		struct {
			uint16_t half_low;
			uint16_t half_high;
		};
		struct {
			uint8_t byte_low;
			uint8_t byte_high;
			uint8_t byte_extended;
			uint8_t byte_padding;
		};

	} Register;

private:
	Bus::SharedPtr bus;
	olc::PixelGameEngine* system;

	Register X;
	Register Y;
	Register S;
	Register A;
	Register PC;
	Register D;

protected:

public:
	W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system);
	~W65C816S();
};