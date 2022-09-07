#include "main_loop.h"

namespace loop {
	MainLoop::MainLoop() {
		Init();
	}

	MainLoop* MainLoop::Get() {
		static MainLoop _mainloop;
		return &_mainloop;
	}

	void MainLoop::Init() {
		if (!is_initialized) {
			loop = uv_default_loop();
			is_initialized = true;
		}
	}

	MainLoop::~MainLoop() {
		uv_loop_close(loop);
	}
}