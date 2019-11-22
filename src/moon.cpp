// moon.cpp : Defines the entry point for the application.
//

#include "moon.h"

Moon::Moon()
{
	sAppName = "Moon";

	bus = std::make_shared<Bus>(this);
	cpu = std::make_shared<W65C816S>(bus, this);

	PHI2 = bus->AttachLine1Bit("PHI2");
	RESB = bus->AttachLine1Bit("RESB");
}

Moon::~Moon()
{
	// TO DO
}

bool Moon::OnUserCreate()
{
	cpu->Start();
	bus->Start();

	pixel_x = 0;
	pixel_y = 0;

	display_scale = 1;
	display_width = ScreenWidth() / display_scale;
	display_height = ScreenHeight() / display_scale;
	display_size = display_width * display_height;

	display_buffer = new olc::Sprite(display_width, display_height);

	screen_buffer_0 = new uint8_t[1024 * 1024];
	screen_buffer_1 = new uint8_t[1024 * 1024];
	screen_buffer_2 = new uint8_t[1024 * 1024];
	screen_buffer_3 = new uint8_t[1024 * 1024];

	sprite_buffer = new uint8_t[1024 * 1024];

	for (uint32_t i = 0; i < (1024 * 1024); i++)
	{
		screen_buffer_0[i] = (((i / (32 * 1024)) + (i / 32)) % 2) * rand();
		screen_buffer_1[i] = (((i / (16 * 1024)) + (i / 16)) % 2) * rand();
		screen_buffer_2[i] = (((i / (8 * 1024)) + (i / 8)) % 2) * rand();
		screen_buffer_3[i] = (((i / (4 * 1024)) + (i / 4)) % 2) * rand();
	}

	pixel_x_start_0 = 0;
	pixel_y_start_0 = 0;
	pixel_x_start_1 = 0;
	pixel_y_start_1 = 0;
	pixel_x_start_2 = 0;
	pixel_y_start_2 = 0;
	pixel_x_start_3 = 0;
	pixel_y_start_3 = 0;

	pixel_x_scale_0 = 0x10000;
	pixel_y_scale_0 = 0x10000;
	pixel_x_scale_1 = 0x10000;
	pixel_y_scale_1 = 0x10000;
	pixel_x_scale_2 = 0x10000;
	pixel_y_scale_2 = 0x10000;
	pixel_x_scale_3 = 0x10000;
	pixel_y_scale_3 = 0x10000;

	screen_buffer_enabled = 0b1111;

	return true;
}

bool Moon::OnUserDestroy()
{
	// TO DO

	return true;
}

bool Moon::OnUserUpdate(float fElapsedTime)
{
	SetDrawTarget(display_buffer);

	pixel_x = 0;
	pixel_y = 0;

	pixel_x_start_0 = (-GetMouseX() << 15) & 0x3ffffff;
	pixel_y_start_0 = (-GetMouseY() << 15) & 0x3ffffff;

	pixel_x_start_1 = (-GetMouseX() << 14) & 0x3ffffff;
	pixel_y_start_1 = (-GetMouseY() << 14) & 0x3ffffff;

	pixel_x_start_2 = (-GetMouseX() << 13) & 0x3ffffff;
	pixel_y_start_2 = (-GetMouseY() << 13) & 0x3ffffff;

	pixel_x_start_3 = (-GetMouseX() << 12) & 0x3ffffff;
	pixel_y_start_3 = (-GetMouseY() << 12) & 0x3ffffff;

	pixel_x_0 = pixel_x_start_0 & 0x3ffffff;
	pixel_y_0 = pixel_y_start_0 & 0x3ffffff;

	pixel_x_1 = pixel_x_start_1 & 0x3ffffff;
	pixel_y_1 = pixel_y_start_1 & 0x3ffffff;

	pixel_x_2 = pixel_x_start_2 & 0x3ffffff;
	pixel_y_2 = pixel_y_start_2 & 0x3ffffff;

	pixel_x_3 = pixel_x_start_3 & 0x3ffffff;
	pixel_y_3 = pixel_y_start_3 & 0x3ffffff;

	if (GetKey(olc::Key::SPACE).bReleased)
		*PHI2 = ~(*PHI2);

	if (GetKey(olc::Key::Q).bReleased)
		cpu->Stop();

	if (GetKey(olc::Key::S).bReleased)
		cpu->Start();

	if (GetKey(olc::Key::R).bHeld)
		*RESB = 0b0;
	else
		*RESB = 0b1;

	if (GetKey(olc::Key::ESCAPE).bReleased)
		return false;

	if (GetKey(olc::Key::K1).bPressed)
		screen_buffer_enabled ^= 0b0001;
	if (GetKey(olc::Key::K2).bPressed)
		screen_buffer_enabled ^= 0b0010;
	if (GetKey(olc::Key::K3).bPressed)
		screen_buffer_enabled ^= 0b0100;
	if (GetKey(olc::Key::K4).bPressed)
		screen_buffer_enabled ^= 0b1000;

	if (GetKey(olc::Key::RIGHT).bHeld)
	{
		pixel_x_scale_0 += 0x100;
		pixel_x_scale_1 += 0x100;
		pixel_x_scale_2 += 0x100;
		pixel_x_scale_3 += 0x100;
	}
	if (GetKey(olc::Key::LEFT).bHeld)
	{
		pixel_x_scale_0 -= 0x100;
		pixel_x_scale_1 -= 0x100;
		pixel_x_scale_2 -= 0x100;
		pixel_x_scale_3 -= 0x100;
	}
	if (GetKey(olc::Key::UP).bHeld)
	{
		pixel_y_scale_0 += 0x100;
		pixel_y_scale_1 += 0x100;
		pixel_y_scale_2 += 0x100;
		pixel_y_scale_3 += 0x100;
	}
	if (GetKey(olc::Key::DOWN).bHeld)
	{
		pixel_y_scale_0 -= 0x100;
		pixel_y_scale_1 -= 0x100;
		pixel_y_scale_2 -= 0x100;
		pixel_y_scale_3 -= 0x100;
	}

	auto pixel_colour = olc::VERY_DARK_GREY;
	uint32_t offset;
	uint32_t pixel_lookup;

	for (uint32_t i = 0; i < display_size; i++)
	{
		pixel_colour = olc::VERY_DARK_YELLOW;

		if (screen_buffer_enabled & 0b0001)
		{
			offset = (pixel_x_0 >> 16) + ((pixel_y_0 >> 16) << 10);
			pixel_lookup = screen_buffer_0[offset];
		}
		else
		{
			pixel_lookup = 0x00;
		}

		if (pixel_lookup != 0x00)
		{
			pixel_colour.r = pixel_lookup;
			pixel_colour.g = 0x00;
			pixel_colour.b = 0x00;
		}
		else
		{
			if (screen_buffer_enabled & 0b0010)
			{
				offset = (pixel_x_1 >> 16) + ((pixel_y_1 >> 16) << 10);
				pixel_lookup = screen_buffer_1[offset];
			}
			else
			{
				pixel_lookup = 0x00;
			}

			if (pixel_lookup != 0x00)
			{
				pixel_colour.r = 0x00;
				pixel_colour.g = pixel_lookup;
				pixel_colour.b = 0x00;
			}
			else
			{
				if (screen_buffer_enabled & 0b0100)
				{
					offset = (pixel_x_2 >> 16) + ((pixel_y_2 >> 16) << 10);
					pixel_lookup = screen_buffer_2[offset];
				}
				else
				{
					pixel_lookup = 0x00;
				}

				if (pixel_lookup != 0x00)
				{
					pixel_colour.r = 0x00;
					pixel_colour.g = 0x00;
					pixel_colour.b = pixel_lookup;
				}
				else
				{
					if (screen_buffer_enabled & 0b1000)
					{
						offset = (pixel_x_3 >> 16) + ((pixel_y_3 >> 16) << 10);
						pixel_lookup = screen_buffer_3[offset];
					}
					else
					{
						pixel_lookup = 0x00;
					}

					if (pixel_lookup != 0x00)
					{
						pixel_colour.r = pixel_lookup;
						pixel_colour.g = pixel_lookup;
						pixel_colour.b = pixel_lookup;
					}
				}
			}
		}

		Draw(pixel_x, pixel_y, pixel_colour);

		++pixel_x;

		if (pixel_x >= display_width)
		{
			pixel_x = 0;

			++pixel_y;

			pixel_x_0 = pixel_x_start_0;
			pixel_x_1 = pixel_x_start_1;
			pixel_x_2 = pixel_x_start_2;
			pixel_x_3 = pixel_x_start_3;

			pixel_y_0 += pixel_y_scale_0;
			pixel_y_1 += pixel_y_scale_1;
			pixel_y_2 += pixel_y_scale_2;
			pixel_y_3 += pixel_y_scale_3;
		}

		pixel_x_0 += pixel_x_scale_0;
		pixel_x_1 += pixel_x_scale_1;
		pixel_x_2 += pixel_x_scale_2;
		pixel_x_3 += pixel_x_scale_3;

		pixel_x_0 &= 0x3ffffff;
		pixel_y_0 &= 0x3ffffff;
		pixel_x_1 &= 0x3ffffff;
		pixel_y_1 &= 0x3ffffff;
		pixel_x_2 &= 0x3ffffff;
		pixel_y_2 &= 0x3ffffff;
		pixel_x_3 &= 0x3ffffff;
		pixel_y_3 &= 0x3ffffff;
	}

	pixel_x_start_0 += pixel_x_scale_0;
	pixel_x_start_1 -= pixel_x_scale_1;
	pixel_y_start_2 += pixel_y_scale_2;
	pixel_y_start_3 -= pixel_y_scale_3;

	pixel_y_start_0 += pixel_x_scale_0 >> 1;
	pixel_y_start_1 -= pixel_x_scale_1 >> 1;
	pixel_x_start_2 += pixel_y_scale_2 >> 1;
	pixel_x_start_3 -= pixel_y_scale_3 >> 1;

	SetDrawTarget(nullptr);
	DrawSprite(0, 0, display_buffer, display_scale);

	bus->Debug();

	return true;
}

int main()
{
	Moon moon;

	if (moon.Construct(848, 480, 2, 2, true, true))
		moon.Start();

	return OK;
}
