#pragma once

#include <thread>

#include "bus.h"
#include "olcPixelGameEngine.h"

class W65C816S {
public:
	enum class MODE
	{
		EMULATION = 1,
		NATIVE = 0,
	};

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
		uint16_t db0_15;
		struct {
			uint8_t b0_7;
			uint8_t b8_15;
		};

	} Register16, Address16;

	typedef union {
		uint32_t tb0_23;
		struct {
			uint16_t db0_15;
			uint8_t b16_23;
			uint8_t padding;
		};
		struct {
			uint8_t b0_7;
			uint8_t b8_15;
			uint8_t b16_23;
			uint8_t padding;
		};

	} Register24, Address24;

	typedef struct {
		Address24 IRQ;
		Address24 RESET;
		Address24 NMI;
		Address24 ABORT;
		Address24 BRK;
		Address24 COP;
	} VECTORS;

private:
	Bus::SharedPtr bus;
	olc::PixelGameEngine* system;
	bool running;

	MODE mode;

	Register8 IR;		// Instruction register
	Register8 P;		// Processor status register
	Register8 EP;		// Extended bits of status register
	Register16 TCU;		// Timing control unit
	Register16 ALU;		// Arithmetic and logic unit
	Register16 A;		// Accumulator
	Register16 X;		// X index register
	Register16 Y;		// Y index register
	Register24 DBR;		// Data bank register
	Register24 D;		// Direct register
	Register24 PC;		// Program counter
	Register24 S;		// Stack register

	VECTORS native;
	VECTORS emulation;

	Bus::Line1Bit ABORTB;
	Bus::Line16Bit A0_A15;
	Bus::Line1Bit BE;
	Bus::Line8Bit D0_D7;
	Bus::Line1Bit E;
	Bus::Line1Bit IRQB;
	Bus::Line1Bit MLB;
	Bus::Line1Bit MX;
	Bus::Line1Bit NMIB;
	Bus::Line1Bit PHI2;
	Bus::Line1Bit RWB;
	Bus::Line1Bit RDY;
	Bus::Line1Bit RESB;
	Bus::Line1Bit VDA;
	Bus::Line1Bit VPA;
	Bus::Line1Bit VPB;

	uint32_t reset_low_cycles;
	bool reset_low;

	uint64_t clock_count;

protected:

public:
	W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system);
	~W65C816S();

	// Status register functions

	void SetC() { P |= (Register8)STATUSBITS::C;  }
	void ClearC() { P &= ~(Register8)STATUSBITS::C; }
	bool GetC() { return ((P & (Register8)STATUSBITS::C) ? true : false); }

	void SetE() { EP |= (Register8)STATUSBITS::E; }
	void ClearE() { EP &= ~(Register8)STATUSBITS::E; }
	bool GetE() { return ((EP & (Register8)STATUSBITS::E) ? true : false); }

	void SetZ() { P |= (Register8)STATUSBITS::Z; }
	void ClearZ() { P &= ~(Register8)STATUSBITS::Z; }
	bool GetZ() { return ((P & (Register8)STATUSBITS::Z) ? true : false); }

	void SetI() { P |= (Register8)STATUSBITS::I; }
	void ClearI() { P &= ~(Register8)STATUSBITS::I; }
	bool GetI() { return ((P & (Register8)STATUSBITS::I) ? true : false); }

	void SetD() { P |= (Register8)STATUSBITS::D; }
	void ClearD() { P &= ~(Register8)STATUSBITS::D; }
	bool GetD() { return ((P & (Register8)STATUSBITS::D) ? true : false); }

	void SetX() { P |= (Register8)STATUSBITS::X; X.b8_15 = 0x00; Y.b8_15 = 0x00; }
	void ClearX() { P &= ~(Register8)STATUSBITS::X; }
	bool GetX() { return ((P & (Register8)STATUSBITS::X) ? true : false); }

	void SetB() { EP |= (Register8)STATUSBITS::B; }
	void ClearB() { EP &= ~(Register8)STATUSBITS::B; }
	bool GetB() { return ((EP & (Register8)STATUSBITS::B) ? true : false); }

	void SetM() { P |= (Register8)STATUSBITS::M; }
	void ClearM() { P &= ~(Register8)STATUSBITS::M; }
	bool GetM() { return ((P & (Register8)STATUSBITS::M) ? true : false); }

	void SetV() { P |= (Register8)STATUSBITS::V; }
	void ClearV() { P &= ~(Register8)STATUSBITS::V; }
	bool GetV() { return ((P & (Register8)STATUSBITS::V) ? true : false); }

	void SetN() { P |= (Register8)STATUSBITS::N; }
	void ClearN() { P &= ~(Register8)STATUSBITS::N; }
	bool GetN() { return ((P & (Register8)STATUSBITS::N) ? true : false); }

	// CPU functions

	void Reset();

	void Clock();

	void Start();
	void Run();
	void Stop();
};