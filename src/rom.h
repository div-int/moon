#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <bitSet>

#include "bus.h"

#include "olcPixelGameEngine.h"

class Rom : public BusDevice
{
public:
	typedef std::shared_ptr<Rom> SharedPtr;

private:
	olc::PixelGameEngine* system;
	uint32_t startAddress;
	uint32_t endAddress;

	std::unique_ptr<uint8_t[]> rom; // pointer to memory storage


public:
	Rom(olc::PixelGameEngine* system, uint32_t startAddress, uint32_t endAddress);
	~Rom();

	void Reset();

	bool ValidWrite(uint32_t address) override;
	bool ValidRead(uint32_t address) override;
	void Write(uint32_t address, uint8_t data) override;
	uint8_t Read(uint32_t address) override;

	void Debug();
};