#include "w65c816s.h"

W65C816S::W65C816S(Bus::SharedPtr bus, olc::PixelGameEngine* system)
{
	this->bus = bus;
	this->system = system;

	running = false;

	mode = MODE::EMULATION;

	emulation.IRQ.tb0_23 = 0x00fffe;
	emulation.RESET.tb0_23 = 0x00fffc;
	emulation.NMI.tb0_23 = 0x00fffa;
	emulation.ABORT.tb0_23 = 0x00fff8;
	emulation.BRK.tb0_23 = 0x000000;
	emulation.COP.tb0_23 = 0x00fff4;

	native.IRQ.tb0_23 = 0x00ffee;
	native.RESET.tb0_23 = 0x000000;
	native.NMI.tb0_23 = 0x00ffea;
	native.ABORT.tb0_23 = 0x00ffe8;
	native.BRK.tb0_23 = 0x00ffe6;
	native.COP.tb0_23 = 0x00ffe4;

	ABORTB = bus->CreateLine1Bit("ABORTB", 0b0);
	A0_A15 = bus->CreateLine16Bit("A0_A15", 0b0);
	BE = bus->CreateLine1Bit("BE", 0b0);
	D0_D7 = bus->CreateLine8Bit("D0_D7", 0b0);
	E = bus->CreateLine1Bit("E", 0b0);
	IRQB = bus->CreateLine1Bit("IRQB", 0b0);
	MLB = bus->CreateLine1Bit("MLB", 0b0);
	MX = bus->CreateLine1Bit("MX", 0b0);
	NMIB = bus->CreateLine1Bit("NMIB", 0b0);
	PHI2 = bus->CreateLine1Bit("PHI2", 0b0);
	RWB = bus->CreateLine1Bit("RWB", 0b0);
	RDY = bus->CreateLine1Bit("RDY", 0b0);
	RESB = bus->CreateLine1Bit("RESB", 0b0);
	VDA = bus->CreateLine1Bit("VDA", 0b0);
	VPA = bus->CreateLine1Bit("VPA", 0b0);
	VPB = bus->CreateLine1Bit("VPB", 0b0);

	reset_low_cycles = 0;
	reset_low = false;

	clock_count = 0x0000000000000000;
}

W65C816S::~W65C816S()
{
	// TO DO
}

void W65C816S::Reset()
{
	++reset_low_cycles;

	if (reset_low_cycles >= 2)
	{
		D.tb0_23 = 0x000000;
		DBR.tb0_23 = 0x000000;
		S.b8_15 = 0x01;
		S.b16_23 = 0x00;
		X.b8_15 = 0x00;
		Y.b8_15 = 0x00;

		SetM();
		SetX();
		ClearD();
		SetI();
		SetE();

		*E = 0x1;
		*MX = 0x1;
		*RWB = 0x1;
		*VDA = 0x0;
		*VPB = 0x1;
		*VPA = 0x0;
	}
}

void W65C816S::Clock()
{
	++clock_count;

	if (*RESB == 0x00)
	{
		reset_low = true;
		Reset();
		return;
	}
	else
	{
		if (reset_low)
		{
			/* When Reset is brought high, an interrupt sequence is initiated
				STP and WAI instructions are cleared
				RWB remains in the high state during the stack address cycles.
				The Reset vector address is 00FFFC,D.(see Table 6-1 for Vectors)
				PC is loaded with the contents of 00FFFC,D */

			PC = emulation.RESET;
			instruction_cycles = 0;

			reset_low = false;

			return;
		}
	}

	if (instruction_cycles == 0)
	{
		std::cout << "Fetched : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(data_in) << std::endl;

		IR = data_in;

		++PC.db0_15;
		++instruction_cycles;
		*VDA = 0b1;
		*VPA = 0b1;
		*VPB = 0b1;

		return;
	}

	switch (mode)
	{
	case MODE::EMULATION:
		switch (instruction_cycles)
		{
		case 1:
			++instruction_cycles;

			switch (IR)
			{
			case 0x18: // CLC
				std::cout << "CLC" << std::endl;
				CLC();
				instruction_cycles = 0;
				break;
			case 0x38: // SEC
				std::cout << "SEC" << std::endl;
				SEC();
				instruction_cycles = 0;
				break;
			case 0x69: // ADC #immediate
				*VDA = 0b0;
				++PC.db0_15;
				fetched = data_in;
				break;
			case 0xfb: // XCE
				std::cout << "XCE" << std::endl;
				XCE();
				instruction_cycles = 0;
				break;
			}
		case 2:
			switch (IR)
			{
			case 0x69:
				std::cout << "ADC #" << std::hex << std::setw(2) << std::setfill('0') << unsigned(data_in) << std::endl;
				ADC_IMM_8(data_in);
				instruction_cycles = 0;

				break;
			}
		}
		break;

	case MODE::NATIVE:
		switch (instruction_cycles)
		{
		case 1:
			++instruction_cycles;

			switch (IR)
			{
			case 0x69: //  // immediate fetch low byte
				*VDA = 0b0;
				++PC.db0_15;
				fetched = data_in;

				break;
			}
			break;

		case 2:
			++instruction_cycles;

			switch (IR)
			{
			case 0x69: // immediate fetch high byte
				*VDA = 0b0;

				if (GetM() == 0b1) // if using 8 bit accumulator register
				{
					std::cout << "ADC #" << std::hex << std::setw(2) << std::setfill('0') << unsigned(fetched) << std::endl;
					ADC_IMM_8(fetched);
					instruction_cycles = 0;
				}
				else
				{
					++PC.db0_15;
					fetched |= ((uint16_t)data_in) << 8;
				}

				break;
			}
			break;

		case 3:
			++instruction_cycles;

			switch (IR)
			{
			case 0x69: // ADC #immediate_16
				std::cout << "ADC #" << std::hex << std::setw(4) << std::setfill('0') << unsigned(fetched) << std::endl;
				ADC_IMM_16(fetched);
				instruction_cycles = 0;
			}
		}
		break;
	}
}

void W65C816S::Run()
{
	while (running) // continue to keep running while running is true
	{
		// Put PC onto the address bus and increase it by 1

		*A0_A15 = PC.db0_15;	// put 16 bit program counter onto address bus
		*D0_D7 = PC.b16_23;		// put 8 bit program bank register onto data bus

		// wait while PHI2 is high

		while (*PHI2 != 0b0) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
		}

		// wait while PHI2 is low

		while (*PHI2 == 0b0) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
		}

		/*if (*RWB == 0b0)
			*D0_D7 = data_out;*/
		if (*RWB == 0b1)
			data_in = *D0_D7;

		std::cout << "W65C816S::Run() : ";
		std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(PC.b16_23) << "/" << std::setw(4) << PC.db0_15;
		std::cout << " => " << std::hex << std::setw(2) << std::setfill('0') << unsigned(data_in);
		std::cout << " <= " << std::hex << std::setw(2) << std::setfill('0') << unsigned(data_out);
		std::cout << " RWB=" << unsigned(*RWB);
		std::cout << std::endl;

		Clock();
	}
}

void W65C816S::Start()
{
	if (running == false)
	{
		running = true;

		std::thread thread_start(&W65C816S::Run, this);
		thread_start.detach();
	}
}

void W65C816S::Stop()
{
	running = false;
}
