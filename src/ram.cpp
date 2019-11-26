#include "ram.h"

Ram::Ram(olc::PixelGameEngine* system, uint32_t startAddress, uint32_t endAddress)
{
	this->system = system;
	this->startAddress = startAddress;
	this->endAddress = endAddress;

	uint32_t length = (endAddress - startAddress) + 1;

	RAM = std::make_unique<uint8_t[]>(length);

	for (uint32_t address = startAddress; address <= endAddress; address++)
		Write(address, rand() % 256);
}

Ram::~Ram()
{
}

void Ram::Reset()
{
	for (uint32_t address = startAddress; address <= endAddress; address++)
		RAM[address] = rand() % 256;
}

bool Ram::ValidWrite(uint32_t address)
{
	if (address >= startAddress && address <= endAddress)
		return true;

	return false;
}

bool Ram::ValidRead(uint32_t address)
{
	if (address >= startAddress && address <= endAddress)
		return true;

	return false;
}

void Ram::Write(uint32_t address, uint8_t data)
{
	/*std::cout << "Ram::Write(";
	std::cout << std::hex << std::setw(6) << std::setfill('0') << address << ", ";
	std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(data) << ")" << std::endl;*/

	RAM[address - startAddress] = data;
}

uint8_t Ram::Read(uint32_t address)
{
	return RAM[address - startAddress];
}

void Ram::Debug()
{
}