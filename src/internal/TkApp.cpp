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
		uint argc = argv.size();
		Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj*[argc]);
		for (uint i = 0; i < argc; i++) {
			objs[i] = argv[i].object;
		}
		if (Tcl_EvalObjv(ev->app->interp, argc, objs, flags) == TCL_ERROR)
			TKI_TCL_ERR(ev->app);
		*(ev->ret) = Tcl_GetObjResult(ev->app->interp);
		delete[] objs;
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->call_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->call_mutex);
		}
		return 1; // 1 means success here.
	}

	int command_wrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
		CmdClientData& cd = *(CmdClientData*)clientData;
		std::vector<Object> objs;
		for (Tcl_Size i = 0; i < objc; i++) {
			objs.push_back(objv[i]);
		}
		Object ret = cd.function(objs);
		if (ret.is())
			Tcl_SetObjResult(cd.app->interp, ret.object);
		else
			Tcl_ResetResult(cd.app->interp);
		return 1;
	}

	void command_delete(ClientData clientData) {
		CmdClientData* data = (CmdClientData*)clientData;
		delete data;
	}

	int command_proc(CommandEvent* ev, int flags) {
		int err;
		if (ev->mode) {
			err = Tcl_CreateObjCommand(ev->app->interp, ev->name->c_str(), command_wrapper, ev->data, nullptr) == nullptr;
		}
		else {
			err = Tcl_DeleteCommand(ev->app->interp, ev->name->c_str());
		}
		if (err) TKI_TCL_ERR(ev->app);
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->command_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->command_mutex);
		}
		return 1; // 1 means success here.
	}

	void set_var(TkApp* app, std::string name1, std::string name2, Object* newval, int flags) {
		Object ok;
		safe_incr_refcnt(newval->object);
		if (name2 == "") {
			ok = Tcl_SetVar2Ex(app->interp, name1.c_str(), NULL, newval->object, flags);
		}
		else {
			ok = Tcl_SetVar2Ex(app->interp, name1.c_str(), name2.c_str(), newval->object, flags);
		}
		if (!ok.is()) TKI_TCL_ERR(app);
	}

	Object get_var(TkApp* app, std::string name1, std::string name2, int flags) {
		Tcl_Obj* ret;
		if (name2 == "")
			ret = Tcl_GetVar2Ex(app->interp, name1.c_str(), NULL, flags);
		else
			ret = Tcl_GetVar2Ex(app->interp, name1.c_str(), name2.c_str(), flags);
		if (!ret) TKI_TCL_ERR(app);
		return ret;
	}

	void unset_var(TkApp* app, std::string name1, std::string name2, int flags) {
		int ok;
		if (name2 == "") {
			ok = Tcl_UnsetVar2(app->interp, name1.c_str(), NULL, flags);
		}
		else {
			ok = Tcl_UnsetVar2(app->interp, name1.c_str(), name2.c_str(), flags);
		}
		if (ok == TCL_ERROR) TKI_TCL_ERR(app);
	}

	int var_proc(VarEvent* ev, int flags) {
		switch (ev->mode) {
		case 0: {
			set_var(ev->app, ev->name1, ev->name2, ev->obj, ev->flags);
			*(ev->obj) = {};
		} break;
		case 1: {
			*(ev->obj) = get_var(ev->app, ev->name1, ev->name2, ev->flags);
		} break;
		case 2: {
			unset_var(ev->app, ev->name1, ev->name2, ev->flags);
			*(ev->obj) = {};
		} break;
		}
		if (ev->done) {
			/* Wake up calling thread. */
			Tcl_MutexLock(&ev->app->var_mutex);
			Tcl_ConditionNotify(ev->done);
			Tcl_MutexUnlock(&ev->app->var_mutex);
		}
		return 1; // 1 means success here.
	}

	std::wstring TkApp::unicode_fromobj(Object object) {
		Tcl_Obj* obj = object.object;
		Tcl_Size len;
		if (obj->typePtr != NULL &&
			(obj->typePtr == this->string_type ||
				obj->typePtr == this->utf32_string_type)) {
			const Tcl_UniChar* u = Tcl_GetUnicodeFromObj(obj, &len);
			if (sizeof(Tcl_UniChar) == 2) {
				const wchar_t* w = (wchar_t*)u;
				return w;
			}
			else TKI_ERR("ValueError", "unknown char size");
		}
		else return {};
	}
	std::string TkApp::string_fromobj(Object object) {
		Tcl_Obj* obj = object.object;
		Tcl_Size len;
		const char* w = Tcl_GetStringFromObj(obj, &len);
		return w;
	}
	bool TkApp::boolean_fromobj(Object object) {
		Tcl_Obj* obj = object.object;
		int boolValue;
		if (Tcl_GetBooleanFromObj(this->interp, obj, &boolValue) == TCL_ERROR) TKI_TCL_ERR(this);
		return boolValue;
	}
	sint TkApp::int_fromobj(Object object) {
		Tcl_Obj* obj = object.object;
		Tcl_WideInt wideValue;
		if (Tcl_GetWideIntFromObj(this->interp, obj, &wideValue) == TCL_OK) {
			if (sizeof(wideValue) <= sizeof(ui32))
				return wideValue;
			else TKI_ERR("OverflowError", "wide is too long");
		}
		else TKI_TCL_ERR(this);
		return {};
	}
	double TkApp::double_fromobj(Object object) {
		Tcl_Obj* obj = object.object;
		double doubleValue;
		Tcl_GetDoubleFromObj(this->interp, obj, &doubleValue);
		return doubleValue;
	}

	Object TkApp::call(std::vector<Object> argv) {
		for (auto& i : argv) {
			if (i.no_func) continue;
			this->createcommand(i.str(), i.function);
		}
		uint argc = argv.size();
#ifdef _DEBUG
		printf("> ");
		for (uint i = 0; i < argc; i++) {
			printf("%s ", argv[i].str().c_str());
		}
		printf("\n");
#endif
		Object ret;
		CallEvent* ev = (CallEvent*)attemptckalloc(sizeof(CallEvent));
		if (ev == nullptr) TKI_MEM_ERR();
		memset(ev, 0, sizeof(CallEvent));
		ev->proc = (Tcl_EventProc*)call_proc;
		ev->app = this;
		ev->argv = &argv;
		ev->flags = TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL;
		ev->ret = &ret;
		ev->done = NULL;
		thread_send_if(this, (Tcl_Event*)ev, &this->call_mutex);
#ifdef _DEBUG
		if (ret.is()) printf("%s\n", ret.str().c_str());
#endif
		return ret;
	}

	void TkApp::createcommand(std::string name, Func function) {
		CmdClientData* cd = new CmdClientData;
		cd->app = this;
		cd->function = function;
		CommandEvent* ev = (CommandEvent*)attemptckalloc(sizeof(CommandEvent));
		if (ev == nullptr) TKI_MEM_ERR();
		memset(ev, 0, sizeof(CommandEvent));
		ev->proc = (Tcl_EventProc*)command_proc;
		ev->app = this;
		ev->data = (ClientData)cd;
		ev->mode = 1;
		ev->name = &name;
		thread_send_if(this, (Tcl_Event*)ev, &this->command_mutex);
	}

	void TkApp::deletecommand(std::string name) {
		CommandEvent* ev = (CommandEvent*)attemptckalloc(sizeof(CommandEvent));
		if (ev == nullptr) TKI_MEM_ERR();
		memset(ev, 0, sizeof(CommandEvent));
		ev->proc = (Tcl_EventProc*)command_proc;
		ev->app = this;
		ev->data = NULL;
		ev->mode = 0;
		ev->name = &name;
		thread_send_if(this, (Tcl_Event*)ev, &this->command_mutex);
	}

	Object TkApp::varinvoke(int mode, std::string name1, std::string name2, Object obj, int flags) {
		Object object = obj;
		VarEvent* ev = (VarEvent*)attemptckalloc(sizeof(VarEvent));
		if (ev == nullptr) TKI_MEM_ERR();
		memset(ev, 0, sizeof(VarEvent));
		ev->proc = (Tcl_EventProc*)var_proc;
		ev->app = this;
		ev->name1 = std::string(name1);
		ev->name2 = std::string(name2);
		ev->obj = &object;
		ev->mode = mode;
		ev->flags = flags;
		thread_send_if(this, (Tcl_Event*)ev, &this->var_mutex);
		return object;
	}

	void TkApp::setvar(std::string name, Object var) {
		varinvoke(0, name, "", var, TCL_LEAVE_ERR_MSG);
	}
	void TkApp::setvar(std::string name1, std::string name2, Object var) {
		varinvoke(0, name1, name2, var, TCL_LEAVE_ERR_MSG);
	}
	void TkApp::globalsetvar(std::string name, Object var) {
		varinvoke(0, name, "", var, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	void TkApp::globalsetvar(std::string name1, std::string name2, Object var) {
		varinvoke(0, name1, name2, var, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	Object TkApp::getvar(std::string name) {
		return varinvoke(1, name, "", {}, TCL_LEAVE_ERR_MSG);
	}
	Object TkApp::getvar(std::string name1, std::string name2) {
		return varinvoke(1, name1, name2, {}, TCL_LEAVE_ERR_MSG);
	}
	Object TkApp::globalgetvar(std::string name) {
		return varinvoke(1, name, "", {}, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	Object TkApp::globalgetvar(std::string name1, std::string name2) {
		return varinvoke(1, name1, name2, {}, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	void TkApp::unsetvar(std::string name) {
		varinvoke(2, name, "", {}, TCL_LEAVE_ERR_MSG);
	}
	void TkApp::unsetvar(std::string name1, std::string name2) {
		varinvoke(2, name1, name2, {}, TCL_LEAVE_ERR_MSG);
	}
	void TkApp::globalunsetvar(std::string name) {
		varinvoke(2, name, "", {}, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	void TkApp::globalunsetvar(std::string name1, std::string name2) {
		varinvoke(2, name1, name2, {}, TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}

	sint TkApp::getint(std::string str) {
		return this->int_fromobj(str);
	}
	sint TkApp::getint(Object obj) {
		return this->int_fromobj(obj);
	}
	double TkApp::getdouble(std::string str) {
		double v;
		Tcl_GetDouble(this->interp, str.c_str(), &v);
		return v;
	}
	double TkApp::getdouble(Object obj) {
		return this->double_fromobj(obj);
	}
	bool TkApp::getboolean(std::string str) {
		int v;
		Tcl_GetBoolean(this->interp, str.c_str(), &v);
		return v == 1;
	}
	bool TkApp::getboolean(Object obj) {
		return this->boolean_fromobj(obj);
	}

	std::vector<Object> TkApp::splitlist(Object obj) {
		Tcl_Size objc;
		Tcl_Obj** objv;
		if (Tcl_ListObjGetElements(this->interp, obj.object, &objc, &objv) == TCL_ERROR) {
			TKI_TCL_ERR(this);
		}
		std::vector<Object> v;
		for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
		return v;
	}
	std::vector<Object> TkApp::splitlist(std::string str) {
		Tcl_Size objc;
		const char** objv;
		if (Tcl_SplitList(this->interp, str.c_str(), &objc, &objv) == TCL_ERROR) {
			TKI_TCL_ERR(this);
		}
		std::vector<Object> v;
		for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
		return v;
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