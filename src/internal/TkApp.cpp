#include <internal/TkApp.hpp>

namespace tki {
	
	void thread_send_if(TkApp* app, Tcl_Event* ev_, Tcl_Mutex* mutex) {
		Event* ev = (Event*)ev_;
		if (Tcl_GetCurrentThread() != app->thread_id) {
			Tcl_Condition cond = NULL;
			ev->done = &cond;
			Tcl_MutexLock(mutex);
			Tcl_ThreadQueueEvent(app->thread_id, ev, TCL_QUEUE_TAIL);
			Tcl_ThreadAlert(app->thread_id);
			Tcl_ConditionWait(&cond, mutex, nullptr);
			Tcl_MutexUnlock(mutex);
			Tcl_ConditionFinalize(&cond);
		}
		else {
			ev->proc(ev_, 0);
			ckfree(ev);
		}
	}

	int call_proc(CallEvent* ev, int flags) {
		auto& argv = *(ev->argv);
		ulll argc = argv.size();
		Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj*[argc]);
		for (ulll i = 0; i < argc; i++) {
			objs[i] = argv[i].object;
		}
		if (Tcl_EvalObjv(ev->app->interp, argc, objs, flags) == TCL_ERROR) *(ev->err) = 1;
		*(ev->ret) = Tcl_GetObjResult(ev->app->interp);
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->call_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->call_mutex);
		}
		delete[] objs;
		return 1; // 1 means success here.
	}

	int command_wrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
		CmdClientData& cd = *(CmdClientData*)clientData;
		std::vector<Object> objs;
		for (Tcl_Size i = 0; i < objc; i++) {
			objs.push_back(objv[i]);
		}
		int err = TCL_OK;
		Object ret = cd.function(err, objs);
		if (ret)
			Tcl_SetObjResult(cd.app->interp, Object(ret).object);
		else
			Tcl_ResetResult(cd.app->interp);
		return err;
	}

	void command_delete(ClientData clientData) {
		CmdClientData* data = (CmdClientData*)clientData;
		delete data;
	}

	int command_proc(CommandEvent* ev, int flags) {
		if (ev->create) {
			(*ev->err) = Tcl_CreateObjCommand(ev->app->interp, ev->name->c_str(), command_wrapper, ev->data, nullptr) == nullptr;
		}
		else {
			(*ev->err) = Tcl_DeleteCommand(ev->app->interp, ev->name->c_str());
		}
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->command_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->command_mutex);
		}
		return 1; // 1 means success here.
	}

	Object TkApp::eval(std::vector<Object> argv) {
		for (auto& i : argv) {
			if (i.no_func) continue;
			this->createcommand(i.str(), i.function);
		}
		ulll argc = argv.size();
#ifdef _DEBUG
		printf("> ");
		for (ulll i = 0; i < argc; i++) {
			printf("%s ", argv[i].str().c_str());
		}
		printf("\n");
#endif
		Object ret;
		int err = 0;
		CallEvent* ev = (CallEvent*)attemptckalloc(sizeof(CallEvent));
		if (ev == nullptr) THROW_ERROR("MemoryError", "no memory");
		memset(ev, 0, sizeof(CallEvent));
		ev->proc = (Tcl_EventProc*)call_proc;
		ev->app = this;
		ev->argv = &argv;
		ev->flags = TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL;
		ev->ret = &ret;
		ev->err = &err;
		ev->done = NULL;
		thread_send_if(this, (Tcl_Event*)ev, &this->call_mutex);
		if (err == TCL_ERROR) THROW_TCL_ERROR(this);
#ifdef _DEBUG
		if (ret) printf("%s\n", ret.str().c_str());
#endif
		return ret;
	}

	void TkApp::createcommand(std::string name, Func function) {
		int err;
		CmdClientData* cd = new CmdClientData;
		cd->app = this;
		cd->function = function;
		CommandEvent* ev = (CommandEvent*)attemptckalloc(sizeof(CommandEvent));
		if (ev == nullptr) THROW_ERROR("MemoryError", "no memory");
		memset(ev, 0, sizeof(CommandEvent));
		ev->proc = (Tcl_EventProc*)command_proc;
		ev->app = this;
		ev->data = (ClientData)cd;
		ev->create = 1;
		ev->err = &err;
		ev->name = &name;
		thread_send_if(this, (Tcl_Event*)ev, &this->command_mutex);
		if (err) THROW_TCL_ERROR(this);
	}

	TkApp::TkApp() {
		this->interp = Tcl_CreateInterp();
		this->thread_id = Tcl_GetCurrentThread();
		this->call_mutex = 0;
		this->command_mutex = 0;
		this->var_mutex = 0;
		this->old_boolean_type = Tcl_GetObjType("boolean");
		{
			Tcl_Obj* value;
			int boolValue;

			/* Tcl 8.5 "booleanString" type is not registered
				and is renamed to "boolean" in Tcl 9.0.
				Based on approach suggested at
				https://core.tcl-lang.org/tcl/info/3bb3bcf2da5b */
			value = Tcl_NewStringObj("true", -1);
			Tcl_GetBooleanFromObj(NULL, value, &boolValue);
			this->boolean_type = value->typePtr;
			safe_decr_refcnt(value);

			// "bytearray" type is not registered in Tcl 9.0
			value = Tcl_NewByteArrayObj(NULL, 0);
			this->byte_array_type = value->typePtr;
			safe_decr_refcnt(value);

			/* TIP 484 suggests retrieving the "int" type without Tcl_GetObjType("int")
				since it is no longer registered in Tcl 9.0. But even though Tcl 8.7
				only uses the "wideInt" type on platforms with 32-bit long, it still has
				a registered "int" type, which FromObj() should recognize just in case. */
			value = Tcl_NewIntObj(0);
			this->int_type = value->typePtr;
			safe_decr_refcnt(value);
		}
		this->double_type = Tcl_GetObjType("double");
		this->wide_int_type = Tcl_GetObjType("wideInt");
		this->bignum_type = Tcl_GetObjType("bignum");
		this->list_type = Tcl_GetObjType("list");
		this->proc_body_type = Tcl_GetObjType("procbody");
		this->string_type = Tcl_GetObjType("string");
		this->utf32_string_type = Tcl_GetObjType("utf32string");

		// Delete the 'exit' command as Python, which can screw things up. 
		Tcl_DeleteCommand(this->interp, "exit");

		Tcl_Init(this->interp);
		Tk_Init(this->interp);
	}

	TkApp::~TkApp() {
		Tcl_DeleteInterp(this->interp);
		Tcl_Finalize();
	}

}