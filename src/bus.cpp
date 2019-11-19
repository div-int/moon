#include "bus.h"

Bus::Bus(olc::PixelGameEngine* system)
{
	this->system = system;
}

Bus::~Bus()
{
	// TO DO
}

void Bus::AddDevice(std::shared_ptr<BusDevice> busDevice)
{
	busDevices.push_back(busDevice);
}

std::shared_ptr<uint1_t> Bus::CreateLine1Bit(std::string name, uint1_t value)
{
	std::shared_ptr<uint1_t> valuePtr = std::make_shared<uint1_t>(value);
	lines1Bit[name] = valuePtr;

	return valuePtr;

}

std::shared_ptr<uint1_t> Bus::AttachLine1Bit(std::string name)
{
	return lines1Bit.find(name)->second;
}

std::shared_ptr<uint8_t> Bus::CreateLine8Bit(std::string name, uint8_t value)
{
	std::shared_ptr<uint8_t> valuePtr = std::make_shared<uint8_t>(value);
	lines8Bit[name] = valuePtr;

	return valuePtr;

}

std::shared_ptr<uint8_t> Bus::AttachLine8Bit(std::string name)
{
	return lines8Bit.find(name)->second;
}

std::shared_ptr<uint16_t> Bus::CreateLine16Bit(std::string name, uint16_t value)
{
	std::shared_ptr<uint16_t> valuePtr = std::make_shared<uint16_t>(value);
	lines16Bit[name] = valuePtr;

	return valuePtr;
}

std::shared_ptr<uint16_t> Bus::AttachLine16Bit(std::string name)
{
	return lines16Bit.find(name)->second;
}

std::shared_ptr<uint32_t> Bus::CreateLine32Bit(std::string name, uint32_t value)
{
	std::shared_ptr<uint32_t> valuePtr = std::make_shared<uint32_t>(value);
	lines32Bit[name] = valuePtr;

	return valuePtr;
}

std::shared_ptr<uint32_t> Bus::AttachLine32Bit(std::string name)
{
	return lines32Bit.find(name)->second;
}


void Bus::Write(uint32_t address, uint32_t data)
{
	for (auto const& busDevice : busDevices)
	{
		if (busDevice->ValidWrite(address))
			return busDevice->Write(address, data);
	}
}

uint32_t Bus::Read(uint32_t address)
{
	for (const auto& busDevice : busDevices)
	{
		if (busDevice->ValidRead(address))
			return busDevice->Read(address);
	}

	return 0x12345678;
}

void Bus::Debug()
{
	using namespace std;

	ostringstream stringStream;
	int col = 0;

	stringStream << setfill('0');

	for (const auto& line : lines1Bit)
	{
		stringStream << left << setfill(' ') << setw(6) << line.first << ": 0b" << bitset<1>(*line.second) << "  ";

		if ((++col) % 4 == 0)
		{
			stringStream << endl;
			col = 0;
		}
		else
		{
			stringStream << " ";
		}
	}

	stringStream << endl;

	if (col != 0)
	{
		stringStream << endl;
		col = 0;
	}

	for (const auto& line : lines8Bit)
	{
		stringStream << left << setfill(' ') << setw(6) << line.first << ": " << right << dec << setw(3) << unsigned(*line.second) << " " << setfill('0') << hex << "0x" << setw(2) << unsigned(*line.second) << " 0b" << bitset<8>(*line.second);

		if ((++col) % 2 == 0)
		{
			stringStream << endl;
		}
		else
		{
			stringStream << " ";
		}
	}

	stringStream << endl;

	if (col != 0)
	{
		stringStream << endl;
		col = 0;
	}

	for (const auto& line : lines16Bit)
	{
		stringStream << left << setfill(' ') << setw(6) << line.first << ": " << right << dec << setw(3) << unsigned(*line.second) << " \t" << setfill('0') << hex << "0x" << setw(4) << unsigned(*line.second) << " 0b" << bitset<16>(*line.second) << endl;
	}

	stringStream << endl;

	if (col != 0)
	{
		stringStream << endl;
		col = 0;
	}

	for (const auto& line : lines32Bit)
	{
		stringStream << left << setfill(' ') << setw(6) << line.first << ": " << right << dec << setw(3) << unsigned(*line.second) << " \t" << setfill('0') << hex << "0x" << setw(8) << unsigned(*line.second) << " 0b" << bitset<32>(*line.second) << endl;
	}

	stringStream << endl;

	if (col != 0)
	{
		stringStream << endl;
		col = 0;
	}

	system->DrawString(9, 9 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 7 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(9, 7 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 9 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(7, 8 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(9, 8 + (0 * 8), stringStream.str(), olc::BLACK, 1);
	system->DrawString(8, 8 + (0 * 8), stringStream.str(), olc::WHITE, 1);
}
