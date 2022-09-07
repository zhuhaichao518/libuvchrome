#ifndef BASE_TASKRUNNER
#define BASE_TASKRUNNER

#include "uv.h"
#include "bind.h"

namespace task{

    class uv_task {
    public:
        base::CallBackContainerBase* callfunc_container;
        base::CallBackContainerBase* callback_container;
		~uv_task() {
			delete callfunc_container;
			delete callback_container;
		}
    };

	static void runcall(uv_work_t* req) {
		task::uv_task* t = (task::uv_task*)(req->data);
		base::CallBackContainerBase* callfunc = t->callfunc_container;
		base::CallBackContainerBase* callback = t->callback_container;
		callback->setarg1(callfunc->operate());
	}

	static void runcallback(uv_work_t* req, int status) {
		task::uv_task* t = (task::uv_task*)(req->data);
		base::CallBackContainerBase* callback = t->callback_container;
		base::CallBackContainerBase* callfunc = t->callfunc_container;
		callback->operate();
		delete req;
		delete t;
	}
}

#endif