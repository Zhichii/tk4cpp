#include <internal/TkApp.hpp>
#include <internal/Eval.hpp>

namespace tk4cpp {

	int cb_call_proc(CallEvent* ev, int flags) {
		auto& argv = *(ev->argv);
		ulll argc = argv.size();
		Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj*[argc]);
		for (ulll i = 0; i < argc; i++) {
			objs[i] = argv[i].object;
		}
		if (Tcl_EvalObjv(ev->app->interp, argc, objs, flags) == TCL_ERROR) *(ev->exc) = 1;
		*(ev->res) = Tcl_GetObjResult(ev->app->interp);
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->call_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->call_mutex);
		}
		delete[] objs;
		return 1; // 1 means success here.
	}

}