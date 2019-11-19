#pragma once

#include "bus.h"
#include "olcPixelGameEngine.h"

class W65C816S {
public:
	enum class STATUSBITS
	{
		N = 1 << 7,		// Negative 1 = negative
		V = 1 << 6,		// Overflow 1 = true
		M = 1 << 5,		// Memory Select 1 = 8-bit 0 = 16-bit
		X = 1 << 4,		// Index Register Select 1 = 8-bit 0 = 16-bit
		B = 1 << 4,		// BRK Bit 1 = BRK 0 = IRQ
		D = 1 << 3,		// Decimal Mode 1 = true
		I = 1 << 2,		// IRQ Disable 1 = disable
		Z = 1 << 1,		// Zero 1 = zero result
		C = 1 << 0,		// Carry 1 = true
		E = 1 << 0,		// Emulation
	};

	typedef std::shared_ptr<W65C816S> SharedPtr;

	typedef uint8_t Register8;

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

	} Register24;

private:
	Bus::SharedPtr bus;
	olc::PixelGameEngine* system;

	Register8 PSW;
	Register24 X;
	Register24 Y;
	Register24 S;
	Register24 A;
	Register24 PC;
	Register24 D;

protected:

public:
	W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system);
	~W65C816S();
};