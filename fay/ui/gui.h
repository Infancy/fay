#pragma once

#include "fay/core/fay.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace fay
{

bool gui_create_window(int width = 1080, int height = 720, bool multisample = false);
bool gui_close_window();
void gui_delete_window();

void gui_updateIO();
void gui_drawGUI();

ImGuiIO& gui_get_io();	// const ImGuiIO& gui_get_io();	

} // namespace fay
