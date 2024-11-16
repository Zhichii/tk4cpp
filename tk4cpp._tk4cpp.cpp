
#include "tk4cpp._tk4cpp.hpp"

namespace _tk4cpp {

	Object::Object() {}
	Object::Object(const Object& n) {
			this->obj = n.obj;
			this->is_func = n.is_func;
			this->function = n.function;
			if (this->obj != nullptr) Tcl_IncrRefCount(this->obj);
		}
	Object::Object(Tcl_Obj* n) {
			this->obj = n;
			if (this->obj != nullptr) Tcl_IncrRefCount(this->obj);
		}
	Object::~Object() {
			if (this->obj == nullptr) return;
			Tcl_DecrRefCount(this->obj);
			this->obj = nullptr;
		}
	bool Object::operator==(Object o) {
		if (this->obj == NULL && o.obj == NULL) return true;
		if (this->obj == NULL && o.obj != NULL) return false;
		if (this->obj != NULL && o.obj == NULL) return false;
		return strcmp(Tcl_GetStringFromObj(this->obj, NULL), Tcl_GetStringFromObj(o.obj, NULL)) == 0;
	}
	bool Object::operator!=(Object o) {
		return !((*this) == o);
	}
	Object::Object(bool n) {
		this->obj = Tcl_NewBooleanObj(n);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(int n) {
		mp_int n2;
		mp_init(&n2);
		mp_set_ll(&n2, n);
		this->obj = Tcl_NewBignumObj(&n2);
		mp_clear(&n2);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(unsigned int n) {
		mp_int n2;
		mp_init(&n2);
		mp_set_ull(&n2, n);
		this->obj = Tcl_NewBignumObj(&n2);
		mp_clear(&n2);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(long long n) {
		mp_int n2;
		mp_init(&n2);
		mp_set_ll(&n2, n);
		this->obj = Tcl_NewBignumObj(&n2);
		mp_clear(&n2);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(unsigned long long n) {
		mp_int n2;
		mp_init(&n2);
		mp_set_ull(&n2, n);
		this->obj = Tcl_NewBignumObj(&n2);
		mp_clear(&n2);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(double n) {
		this->obj = Tcl_NewDoubleObj(n);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(char* n) {
		this->obj = Tcl_NewStringObj(n, -1);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(const char* n) {
		this->obj = Tcl_NewStringObj(n, -1);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(wchar_t* n) {
		Tcl_Size len = wcslen(n);
		Tcl_UniChar* z = (Tcl_UniChar*)attemptckalloc(sizeof(Tcl_UniChar) * (len + 1));
		if (z == nullptr) throw MemoryError();
		for (Tcl_Size i = 0; i < len; i++) {
			z[i] = n[i];
		}
		z[len] = 0;
		this->obj = Tcl_NewUnicodeObj(z, len);
		ckfree(z);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(const wchar_t* n) {
		Tcl_Size len = wcslen(n);
		Tcl_UniChar* z = (Tcl_UniChar*)attemptckalloc(sizeof(Tcl_UniChar) * (len + 1));
		if (z == nullptr) throw MemoryError();
		for (Tcl_Size i = 0; i < len; i++) {
			z[i] = n[i];
		}
		z[len] = 0;
		this->obj = Tcl_NewUnicodeObj(z, len);
		ckfree(z);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(std::string n) {
		this->obj = Tcl_NewStringObj(n.c_str(), n.size());
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(std::wstring n) {
		Tcl_Size len = n.size();
		Tcl_UniChar* z = (Tcl_UniChar*)attemptckalloc(sizeof(Tcl_UniChar) * (len + 1));
		if (z == nullptr) throw MemoryError();
		for (Tcl_Size i = 0; i < len; i++) {
			z[i] = n[i];
		}
		z[len] = 0;
		this->obj = Tcl_NewUnicodeObj(z, len);
		ckfree(z);
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(Func function) {
		this->is_func = true;
		std::string id = "CPPFUN_"+std::to_string((uint)(&function));
		this->obj = Tcl_NewStringObj(id.c_str(), id.size());
		this->function = function;
		Tcl_IncrRefCount(this->obj);
	}
	Object::Object(std::vector<Object> obj) {
		Tcl_Obj** objv = (Tcl_Obj**)attemptckalloc(obj.size() * sizeof(Tcl_Obj*));
		if (objv == nullptr) throw MemoryError();
		for (uint i = 0; i < obj.size(); i++) objv[i] = obj[i].obj;
		this->obj = Tcl_NewListObj(obj.size(), objv);
		Tcl_IncrRefCount(this->obj);
		ckfree(objv);
	}
	Object::operator bool() {
		return this->obj;
	}
	Object::operator std::string() {
		if (*this) return Tcl_GetStringFromObj(this->obj, NULL);
		return "";
	}
	std::ostream& operator<<(std::ostream& os, Object& obj) {
		if (obj) os << std::string(Tcl_GetStringFromObj(obj.obj, NULL));
		return os;
	}

	void __readargs(std::vector<Object>& vs) {}
	template <class A>
	void __readargs(std::vector<Object>& vs, A a) {
		vs.push_back(a);
	}
	template <class A, class ... B>
	void __readargs(std::vector<Object>& vs, A a, B... b) {
		vs.push_back(a);
		__readargs(vs, b...);
	}

	TclError TkApp::tclerror() {
		std::string e = Tcl_GetStringResult(this->interp);
		printf("TclError[%s]\n", e.c_str());
		return TclError(e);
	}

	Id TkApp::newid(Id master) {
		Id id = master;
		id.push_back(std::to_string(this->widCnt++));
		return id;
	}

	std::wstring TkApp::unicode_fromobj(Object object) {
		Tcl_Obj* obj = object.obj;
		Tcl_Size len;
#if USE_TCL_UNICODE
		if (obj->typePtr != NULL &&
			(obj->typePtr == StringType ||
				obj->typePtr == UTF32StringType)) {
			const Tcl_UniChar* u = Tcl_GetUnicodeFromObj(obj, &len);
			if (sizeof(Tcl_UniChar) == 2) {
				const wchar_t* w = (wchar_t*)u;
				return w;
			}
			//else if (sizeof(Tcl_UniChar) == 4) {
			//	
			//}
			else throw ValueError("unknown size");
		}
#endif /* USE_TCL_UNICODE */
		const char* s = Tcl_GetStringFromObj(obj, &len);
		wchar_t* z = (wchar_t*)attemptckalloc(len);
		if (z == NULL) throw MemoryError();
		for (Tcl_Size i = 0; i < len; i++) z[i] = s[i];
		std::wstring w = z;
		ckfree(z);
		return w;
	}
	std::string TkApp::string_fromobj(Object object) {
		Tcl_Obj* obj = object.obj;
		Tcl_Size len;
#if USE_TCL_UNICODE
		if (obj->typePtr != NULL &&
			(obj->typePtr == StringType ||
				obj->typePtr == UTF32StringType)) {
			const Tcl_UniChar* u = Tcl_GetUnicodeFromObj(obj, &len);
			char* z = (char*)attemptckalloc(len * sizeof(Tcl_UniChar));
			if (z == NULL) throw MemoryError();
			Tcl_Size i = 0;
			for (; i < len;) i += Tcl_UniCharToUtf(u[i], z + i);
			ckfree(z);
		}
#endif /* USE_TCL_UNICODE */
		const char* w = Tcl_GetStringFromObj(obj, &len);
		return w;
	}
	bool TkApp::boolean_fromobj(Object object) {
		Tcl_Obj* obj = object.obj;
		int boolValue;
		if (Tcl_GetBooleanFromObj(this->interp, obj, &boolValue) == TCL_ERROR) throw tclerror();
		return boolValue;
	}
	uint TkApp::int_fromobj(Object object) {
		Tcl_Obj* obj = object.obj;
		Tcl_WideInt wideValue;
		if (Tcl_GetWideIntFromObj(this->interp, obj, &wideValue) == TCL_OK) {
			if (sizeof(wideValue) <= sizeof(uint))
				return wideValue;
			throw OverflowError("wide int is too long");
		}
		throw tclerror();
	}
	double TkApp::double_fromobj(Object object) {
		Tcl_Obj* obj = object.obj;
		double doubleValue;
		Tcl_GetDoubleFromObj(this->interp, obj, &doubleValue);
		return doubleValue;
	}

	Object TkApp::call(std::vector<Object> argv) {
		for (const auto& i : argv) {
			if (!i.is_func) continue;
			this->createcommand(this->string_fromobj(i), i.function);
		}
		uint argc = argv.size();
#ifdef _DEBUG
		printf("> ");
		for (uint i = 0; i < argc; i++) {
			printf("%s ", string_fromobj(argv[i]).c_str());
		}
		printf("\n");
#endif
		Object res;
		int exc = 0;
		if (Tcl_GetCurrentThread() != this->thread_id) {
			Tcl_Condition cond = NULL;
			CallEvent* ev;
			ev = (CallEvent*)attemptckalloc(sizeof(CallEvent));
			if (ev == nullptr) throw MemoryError("no memory");
			memset(ev, 0, sizeof(CallEvent));
			ev->ev.proc = (Tcl_EventProc*)CallProc;
			ev->app = this;
			ev->argv = &argv;
			ev->flags = TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL;
			ev->res = &res;
			ev->exc = &exc;
			ev->done = &cond;
			ThreadSend(this, (Tcl_Event*)ev, &cond, &this->call_mutex);
			Tcl_ConditionFinalize(&cond);
		}
		else {
			Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj* [argc]);
			for (uint i = 0; i < argc; i++) objs[i] = argv[i].obj;
			if (Tcl_EvalObjv(interp, argc, objs, TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL) == TCL_ERROR) exc = 1;
			res = Tcl_GetObjResult(interp);
			if (res) Tcl_IncrRefCount(res.obj);
			delete[] objs;
		}
#ifdef _DEBUG
		if (res != "") printf("%s\n", this->string_fromobj(res).c_str());
#endif
		if (exc) throw this->tclerror();
		return res;
	}

	void TkApp::createcommand(std::string name, Func function) {
		int err;
		CmdClientData* cd = new CmdClientData;
		cd->app = this;
		cd->function = function;
		if (Tcl_GetCurrentThread() != this->thread_id) {
			Tcl_Condition cond = NULL;
			CommandEvent* ev;
			ev = (CommandEvent*)attemptckalloc(sizeof(CommandEvent));
			if (ev == NULL) throw MemoryError();
			memset(ev, 0, sizeof(CommandEvent));
			ev->ev.proc = (Tcl_EventProc*)CommandProc;
			ev->app = this;
			ev->data = (ClientData)cd;
			ev->create = 1;
			ev->status = &err;
			ev->name = &name;
			ev->done = &cond;
			ThreadSend(this, (Tcl_Event*)ev, &cond, &this->command_mutex);
			Tcl_ConditionFinalize(&cond);
		}
		else {
			err = Tcl_CreateObjCommand(this->interp, name.c_str(), CommandWrapper, (ClientData)cd, CommandDelete) == nullptr;
		}
		if (err) throw this->tclerror();
	}

	void TkApp::deletecommand(std::string name) {
		int err;
		if (Tcl_GetCurrentThread() != this->thread_id) {
			Tcl_Condition cond = NULL;
			CommandEvent* ev;
			ev = (CommandEvent*)attemptckalloc(sizeof(CommandEvent));
			if (ev == NULL) throw MemoryError();
			memset(ev, 0, sizeof(CommandEvent));
			ev->ev.proc = (Tcl_EventProc*)CommandProc;
			ev->app = this;
			ev->create = 0;
			ev->status = &err;
			ev->name = &name;
			ev->done = &cond;
			ThreadSend(this, (Tcl_Event*)ev, &cond, &this->command_mutex);
			Tcl_ConditionFinalize(&cond);
		}
		else {
			err = Tcl_DeleteCommand(this->interp, name.c_str());
		}
		if (err) throw this->tclerror();
	}

	Object TkApp::varinvoke(int mode, std::string name1, std::string name2, Object obj, int flags) {
		if (Tcl_GetCurrentThread() != this->thread_id) {
			Object object = obj;
			Tcl_Condition cond = nullptr;
			VarEvent* ev;
			ev = (VarEvent*)attemptckalloc(sizeof(VarEvent));
			if (ev == nullptr) throw MemoryError();
			memset(ev, 0, sizeof(VarEvent));
			ev->ev.proc = (Tcl_EventProc*)VarProc;
			ev->app = this;
			ev->name1 = std::string(name1);
			ev->name2 = std::string(name2);
			ev->obj = &object;
			ev->mode = mode;
			ev->flags = flags;
			ev->done = &cond;
			ThreadSend(this, (Tcl_Event*)ev, &cond, &this->var_mutex);
			Tcl_ConditionFinalize(&cond);
			return object;
		}
		else {
			switch (mode) {
			case 0: {
				SetVar(this, name1, name2, &obj, flags);
				return {};
			} break;
			case 1: {
				return GetVar(this, name1, name2, flags);
			} break;
			case 2: {
				UnsetVar(this, name1, name2, flags);
				return {};
			} break;
			}
		}
		return {};
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
		return varinvoke(1, name, "", Object(), TCL_LEAVE_ERR_MSG);
	}
	Object TkApp::getvar(std::string name1, std::string name2) {
		return varinvoke(1, name1, name2, Object(), TCL_LEAVE_ERR_MSG);
	}
	Object TkApp::globalgetvar(std::string name) {
		return varinvoke(1, name, "", Object(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	Object TkApp::globalgetvar(std::string name1, std::string name2) {
		return varinvoke(1, name1, name2, Object(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	void TkApp::unsetvar(std::string name) {
		varinvoke(2, name, "", Object(), TCL_LEAVE_ERR_MSG);
	}
	void TkApp::unsetvar(std::string name1, std::string name2) {
		varinvoke(2, name1, name2, Object(), TCL_LEAVE_ERR_MSG);
	}
	void TkApp::globalunsetvar(std::string name) {
		varinvoke(2, name, "", Object(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}
	void TkApp::globalunsetvar(std::string name1, std::string name2) {
		varinvoke(2, name1, name2, Object(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
	}

	sint TkApp::getint(Object obj) {
		return int_fromobj(obj);
	}
	sint TkApp::getint(std::string str) {
		CHECK_STRING_LENGTH(str);
		return int_fromobj(Object(str));
	}
	double TkApp::getdouble(Object obj) {
		return double_fromobj(obj);
	}
	double TkApp::getdouble(std::string str) {
		CHECK_STRING_LENGTH(str);
		return double_fromobj(Object(str));
	}
	bool TkApp::getboolean(Object obj) {
		return boolean_fromobj(obj);
	}
	bool TkApp::getboolean(std::string str) {
		CHECK_STRING_LENGTH(str);
		int v;
		Tcl_GetBoolean(this->interp, str.c_str(), &v);
		return v == 1;
	}

	std::vector<Object> TkApp::splitlist(Object obj) {
			Tcl_Size objc;
			Tcl_Obj** objv;
			if (Tcl_ListObjGetElements(this->interp, obj.obj, &objc, &objv) == TCL_ERROR) {
				throw this->tclerror();
			}
			std::vector<Object> v;
			for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
			return v;
		}
	std::vector<Object> TkApp::splitlist(std::string str) {
			Tcl_Size objc;
			const char** objv;
			if (Tcl_SplitList(this->interp, str.c_str(), &objc, &objv) == TCL_ERROR) {
				throw this->tclerror();
			}
			std::vector<Object> v;
			for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
			return v;
		}

	TkApp::TkApp() {
		this->interp = Tcl_CreateInterp();
		this->thread_id = Tcl_GetCurrentThread();
		this->widCnt = 1;
		this->call_mutex = 0;
		this->command_mutex = 0;
		this->var_mutex = 0;
		this->OldBooleanType = Tcl_GetObjType("boolean");
		{
			Tcl_Obj* value;
			int boolValue;

			/* Tcl 8.5 "booleanString" type is not registered
				and is renamed to "boolean" in Tcl 9.0.
				Based on approach suggested at
				https://core.tcl-lang.org/tcl/info/3bb3bcf2da5b */
			value = Tcl_NewStringObj("true", -1);
			Tcl_GetBooleanFromObj(NULL, value, &boolValue);
			this->BooleanType = value->typePtr;
			Tcl_DecrRefCount(value);

			// "bytearray" type is not registered in Tcl 9.0
			value = Tcl_NewByteArrayObj(NULL, 0);
			this->ByteArrayType = value->typePtr;
			Tcl_DecrRefCount(value);

			/* TIP 484 suggests retrieving the "int" type without Tcl_GetObjType("int")
			    since it is no longer registered in Tcl 9.0. But even though Tcl 8.7
			    only uses the "wideInt" type on platforms with 32-bit long, it still has
			    a registered "int" type, which FromObj() should recognize just in case. */
			value = Tcl_NewIntObj(0);
			this->IntType = value->typePtr;
			Tcl_DecrRefCount(value);
		}
		this->DoubleType = Tcl_GetObjType("double");
		this->WideIntType = Tcl_GetObjType("wideInt");
		this->BignumType = Tcl_GetObjType("bignum");
		this->ListType = Tcl_GetObjType("list");
		this->ProcBodyType = Tcl_GetObjType("procbody");
		this->StringType = Tcl_GetObjType("string");
		this->UTF32StringType = Tcl_GetObjType("utf32string");

		// Delete the 'exit' command as Python, which can screw things up. 
		Tcl_DeleteCommand(this->interp, "exit");

		Tcl_Init(this->interp);
		Tk_Init(this->interp);
	}

	TkApp::~TkApp() {
		Tcl_DeleteInterp(this->interp);
		Tcl_Finalize();
	}

	void ThreadSend(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex) {
		Tcl_MutexLock(mutex);
		Tcl_ThreadQueueEvent(app->thread_id, ev, TCL_QUEUE_TAIL);
		Tcl_ThreadAlert(app->thread_id);
		Tcl_ConditionWait(cond, mutex, nullptr);
		Tcl_MutexUnlock(mutex);
	}

	int CallProc(CallEvent* ev, int flags) {
		auto& argv = *(ev->argv);
		size_t argc = ev->argv->size();
		Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj * [argc]);
		for (size_t i = 0; i < argc; i++) {
			objs[i] = argv[i].obj;
			if (objs[i] == nullptr) {
				*(ev->exc) = 1;
				*(ev->res) = "no memory";
				/* Reset objc, so it attempts to clear
				   objects only up to i. */
				argc = i;
				goto CLEANUP;
			}
		}
		if (Tcl_EvalObjv(ev->app->interp, argc, objs, flags) == TCL_ERROR) *(ev->exc) = 1;
		*(ev->res) = Tcl_GetObjResult(ev->app->interp);
		if (*ev->res) Tcl_IncrRefCount(ev->res->obj);
		/* Wake up calling thread. */
		Tcl_MutexLock(&ev->app->call_mutex);
		Tcl_ConditionNotify(ev->done);
		Tcl_MutexUnlock(&ev->app->call_mutex);
	CLEANUP:
		delete[] objs;
		return 1;
	}

	int CommandWrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
		CmdClientData& cd = *(CmdClientData*)clientData;
		std::vector<Object> objs;
		for (Tcl_Size i = 0; i < objc; i++) {
			objs.push_back(objv[i]);
		}
		Object res = cd.function(objs);
		if (res)
			Tcl_SetObjResult(cd.app->interp, res.obj);
		else
			Tcl_ResetResult(cd.app->interp);
		return 0;
	}

	void CommandDelete(ClientData clientData) {
		CmdClientData* data = (CmdClientData*)clientData;
		delete data;
	}

	int CommandProc(CommandEvent* ev, int flags) {
		if (ev->create) {
			(*ev->status) = Tcl_CreateObjCommand(ev->app->interp, ev->name->c_str(), CommandWrapper, ev->data, nullptr) == nullptr;
		}
		else {
			(*ev->status) = Tcl_DeleteCommand(ev->app->interp, ev->name->c_str());
		}
		/* Wake up calling thread. */
		Tcl_MutexLock(&ev->app->command_mutex);
		Tcl_ConditionNotify(ev->done);
		Tcl_MutexUnlock(&ev->app->command_mutex);
		return 1;
	}

	void SetVar(TkApp* app, std::string name1, std::string name2, Object* newval, int flags) {
		Object ok;
		if (name2 == "") {
			CHECK_STRING_LENGTH(name1);
			ok = Tcl_SetVar2Ex(app->interp, name1.c_str(), NULL, newval->obj, flags);
			if (ok.obj == NULL) throw app->tclerror();
		}
		else {
			CHECK_STRING_LENGTH(name1);
			CHECK_STRING_LENGTH(name2);
			ok = Tcl_SetVar2Ex(app->interp, name1.c_str(), name2.c_str(), newval->obj, flags);
			if (ok.obj == NULL) throw app->tclerror();
		}
	}

	Object GetVar(TkApp* app, std::string name1, std::string name2, int flags) {
		CHECK_STRING_LENGTH(name1);
		CHECK_STRING_LENGTH(name2);
		Tcl_Obj* tres;
		if (name2 == "")
			tres = Tcl_GetVar2Ex(app->interp, name1.c_str(), NULL, flags);
		else
			tres = Tcl_GetVar2Ex(app->interp, name1.c_str(), name2.c_str(), flags);
		if (tres == NULL) throw app->tclerror();
		return tres;
	}

	void UnsetVar(TkApp* app, std::string name1, std::string name2, int flags) {
		Object ok;
		if (name2 == "") {
			CHECK_STRING_LENGTH(name1);
			ok = Tcl_UnsetVar2(app->interp, name1.c_str(), NULL, flags);
			if (ok.obj == NULL) throw app->tclerror();
		}
		else {
			CHECK_STRING_LENGTH(name1);
			CHECK_STRING_LENGTH(name2);
			ok = Tcl_UnsetVar2(app->interp, name1.c_str(), name2.c_str(), flags);
			if (ok.obj == NULL) throw app->tclerror();
		}
	}

	int VarProc(VarEvent* ev, int flags) {
		switch (ev->mode) {
		case 0: {
			SetVar(ev->app, ev->name1, ev->name2, ev->obj, ev->flags);
			*(ev->obj) = {};
		} break;
		case 1: {
			*(ev->obj) = GetVar(ev->app, ev->name1, ev->name2, ev->flags);
		} break;
		case 2: {
			UnsetVar(ev->app, ev->name1, ev->name2, ev->flags);
			*(ev->obj) = {};
		} break;
		}
		/* Wake up calling thread. */
		Tcl_MutexLock(&ev->app->var_mutex);
		Tcl_ConditionNotify(ev->done);
		Tcl_MutexUnlock(&ev->app->var_mutex);
		return 1;
	}

}