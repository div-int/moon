#include "rom.h"

Rom::Rom(olc::PixelGameEngine* system, uint32_t startAddress, uint32_t endAddress)
{
	this->system = system;
	this->startAddress = startAddress;
	this->endAddress = endAddress;

	uint32_t length = (endAddress - startAddress) + 1;

	rom = std::make_unique<uint8_t[]>(length);

	for (uint32_t address = startAddress; address <= endAddress; address++)
		Write(address, rand() % 256);
}

Rom::~Rom()
{
}

void Rom::Reset()
{
	for (uint32_t address = startAddress; address <= endAddress; address++)
		rom[address] = rand() % 256;
}

bool Rom::ValidWrite(uint32_t address)
{
	return false;
}

bool Rom::ValidRead(uint32_t address)
{
	if (address >= startAddress && address <= endAddress)
		return true;

	return false;
}

void Rom::Write(uint32_t address, uint8_t data)
{
}

uint8_t Rom::Read(uint32_t address)
{
	/*std::cout << "Rom::Read(";
	std::cout << std::hex << std::setw(4) << std::setfill('0') << address << ", ";
	std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(rom[address - startAddress]) << ")" << std::endl;*/

	return rom[address - startAddress];
}

void Rom::Debug()
{
}