#pragma once

#include <string>
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

	enum class ADDRESSINGMODES
	{
		immediate = 0,
		accumulator,
		program_counter_relative,
		program_counter_relative_long,
		implied,
		stack,
		direct,
		direct_indexed_with_x,
		direct_indexed_with_y,
		direct_indirect,
		direct_indexed_indirect,
		direct_indirect_indexed,
		direct_indirect_long,
		direct_indirect_long_indexed,
		absolute,
		absolute_indexed_with_x,
		absolute_indexed_with_y,
		absolute_long,
		absolute_long_indexed,
		stack_relative,
		stack_relative_indirect_indexed,
		absolute_indirect,
		absolute_indexed_indirect,
		block_move
	};

	typedef struct
	{
		std::string symbol;
		std::string description;
	} ADDRESSINGMODE;

	ADDRESSINGMODE addressing_modes[24] = {
		{"#","immediate"},
		{"A","accumulator"},
		{"r","program counter relative"},
		{"rl","program counter relative long"},
		{"i","implied"},
		{"s","stack"},
		{"d","direct"},
		{"d,x","direct indexed with x"},
		{"d,y","direct indexed with y"},
		{"(d)","direct indirect"},
		{"(d,x)","direct indexed indirect"},
		{"(d),y","direct indirect indexed"},
		{"[d]","direct indirect long"},
		{"[d],y","direct indirect long indexed"},
		{"a","absolute"},
		{"a,x","absolute indexed with x"},
		{"a,y","absolute indexed with y"},
		{"al","absolute long"},
		{"al,x","absolute long indexed"},
		{"d,s","stack relative"},
		{"(d,s),y","stack relative indirect indexed"},
		{"(a)","absolute indirect"},
		{"(a,x)","absolute indexed indirect"},
		{"xyc","block move"},
	};

	typedef struct
	{
		std::string mnemonic;
		ADDRESSINGMODES addressing_mode;
		uint8_t cycles;
		uint8_t bytes;
	} OPCODE;

	OPCODE opcodes[256] = {

		// 0x00 - 0x0f

		{"BRK", ADDRESSINGMODES::stack, 7, 2},
		{"ORA", ADDRESSINGMODES::direct_indexed_indirect, 6, 2},
		{"COP", ADDRESSINGMODES::stack, 7, 2},
		{"ORA", ADDRESSINGMODES::stack_relative, 4, 2},
		{"TSB", ADDRESSINGMODES::direct, 5, 2},
		{"ORA", ADDRESSINGMODES::direct, 3, 2},
		{"ASL", ADDRESSINGMODES::direct, 5, 2},
		{"ORA", ADDRESSINGMODES::direct_indirect_long, 6, 2},
		{"PHP", ADDRESSINGMODES::stack, 3, 1},
		{"ORA", ADDRESSINGMODES::immediate, 2, 2},
		{"ASL", ADDRESSINGMODES::accumulator, 2, 1},
		{"PHD", ADDRESSINGMODES::stack, 4, 1},
		{"TSB", ADDRESSINGMODES::absolute, 6, 3},
		{"ORA", ADDRESSINGMODES::absolute, 4, 3},
		{"ASL", ADDRESSINGMODES::absolute, 6, 3},
		{"ORA", ADDRESSINGMODES::absolute_long, 5, 4},

		// 0x10 - 0x1f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2},
		{"TRB", ADDRESSINGMODES::direct, 5, 2},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2},
		{"CLC", ADDRESSINGMODES::implied, 2, 1},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1},
		{"TCS", ADDRESSINGMODES::implied, 2, 1},
		{"TRB", ADDRESSINGMODES::absolute, 6, 3},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4},
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

	Register8 data_in;
	Register8 data_out;

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

	uint8_t instruction_cycles;
	uint16_t fetched;
	uint32_t reset_low_cycles;
	bool reset_low;

	uint64_t clock_count;

protected:

public:
	W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system);
	~W65C816S();

	// Status register functions

	inline void SetC() { P |= (Register8)STATUSBITS::C; }
	inline void ClearC() { P &= ~(Register8)STATUSBITS::C; }
	inline bool GetC() { return ((P & (Register8)STATUSBITS::C) ? true : false); }

	inline void SetE() { EP |= (Register8)STATUSBITS::E; mode = MODE::EMULATION; SetM(); SetX(); }
	inline void ClearE() { EP &= ~(Register8)STATUSBITS::E; mode = MODE::NATIVE; }
	inline bool GetE() { return ((EP & (Register8)STATUSBITS::E) ? true : false); }

	inline void SetZ() { P |= (Register8)STATUSBITS::Z; }
	inline void ClearZ() { P &= ~(Register8)STATUSBITS::Z; }
	inline bool GetZ() { return ((P & (Register8)STATUSBITS::Z) ? true : false); }

	inline void SetI() { P |= (Register8)STATUSBITS::I; }
	inline void ClearI() { P &= ~(Register8)STATUSBITS::I; }
	inline bool GetI() { return ((P & (Register8)STATUSBITS::I) ? true : false); }

	inline void SetD() { P |= (Register8)STATUSBITS::D; }
	inline void ClearD() { P &= ~(Register8)STATUSBITS::D; }
	inline bool GetD() { return ((P & (Register8)STATUSBITS::D) ? true : false); }

	inline void SetX() { P |= (Register8)STATUSBITS::X; X.b8_15 = 0x00; Y.b8_15 = 0x00; }
	inline void ClearX() { if (!GetE()) P &= ~(Register8)STATUSBITS::X; }
	inline bool GetX() { return ((P & (Register8)STATUSBITS::X) ? true : false); }

	inline void SetB() { EP |= (Register8)STATUSBITS::B; }
	inline void ClearB() { EP &= ~(Register8)STATUSBITS::B; }
	inline bool GetB() { return ((EP & (Register8)STATUSBITS::B) ? true : false); }

	inline void SetM() { P |= (Register8)STATUSBITS::M; }
	inline void ClearM() { if (!GetE()) P &= ~(Register8)STATUSBITS::M; }
	inline bool GetM() { return ((P & (Register8)STATUSBITS::M) ? true : false); }

	inline void SetV() { P |= (Register8)STATUSBITS::V; }
	inline void ClearV() { P &= ~(Register8)STATUSBITS::V; }
	inline bool GetV() { return ((P & (Register8)STATUSBITS::V) ? true : false); }

	inline void SetN() { P |= (Register8)STATUSBITS::N; }
	inline void ClearN() { P &= ~(Register8)STATUSBITS::N; }
	inline bool GetN() { return ((P & (Register8)STATUSBITS::N) ? true : false); }

	// CPU functions

	void Reset();

	void Clock();

	void Run();
	void Start();
	void Stop();

	// Opcode functions

	inline void ADC_IMM_8(uint8_t immediate)
	{
		uint8_t PA = A.b0_7;

		A.b0_7 += immediate + GetC();

		if (A.b0_7 < PA)
			SetC();
	}

	inline void ADC_IMM_16(uint16_t immediate)
	{
		uint8_t PA = A.db0_15;

		A.db0_15 += immediate + GetC();

		if (A.db0_15 < PA)
			SetC();
	}

	inline void CLC() { ClearC(); }
	inline void SEC() { SetC(); }
	inline void XCE() { GetC() ? SetE() : ClearE(); }
};