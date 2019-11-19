#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <bitSet>

#include "olcPixelGameEngine.h"

typedef unsigned char uint1_t;

class BusDevice
{
public:
	virtual bool ValidWrite(uint32_t address) = 0;
	virtual bool ValidRead(uint32_t address) = 0;
	virtual void Write(uint32_t address, uint32_t data) = 0;
	virtual uint32_t Read(uint32_t address) = 0;
};

class Bus
{
public:
	typedef std::shared_ptr<Bus> SharedPtr;
	typedef std::shared_ptr<uint1_t> Line1Bit;
	typedef std::shared_ptr<uint8_t> Line8Bit;
	typedef std::shared_ptr<uint16_t> Line16Bit;
	typedef std::shared_ptr<uint32_t> Line32Bit;

private:
	olc::PixelGameEngine* system;

	std::vector<std::shared_ptr<BusDevice>> busDevices;
	std::map<std::string, Line1Bit> lines1Bit;
	std::map<std::string, Line8Bit> lines8Bit;
	std::map<std::string, Line16Bit> lines16Bit;
	std::map<std::string, Line32Bit> lines32Bit;

public:
	Bus(olc::PixelGameEngine* system);
	~Bus();

	void AddDevice(std::shared_ptr<BusDevice> busDevice);

	Line1Bit CreateLine1Bit(std::string name, uint1_t value);
	Line1Bit AttachLine1Bit(std::string name);
	Line8Bit CreateLine8Bit(std::string name, uint8_t value);
	Line8Bit AttachLine8Bit(std::string name);
	Line16Bit CreateLine16Bit(std::string name, uint16_t value);
	Line16Bit AttachLine16Bit(std::string name);
	Line32Bit CreateLine32Bit(std::string name, uint32_t value);
	Line32Bit AttachLine32Bit(std::string name);

	void Write(uint32_t address, uint32_t data);
	uint32_t Read(uint32_t address);

	void Debug();
};