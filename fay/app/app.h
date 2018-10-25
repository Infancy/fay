#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_APP_APP_H
#define FAY_APP_APP_H

#include "fay/app/window.h"
#include "fay/core/fay.h"

namespace fay
{

struct app_desc
{
	window_desc window{};
};

class app	// : protected app_desc
{
public:
	app(const app_desc& desc) : desc_{ desc }
	{
	}

	virtual ~app()
	{
	}



	virtual void init()
	{
	}

	virtual void update()
	{
	}

	// virtual void close()

	virtual int run()
	{
		init_app();
		init();	// init_user_setup

		while (true)
		{
			// event
			window_->update_input();

			// update
			update();

			// show
			window_->swapbuffers();
		}

		return 0;
	}

private:
	void init_app()
	{
		// TODO: factory
		window_ = std::make_unique<window_glfw>(desc_.window);
	}

protected:
	app_desc desc_;
	std::unique_ptr<window_glfw> window_;
};

class app_manager
{

};

} // namespace fay

#endif // FAY_APP_APP_H