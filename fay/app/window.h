#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_APP_WINDOW_H
#define FAY_APP_WINDOW_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fay/app/input.h"
#include "fay/core/config.h"
#include "fay/core/fay.h"

namespace fay
{

struct window_desc	// TODO: merge to config?
{
	uint32_t width{ 1080 };
	uint32_t height{ 720 };
	std::string title{ "test" };

	cursor_mode cursor_mode_v{ cursor_mode::hidden };
	render_backend render_backend_v{ g_config.render_backend_v };
	uint32_t MSAA{ 1 };	// if MSAA > 1, open MSAA
};

class window
{
public:
	window() {}
	window(const window_desc& desc) : desc_{ desc }
	{
	}

	virtual ~window()
	{
	}



	// virtual void open() = 0;
	virtual void close() = 0;
	virtual void update_input() = 0;

protected:
	window_desc desc_{};
};



// -------------------------------------------------------------------------------------------------
// glfw



namespace glfw_detail
{

// TODO��
// inline std::unorderd_map window_input;

// glfw callback functions

inline void error_callback(int error, const char* description)
{
	LOG(ERROR) << "GLFW error " << error << ": " << description;
}

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.

	// TODO: render_viewport((0, 0, width, height);
	glViewport(0, 0, width, height);
}

inline void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// TODO
	input_.left = input_.middle = input_.right = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		input_.left = true;
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		input_.middle = true;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		input_.right = true;
}

inline void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
	// ???
	input_.wheel = yoffset;
}

}

class window_glfw : public window
{
public:
	// using window::window;
	window_glfw() {}
	window_glfw(const window_desc& desc) : window(desc)
	{
		glfwSetErrorCallback(glfw_detail::error_callback);
		glfwInit();

		if (desc_.render_backend_v == render_backend::opengl33)
		{
			create_window_and_glcontext(3, 3);
		}
		else if (desc_.render_backend_v == render_backend::opengl45)
		{
			create_window_and_glcontext(4, 5);
		}
		else
		{
			// TODO: create other context/device by window too?
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			window_ = glfwCreateWindow(desc_.width, desc_.height, desc_.title.c_str(), nullptr, nullptr);
			if (window_ == nullptr)
			{
				LOG(ERROR) << "window_glfw: Failed to create GLFW window";
				glfwTerminate();
			}
		}

		// tell GLFW to capture mouse
		switch (desc_.cursor_mode_v)
		{
		case cursor_mode::normal:
		{
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		}
		case cursor_mode::hidden:
		{
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
		}
		case cursor_mode::disabled:
		{
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
		default:
			break;
		}
		glfwGetCursorPos(window_, &input_.lastx, &input_.lasty);

		glfwSetFramebufferSizeCallback(window_, glfw_detail::framebuffer_size_callback);
		glfwSetMouseButtonCallback(window_, glfw_detail::mouse_button_callback);
		glfwSetScrollCallback(window_, glfw_detail::scroll_callback);
	}

	~window_glfw() override
	{
		// Destroys the specified window and its context.
		glfwDestroyWindow(window_);
		// clearing all previously allocated GLFW resources.
		glfwTerminate();
	}

	// TODO: broadcast(event::close_window);
	bool should_close()
	{
		return glfwWindowShouldClose(window_);
	}

	void close() override
	{
		glfwSetWindowShouldClose(window_, true);	// then glfwWindowShouldClose(GLFWwindow*  window) return true
		LOG_IF(ERROR, glfwWindowShouldClose(window_) != 0);
	}

	void update_input()
	{
		// Processing events will cause the window
		// and input callbacks associated with those events to be called.
		glfwPollEvents();

		// mouse position
		input_.lastx = input_.posx; input_.lasty = input_.posy;
		//if (glfwGetWindowAttrib(window_, GLFW_FOCUSED))
		glfwGetCursorPos(window_, &input_.posx, &input_.posy);
		input_.dx = input_.posx - input_.lastx;
		input_.dy = input_.posy - input_.lasty;

		// time
		input_.last = input_.time;
		input_.time = glfwGetTime();
		input_.dt = input_.time - input_.last;
		if (input_.dt == 0.0)
			input_.dt = 1.0 / 60.0;	// TODO: depend by screen

		// TODO: keyboard
	}

	// TODO:remove
	void swapbuffers()	// don't clear framebuffer
	{
		glfwSwapBuffers(window_);
	}

private:
	// create glcontext by glfw and glad, or you want to handle it by yourself, deal with wgl, glx...?
	void create_window_and_glcontext(int major, int minor)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef FAY_IN_APPLE
		// uncomment this statement to fix compilation on OS X
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
		if (desc_.MSAA > 1)
			glfwWindowHint(GLFW_SAMPLES, desc_.MSAA);

		// glfw window creation
		window_ = glfwCreateWindow(desc_.width, desc_.height, desc_.title.c_str(), nullptr, nullptr);
		if (window_ == nullptr)
		{
			LOG(ERROR) << "window_glfw: Failed to create GLFW window";
			glfwTerminate();
		}

		glfwMakeContextCurrent(window_);
		// glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG(ERROR) << "Failed to initialize GLAD";
		}
	}

	bool keydown(int key) { return (glfwGetKey(window_, key) == GLFW_PRESS); }

private:
	GLFWwindow*  window_{ nullptr };
};



// -------------------------------------------------------------------------------------------------
// imgui



class window_imgui
{

};

} // namespace fay

#endif // FAY_APP_WINDOW_H