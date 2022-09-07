#pragma once

#include "uv.h"
#include "bind.h"
#include "task.h"
#include <memory>
namespace loop {
	class MainLoop {
	public:
		// Returns the current instance. This is used to get access to the getters
		// that return objects which are owned by this class.
		MainLoop(const MainLoop&) = delete;
		MainLoop& operator=(const MainLoop&) = delete;
		virtual ~MainLoop();
		static MainLoop* Get();
		void Init();

		template <class ReturnT, class ... Args1, class ReturnT2, class ... Args2>
		void PostTask(base::OnceCallback<ReturnT(Args1...)>&& callback, base::OnceCallback<ReturnT2(Args2...)>&& call) {
			task::uv_task * uv_task= new task::uv_task();
			uv_task->callfunc_container= new base::CallBackContainer<ReturnT2,Args2...>(std::move(call));
			uv_task->callback_container = new base::CallBackContainer<ReturnT, Args1...>(std::move(callback));
			uv_work_t* req = new uv_work_t();
			req->data = (void*)uv_task;
			uv_queue_work(loop, req, task::runcall, task::runcallback);
		}

		void Run() {
			uv_run(loop, UV_RUN_DEFAULT);
		}

		void PostDelayedTask();
		void PreCreateMainMessageLoop();
		void CreateMainMessageLoop();
		void PostCreateMainMessageLoop();
		// Performs the shutdown sequence, starting with PostMainMessageLoopRun
		// through stopping threads to PostDestroyThreads.
		void ShutdownThreadsAndCleanUp();

	private:
		MainLoop();
		uv_loop_t* loop;
		bool is_initialized;
		//std::unique_ptr<MainThreadImpl> main_thread_;
		//std::unique_ptr<CurlThread> curl_thread_;
	};
}