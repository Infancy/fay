#pragma once

#include "fay/core/fay.h"

namespace fay
{

struct IO
{
	// mouse
	double xpos{}, ypos{}, xlast{}, ylast{};
	double dx{}, dy{};
	float wheel {};
	bool left{}, middle{}, right{};

	// keyboard

	// time
	double last_time{}, cur_time{}, delta_time{};
};

bool create_window(int width = 1080, int height = 720, bool multisample = false);
bool close_window();
void delete_window();

void updateIO();
void swap_framebuffer();

bool keydown(int key);
const IO& get_io();	// const ImGuiIO& gui_get_io();	

} // namespace fay
