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
		void(W65C816S::* function)(void* opcode);
	} OPCODE;

	OPCODE opcodes[256] = {

		// 0x00 - 0x0f

		{"BRK", ADDRESSINGMODES::stack, 7, 2, &W65C816S::BRK},
		{"ORA", ADDRESSINGMODES::direct_indexed_indirect, 6, 2, &W65C816S::ORA},
		{"COP", ADDRESSINGMODES::stack, 7, 2, &W65C816S::COP},
		{"ORA", ADDRESSINGMODES::stack_relative, 4, 2, &W65C816S::ORA},
		{"TSB", ADDRESSINGMODES::direct, 5, 2, &W65C816S::TSB},
		{"ORA", ADDRESSINGMODES::direct, 3, 2, &W65C816S::ORA},
		{"ASL", ADDRESSINGMODES::direct, 5, 2, &W65C816S::ASL},
		{"ORA", ADDRESSINGMODES::direct_indirect_long, 6, 2, &W65C816S::ORA},
		{"PHP", ADDRESSINGMODES::stack, 3, 1, &W65C816S::PHP},
		{"ORA", ADDRESSINGMODES::immediate, 2, 2, &W65C816S::ORA},
		{"ASL", ADDRESSINGMODES::accumulator, 2, 1, &W65C816S::ASL },
		{"PHD", ADDRESSINGMODES::stack, 4, 1, &W65C816S::PHD},
		{"TSB", ADDRESSINGMODES::absolute, 6, 3, &W65C816S::TSB},
		{"ORA", ADDRESSINGMODES::absolute, 4, 3, &W65C816S::ORA},
		{"ASL", ADDRESSINGMODES::absolute, 6, 3, &W65C816S::ASL},
		{"ORA", ADDRESSINGMODES::absolute_long, 5, 4, &W65C816S::ORA},

		// 0x10 - 0x1f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, &W65C816S::BPL},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, &W65C816S::ORA},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2, &W65C816S::ORA},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, &W65C816S::ORA},
		{"TRB", ADDRESSINGMODES::direct, 5, 2, &W65C816S::TRB},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, &W65C816S::ORA},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, &W65C816S::ASL},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, &W65C816S::ORA},
		{"CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, &W65C816S::ORA},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1, &W65C816S::INC},
		{"TCS", ADDRESSINGMODES::implied, 2, 1, &W65C816S::TCS},
		{"TRB", ADDRESSINGMODES::absolute, 6, 3, &W65C816S::TRB},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, &W65C816S::ORA},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, &W65C816S::ASL},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, &W65C816S::ORA},

		// 0x20 - 0x2f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr},
		{"TRB", ADDRESSINGMODES::direct, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr},
		{"CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr},
		{"TCS", ADDRESSINGMODES::implied, 2, 1, nullptr},
		{"TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr},

		// 0x30 - 0x3f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr},
		{"TRB", ADDRESSINGMODES::direct, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr},
		{"SEC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::SEC },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr},
		{"TCS", ADDRESSINGMODES::implied, 2, 1, nullptr},
		{"TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr},

		// 0x40 - 0x4f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr},
		{"TRB", ADDRESSINGMODES::direct, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr},
		{"CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr},
		{"TCS", ADDRESSINGMODES::implied, 2, 1, nullptr},
		{"JMP", ADDRESSINGMODES::absolute, 3, 3, &W65C816S::JMP},
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr},

		// 0x50 - 0x5f

		{"BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr},
		{"TRB", ADDRESSINGMODES::direct, 5, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr},
		{"ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr},
		{"ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr},
		{"CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr},
		{"INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr},
		{"TCS", ADDRESSINGMODES::implied, 2, 1, nullptr},
		{"JMP", ADDRESSINGMODES::absolute_long, 4, 4, &W65C816S::JMP },
		{"ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr},
		{"ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr},
		{"ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr},

		// 0x60 - 0x6f

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ADC", ADDRESSINGMODES::immediate, 2, 2, &W65C816S::ADC },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0x70 - 0x7f

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0x80 - 0x8f

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "DEY", ADDRESSINGMODES::implied, 2, 1, &W65C816S::DEY },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "STA", ADDRESSINGMODES::absolute, 4, 3, &W65C816S::STA },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0x90 - 0x9f

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xa0 - 0xaf

		{ "LDY", ADDRESSINGMODES::immediate, 2, 2, &W65C816S::LDY },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "LDX", ADDRESSINGMODES::immediate, 2, 2, &W65C816S::LDA },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "LDA", ADDRESSINGMODES::immediate, 2, 2, &W65C816S::LDA },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xb0 - 0xbf

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xc0 - 0xcf

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "REP", ADDRESSINGMODES::immediate, 3, 2, &W65C816S::REP },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "INY", ADDRESSINGMODES::implied, 2, 1, &W65C816S::INY },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "DEX", ADDRESSINGMODES::implied, 2, 1, &W65C816S::DEX },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xd0 - 0xdf

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "STP", ADDRESSINGMODES::implied, 3, 1, &W65C816S::STP },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xe0 - 0xef

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "INX", ADDRESSINGMODES::implied, 2, 1, &W65C816S::INX },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "TCS", ADDRESSINGMODES::implied, 2, 1, nullptr },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },

		// 0xf0 - 0xff

		{ "BPL", ADDRESSINGMODES::program_counter_relative, 2, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_indexed, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::stack_relative_indirect_indexed, 7, 2, nullptr },
		{ "TRB", ADDRESSINGMODES::direct, 5, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indexed_with_x, 4, 2, nullptr },
		{ "ASL", ADDRESSINGMODES::direct_indexed_with_x, 6, 2, nullptr },
		{ "ORA", ADDRESSINGMODES::direct_indirect_long_indexed, 6, 2, nullptr },
		{ "CLC", ADDRESSINGMODES::implied, 2, 1, &W65C816S::CLC },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_y, 4, 3, nullptr },
		{ "INC", ADDRESSINGMODES::accumulator, 2, 1, nullptr },
		{ "XCE", ADDRESSINGMODES::implied, 2, 1, &W65C816S::XCE },
		{ "TRB", ADDRESSINGMODES::absolute, 6, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_indexed_with_x, 4, 3, nullptr },
		{ "ASL", ADDRESSINGMODES::absolute_indexed_with_x, 7, 3, nullptr },
		{ "ORA", ADDRESSINGMODES::absolute_long_indexed, 5, 4, nullptr },
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

	Register24 address_out;
	Register8 data_in;
	Register8 data_out;

	Register16 immediate_data;

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

	uint32_t reset_low_cycles;
	bool reset_low;

	bool stp, wai;

	uint64_t clock_count;
	std::thread thread_run;

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

	// Debug functions

	void W65C816S::Debug();

	// Bus in/out function helper

	inline void VPB_MLB_VDA_VPA_RWB(uint1_t vpb, uint1_t mlb, uint1_t vda, uint1_t vpa, uint1_t rwb)
	{
		*VPB = vpb;
		*MLB = mlb;
		*VDA = vda;
		*VPA = vpa;
		*RWB = rwb;
	}

	// Opcode functions

	void ADC(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				if (GetM()) // 8 bit
				{
					Register8 PA = A.b0_7;

					A.b0_7 += data_in;

					if (A.b0_7 == 0x00) SetZ();
					if (A.b0_7 < PA) SetC();

					instruction_cycles = 0;
				}
				else // 16 bit
				{
					immediate_data.db0_15 = data_in;
					++instruction_cycles;
				}
				break;
			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				Register16 PA = A;

				immediate_data.b8_15 = data_in;

				A.db0_15 += immediate_data.db0_15;

				if (A.db0_15 == 0x0000) SetZ();
				if (A.db0_15 < PA.db0_15) SetC();

				instruction_cycles = 0;
				break;
			}
			break;
		}
	}

	void AND(void* opcode) {}
	void ASL(void* opcode) {}
	void BCC(void* opcode) {}
	void BCS(void* opcode) {}
	void BEQ(void* opcode) {}
	void BIT(void* opcode) {}
	void BMI(void* opcode) {}
	void BNE(void* opcode) {}
	void BPL(void* opcode) {}
	void BRA(void* opcode) {}
	void BRK(void* opcode) {}
	void BRL(void* opcode) {}
	void BVC(void* opcode) {}
	void BVS(void* opcode) {}

	void CLC(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			ClearC();

			instruction_cycles = 0;
			break;
		}
	}

	void CLD(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			ClearD();

			instruction_cycles = 0;
			break;
		}
	}

	void CLI(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			ClearI();

			instruction_cycles = 0;
			break;
		}
	}

	void CLV(void* opcode) {}
	void CMP(void* opcode) {}
	void COP(void* opcode) {}
	void CPX(void* opcode) {}
	void CPY(void* opcode) {}
	void DEC(void* opcode) {}

	void DEX(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				--X.b0_7;

				if (X.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (X.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				--X.db0_15;

				if (X.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (X.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void DEY(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				--Y.b0_7;

				if (Y.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (Y.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				--Y.db0_15;

				if (X.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (Y.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}


	void EOR(void* opcode) {}
	void INC(void* opcode) {}

	void INX(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				++X.b0_7;

				if (X.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (X.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				++X.db0_15;

				if (X.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (X.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void INY(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				++Y.b0_7;

				if (Y.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (Y.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				++Y.db0_15;

				if (Y.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (Y.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void JML(void* opcode) {}

	void JMP(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::absolute:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b0_7 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b8_15 = data_in;

				PC.db0_15 = immediate_data.db0_15;
				address_out = PC;

				instruction_cycles = 0;
				break;
			}
			break;

		case ADDRESSINGMODES::absolute_long:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b0_7 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b8_15 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 3:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				PC.db0_15 = immediate_data.db0_15;
				PC.b16_23 = data_in;
				address_out = PC;

				instruction_cycles = 0;
				break;
			}
			break;

		}
	}

	void JSL(void* opcode) {}
	void JSR(void* opcode) {}

	void LDA(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				if (GetM()) // 8 bit
				{
					A.b0_7 = data_in;

					if (A.b0_7 == 0x00) SetZ();
					if (A.b0_7 & 0x80) SetN();

					instruction_cycles = 0;
				}
				else // 16 bit
				{
					immediate_data.db0_15 = data_in;
					++instruction_cycles;
				}
				break;
			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				immediate_data.b8_15 = data_in;

				A.db0_15 += immediate_data.db0_15;

				if (A.db0_15 == 0x0000) SetZ();
				if (A.db0_15 & 0x8000) SetN();

				instruction_cycles = 0;
				break;
			}
			break;
		}
	}

	void LDX(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				if (GetM()) // 8 bit
				{
					X.b0_7 = data_in;

					if (X.b0_7 == 0x00) SetZ();
					if (X.b0_7 & 0x80) SetN();

					instruction_cycles = 0;
				}
				else // 16 bit
				{
					immediate_data.db0_15 = data_in;
					++instruction_cycles;
				}
				break;
			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				immediate_data.b8_15 = data_in;

				X.db0_15 += immediate_data.db0_15;

				if (X.db0_15 == 0x0000) SetZ();
				if (X.db0_15 & 0x8000) SetN();

				instruction_cycles = 0;
				break;
			}
			break;
		}
	}

	void LDY(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				if (GetM()) // 8 bit
				{
					Y.b0_7 = data_in;

					if (Y.b0_7 == 0x00) SetZ();
					if (Y.b0_7 & 0x80) SetN();

					instruction_cycles = 0;
				}
				else // 16 bit
				{
					immediate_data.db0_15 = data_in;
					++instruction_cycles;
				}
				break;
			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				immediate_data.b8_15 = data_in;

				Y.db0_15 += immediate_data.db0_15;

				if (Y.db0_15 == 0x0000) SetZ();
				if (Y.db0_15 & 0x8000) SetN();

				instruction_cycles = 0;
				break;
			}
			break;
		}
	}

	void LSR(void* opcode) {}
	void MVN(void* opcode) {}
	void MVP(void* opcode) {}

	void NOP(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			instruction_cycles = 0;
			break;
		}
	}

	void ORA(void* opcode) {}
	void PEA(void* opcode) {}
	void PEI(void* opcode) {}
	void PER(void* opcode) {}
	void PHA(void* opcode) {}
	void PHB(void* opcode) {}
	void PHD(void* opcode) {}
	void PHK(void* opcode) {}
	void PHP(void* opcode) {}
	void PHX(void* opcode) {}
	void PHY(void* opcode) {}
	void PLA(void* opcode) {}
	void PLB(void* opcode) {}
	void PLD(void* opcode) {}
	void PLX(void* opcode) {}
	void PLY(void* opcode) {}


	void REP(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				P &= ~(data_in);
				instruction_cycles = 0;
				break;
			}
		}
	}

	void ROL(void* opcode) {}
	void ROR(void* opcode) {}
	void RTI(void* opcode) {}
	void RTL(void* opcode) {}
	void RTS(void* opcode) {}
	void SBC(void* opcode) {}

	void SEP(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::immediate:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;
			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;

				P |= data_in;
				instruction_cycles = 0;
				break;
			}
		}
	}

	void SEC(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			SetC();

			instruction_cycles = 0;
			break;
		}
	}

	void SED(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			SetD();

			instruction_cycles = 0;
			break;
		}
	}

	void SEI(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			SetI();

			instruction_cycles = 0;
			break;
		}
	}

	void STA(void* opcode)
	{
		switch (((OPCODE*)opcode)->addressing_mode)
		{
		case ADDRESSINGMODES::absolute:
			switch (instruction_cycles)
			{
			case 0:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 1:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b0_7 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 2:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b1, 0b1);

				immediate_data.b8_15 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 3:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b0, 0b0);

				address_out.db0_15 = immediate_data.db0_15;
				address_out.b16_23 = DBR.b16_23;
				data_out = A.b0_7;

				if (GetM())
					instruction_cycles = 0;
				else
					++instruction_cycles;
				break;

			case 4:
				VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b0, 0b0);

				++immediate_data.db0_15;
				address_out.db0_15 = immediate_data.db0_15;
				address_out.b16_23 = DBR.b16_23;
				data_out = A.b8_15;

				instruction_cycles = 0;
				break;
			}
			break;

		/*case ADDRESSINGMODES::absolute_long:
			switch (instruction_cycles)
			{
			case 0:
				*VPB = 0b1;
				*MLB = 0b1;
				*VDA = 0b1;
				*VPA = 0b1;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 1:
				*VPB = 0b1;
				*MLB = 0b1;
				*VDA = 0b0;
				*VPA = 0b1;

				immediate_data.b0_7 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 2:
				*VPB = 0b1;
				*MLB = 0b1;
				*VDA = 0b0;
				*VPA = 0b1;

				immediate_data.b8_15 = data_in;

				++PC.db0_15;
				address_out = PC;
				++instruction_cycles;

				break;

			case 3:
				*VPB = 0b1;
				*MLB = 0b1;
				*VDA = 0b0;
				*VPA = 0b1;

				PC.db0_15 = immediate_data.db0_15;
				PC.b16_23 = data_in;
				address_out = PC;

				instruction_cycles = 0;
				break;
			}
			break;*/
		}
	}

	void STP(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			++instruction_cycles;

			break;
		case 2:
			*RDY = 0b0;

			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			stp = true;

			instruction_cycles = 0;

			break;
		}
	}

	void STX(void* opcode) {}
	void STY(void* opcode) {}
	void STZ(void* opcode) {}

	void TAX(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				X.b0_7 = A.b0_7;

				if (X.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (X.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				X.db0_15 = A.db0_15;

				if (X.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (X.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void TAY(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				Y.b0_7 = A.b0_7;

				if (Y.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (Y.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				Y.db0_15 = A.db0_15;

				if (Y.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (Y.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void TCD(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			D.db0_15 = A.db0_15;

			if (D.db0_15 == 0x0000)
				SetZ();
			else
				ClearZ();

			if (D.db0_15 & 0x8000)
				SetN();
			else
				ClearN();

			instruction_cycles = 0;
			break;
		}
	}

	void TCS(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			S.db0_15 = A.db0_15;

			instruction_cycles = 0;
			break;
		}
	}

	void TDC(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			A.db0_15 = D.db0_15;

			if (A.db0_15 == 0x0000)
				SetZ();
			else
				ClearZ();

			if (A.db0_15 & 0x8000)
				SetN();
			else
				ClearN();

			instruction_cycles = 0;
			break;
		}
	}

	void TRB(void* opcode) {}
	void TSB(void* opcode) {}

	void TSC(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			A.db0_15 = S.db0_15;

			if (A.db0_15 == 0x0000)
				SetZ();
			else
				ClearZ();

			if (A.db0_15 & 0x8000)
				SetN();
			else
				ClearN();

			instruction_cycles = 0;
			break;
		}
	}

	void TSX(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			X.db0_15 = S.db0_15;

			if (X.db0_15 == 0x0000)
				SetZ();
			else
				ClearZ();

			if (X.db0_15 & 0x8000)
				SetN();
			else
				ClearN();

			instruction_cycles = 0;
			break;
		}
	}

	void TXA(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetM())
			{
				A.b0_7 = X.b0_7;

				if (A.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (A.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				A.db0_15 = X.db0_15;

				if (A.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (A.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void TXS(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			S.db0_15 = X.db0_15;

			instruction_cycles = 0;
			break;
		}
	}

	void TXY(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				Y.b0_7 = X.b0_7;

				if (Y.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (Y.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				Y.db0_15 = X.db0_15;

				if (Y.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (Y.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void TYA(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetM())
			{
				A.b0_7 = Y.b0_7;

				if (A.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (A.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				A.db0_15 = Y.db0_15;

				if (A.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (A.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void TYX(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			if (GetX())
			{
				X.b0_7 = Y.b0_7;

				if (X.b0_7 == 0x00)
					SetZ();
				else
					ClearZ();

				if (X.b0_7 & 0x80)
					SetN();
				else
					ClearN();
			}
			else
			{
				X.db0_15 = Y.db0_15;

				if (X.db0_15 == 0x0000)
					SetZ();
				else
					ClearZ();

				if (X.db0_15 & 0x8000)
					SetN();
				else
					ClearN();
			}

			instruction_cycles = 0;
			break;
		}
	}

	void WAI(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			*RDY = 0b1;
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			++instruction_cycles;

			break;
		case 2:
			*RDY = 0b0;
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);
			wai = true;

			instruction_cycles = 0;

			break;
		}
	}

	void WDM(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			*VPB = 0b1;
			*MLB = 0b1;
			*VDA = 0b0;
			*VPA = 0b0;

			++PC.db0_15;
			address_out = PC;
			instruction_cycles = 0;
			break;
		}
	}

	void XBA(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			Register8 B;

			B = A.b8_15;
			A.b8_15 = A.b0_7;
			A.b0_7 = B;

			if (A.b0_7 == 0x00)
				SetZ();
			else
				ClearZ();

			if (A.b0_7 & 0x80)
				SetN();
			else
				ClearN();

			instruction_cycles = 0;
			break;
		}
	}

	void XCE(void* opcode)
	{
		switch (instruction_cycles)
		{
		case 0:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b1, 0b1, 0b1);

			++PC.db0_15;
			address_out = PC;
			++instruction_cycles;

			break;
		case 1:
			VPB_MLB_VDA_VPA_RWB(0b1, 0b1, 0b0, 0b0, 0b1);

			GetC() ? SetE() : ClearE();

			instruction_cycles = 0;
			break;
		}
	}
};