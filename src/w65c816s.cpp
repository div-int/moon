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

		A.db0_15 = 0x0000;
		X.db0_15 = 0x0000;
		Y.db0_15 = 0x0000;
	}
}

void W65C816S::Clock()
{
	++clock_count;

	address_out = PC;

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
			address_out = PC;

			instruction_cycles = 0;

			reset_low = false;

			*VPB = 0b1;
			*MLB = 0b1;
			*VDA = 0b1;
			*VPA = 0b1;
			*RWB = 0b1;

			return;
		}
	}

	if (instruction_cycles == 0)
	{
		std::cout << "Fetched : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(data_in) << " : ";

		IR = data_in;

		std::cout << opcodes[IR].mnemonic << " (" << addressing_modes[(int)opcodes[IR].addressing_mode].description << ")" << std::endl;

		if (opcodes[IR].function != nullptr)
			(this->*(opcodes[IR].function))((void*)&opcodes[IR]);
		else
			std::cout << "function not defined" << std::endl;

		return;
	}
	else
	{
		std::cout << "Executing : " << std::hex << std::setw(2) << std::setfill('0') << unsigned(IR) << " : " << std::dec << unsigned(instruction_cycles) << " : ";
		std::cout << opcodes[IR].mnemonic << " (" << addressing_modes[(int)opcodes[IR].addressing_mode].description << ")" << std::endl;

		if (opcodes[IR].function != nullptr)
			(this->*(opcodes[IR].function))((void*)&opcodes[IR]);
		else
			std::cout << "function not defined" << std::endl;

	}
}

void W65C816S::Run()
{
	while (running) // continue to keep running while running is true
	{
		// Put the required address valus onto the address bus and data bus

		*A0_A15 = address_out.db0_15;	// put lower 16 bits of address onto address bus
		*D0_D7 = address_out.b16_23;		// put highest 8 bits of address onto data bus

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

		thread_run = std::thread(&W65C816S::Run, this);
		thread_run.detach();
	}
}

void W65C816S::Stop()
{
	running = false;
	
	if (thread_run.joinable())
		thread_run.join();
}

void W65C816S::Debug()
{
	using namespace std;

	ostringstream stringStream;
	int col = 0;

	stringStream << setfill('0');

	stringStream << "PC : " << hex << setw(2) << setfill('0') << unsigned(PC.b16_23) << ":" << hex << setw(4) << setfill('0') << PC.db0_15 << " \t";
	stringStream << "A  : " << dec << setw(5) << setfill(' ') << A.db0_15 << " 0x" << hex << setw(4) << setfill('0') << A.db0_15 << " \t";
	stringStream << "X  : " << dec << setw(5) << setfill(' ') << X.db0_15 << " 0x" << hex << setw(4) << setfill('0') << X.db0_15 << " \t";
	stringStream << "Y  : " << dec << setw(5) << setfill(' ') << Y.db0_15 << " 0x" << hex << setw(4) << setfill('0') << Y.db0_15 << " \t";

	system->DrawString(9, 9 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 7 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(9, 7 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 9 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 8 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(9, 8 + (9 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(8, 8 + (9 * 8), stringStream.str(), olc::WHITE, 1);
}

