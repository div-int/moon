#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <bitSet>

#include "bus.h"

#include "olcPixelGameEngine.h"

class Ram : public BusDevice
{
public:
	typedef std::shared_ptr<Ram> SharedPtr;

private:
	olc::PixelGameEngine* system;
	uint32_t startAddress;
	uint32_t endAddress;

	std::unique_ptr<uint8_t[]> RAM; // pointer to memory storage


public:
	Ram(olc::PixelGameEngine* system, uint32_t startAddress, uint32_t endAddress);
	~Ram();

	void Reset();

	bool ValidWrite(uint32_t address) override;
	bool ValidRead(uint32_t address) override;
	void Write(uint32_t address, uint8_t data) override;
	uint8_t Read(uint32_t address) override;

	void Debug();
};