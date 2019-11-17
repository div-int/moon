﻿// moon.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const int OK = 0;
const int FAIL = -1;

class Moon : public olc::PixelGameEngine
{
private:
	uint32_t pixel_x;
	uint32_t pixel_y;

	uint32_t display_scale;
	uint32_t display_width;
	uint32_t display_height;
	uint32_t display_size;

	olc::Sprite* display_buffer;

	uint32_t pixel_x_0;
	uint32_t pixel_y_0;
	uint32_t pixel_x_1;
	uint32_t pixel_y_1;
	uint32_t pixel_x_2;
	uint32_t pixel_y_2;
	uint32_t pixel_x_3;
	uint32_t pixel_y_3;

	uint32_t pixel_x_start_0;
	uint32_t pixel_y_start_0;
	uint32_t pixel_x_start_1;
	uint32_t pixel_y_start_1;
	uint32_t pixel_x_start_2;
	uint32_t pixel_y_start_2;
	uint32_t pixel_x_start_3;
	uint32_t pixel_y_start_3;

	uint32_t pixel_x_scale_0;
	uint32_t pixel_y_scale_0;
	uint32_t pixel_x_scale_1;
	uint32_t pixel_y_scale_1;
	uint32_t pixel_x_scale_2;
	uint32_t pixel_y_scale_2;
	uint32_t pixel_x_scale_3;
	uint32_t pixel_y_scale_3;

	uint8_t *screen_buffer_0;
	uint8_t *screen_buffer_1;
	uint8_t *screen_buffer_2;
	uint8_t *screen_buffer_3;

	uint8_t screen_buffer_enabled;

protected:

public:
	Moon();
	~Moon();

	bool OnUserCreate() override;
	bool Moon::OnUserDestroy() override;
	bool OnUserUpdate(float fElapsedTime) override;
};
