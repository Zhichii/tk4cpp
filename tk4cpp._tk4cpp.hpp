/* Interface to Tcl/Tk. Thanks to Python! */

#ifndef TK4CPP__TKINTER_HEADER
#define TK4CPP__TKINTER_HEADER

#include <limits>

// Minimum value between x and y.
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

// Maximum value between x and y.
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// Absolute value of the number x.
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define CHECK_SIZE(size, elemsize) \
    ((uint)(size) <= Py_MIN((uint)INT_MAX, UINT_MAX / (uint)(elemsize)))
#if SIZE_MAX > INT_MAX
#define CHECK_STRING_LENGTH(s) do {                                     \
        if (s.size() >= INT_MAX) {                                      \
            throw OverflowError("string is too long");                  \
        } } while(0)
#else
#define CHECK_STRING_LENGTH(s)
#endif

#define USE_TCL_UNICODE 1

#define STATIC_BUILD 1
#define TCL_THREADS 1
#define TCL_SHARED_BUILD 0
#include <tcl.h>
#include <tk.h>

#define TK_HEX_VERSION ((TK_MAJOR_VERSION << 24) | \
                        (TK_MINOR_VERSION << 16) | \
                        (TK_RELEASE_LEVEL << 8) | \
                        (TK_RELEASE_SERIAL << 0))
#if TK_HEX_VERSION < 0x0806020f
#error "Please use Tk 8.6.15"
#endif

#ifndef TCL_WITH_EXTERNAL_TOMMATH
#define TCL_NO_TOMMATH_H
#endif
#if defined(TCL_WITH_EXTERNAL_TOMMATH) || (TK_HEX_VERSION >= 0x08070000)
#define USE_DEPRECATED_TOMMATH_API 0
#else
#define USE_DEPRECATED_TOMMATH_API 1
#endif
#include <tclTomMath.h>

#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
#define TCL_SIZE_MAX INT_MAX
#endif

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <thread>
#include <functional>
#include <stdexcept>
#include <cstdlib>
#include <malloc.h>
#include <cassert>
#define NONE _tk4cpp::Obj()

namespace _tk4cpp {

//#if defined(_WIN64) & defined(_WIN32)
//	typedef __int64 sint;
//	typedef unsigned __int64 uint;
//#endif
//#if !defined(_WIN64) & defined(_WIN32)
//	typedef __int32 sint;
//	typedef unsigned __int32 uint;
//#endif
//#if defined(__APPLE__) & defined(__LP64__)
//	typedef long long sint;
//	typedef unsigned long long uint;
//#endif
//#if defined(__APPLE__) & !defined(__LP64__)
//	typedef int sint;
//	typedef unsigned int uint;
//#endif
	typedef long long sint;
	typedef unsigned long long uint;

	class TkApp;

	//  Stores the identifier for widgets.
	class Id : public std::vector<std::string> {
	public:
		inline std::string to_string() {
			std::string _Output;
			for (const std::string& _Iter : (*this)) {
				_Output += "." + _Iter;
			}
			return _Output;
		}
	};

	class Error : public std::logic_error { // base of all logic-error exceptions
	public:
		using _Mybase = logic_error;
		explicit Error(const std::string& _Message) : _Mybase(_Message.c_str()) {}
		explicit Error(const char* _Message) : _Mybase(_Message) {}
		explicit Error() : _Mybase("") {}
	};
	typedef Error OverflowError;
	typedef Error MemoryError;
	typedef Error ValueError;
	typedef Error RuntimeError;
	typedef Error AssertionError;
	typedef Error TypeError;
	typedef Error TclError;

	class Obj;

	typedef Obj _Func(std::vector<Obj>&);
	typedef std::function<_Func> Func;

	class Obj {
	public:
		Tcl_Obj* obj = nullptr;
		bool is_func = false;
		Func function;
		Obj() {}
		Obj(const Obj& n) {
			this->obj = n.obj;
			this->is_func = n.is_func;
			this->function = n.function;
			if (this->obj != nullptr) Tcl_IncrRefCount(this->obj);
		}
		Obj(Tcl_Obj* n) {
			this->obj = n;
			if (this->obj != nullptr) Tcl_IncrRefCount(this->obj);
		}
		~Obj() {
			if (this->obj == nullptr) return;
			Tcl_DecrRefCount(this->obj);
			this->obj = nullptr;
		}
		bool operator==(Obj o) {
			if (this->obj == NULL && o.obj == NULL) return true;
			if (this->obj == NULL && o.obj != NULL) return false;
			if (this->obj != NULL && o.obj == NULL) return false;
			return strcmp(Tcl_GetStringFromObj(this->obj, NULL), Tcl_GetStringFromObj(o.obj, NULL)) == 0;
		}
		inline Obj(bool n) {
			this->obj = Tcl_NewBooleanObj(n);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(int n) {
			mp_int n2;
			mp_init(&n2);
			mp_set_ll(&n2, n);
			this->obj = Tcl_NewBignumObj(&n2);
			mp_clear(&n2);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(unsigned int n) {
			mp_int n2;
			mp_init(&n2);
			mp_set_ull(&n2, n);
			this->obj = Tcl_NewBignumObj(&n2);
			mp_clear(&n2);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(long long n) {
			mp_int n2;
			mp_init(&n2);
			mp_set_ll(&n2, n);
			this->obj = Tcl_NewBignumObj(&n2);
			mp_clear(&n2);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(unsigned long long n) {
			mp_int n2;
			mp_init(&n2);
			mp_set_ull(&n2, n);
			this->obj = Tcl_NewBignumObj(&n2);
			mp_clear(&n2);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(double n) {
			this->obj = Tcl_NewDoubleObj(n);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(char* n) {
			this->obj = Tcl_NewStringObj(n, -1);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(const char* n) {
			this->obj = Tcl_NewStringObj(n, -1);
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(wchar_t* n) {
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
		inline Obj(const wchar_t* n) {
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
		inline Obj(std::string n) {
			this->obj = Tcl_NewStringObj(n.c_str(), n.size());
			Tcl_IncrRefCount(this->obj);
		}
		inline Obj(std::wstring n) {
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
		inline Obj(std::vector<Obj> obj) {
			Tcl_Obj** objv = (Tcl_Obj**)attemptckalloc(obj.size() * sizeof(Tcl_Obj*));
			if (objv == nullptr) throw MemoryError();
			for (uint i = 0; i < obj.size(); i++) objv[i] = obj[i].obj;
			this->obj = Tcl_NewListObj(obj.size(), objv);
			Tcl_IncrRefCount(this->obj);
			ckfree(objv);
		}
		inline Obj(Func function) {
			this->is_func = true;
			std::string id = "CPPFUN_"+std::to_string((uint)(&function));
			this->obj = Tcl_NewStringObj(id.c_str(), id.size());
			this->function = function;
			Tcl_IncrRefCount(this->obj);
		}
		friend std::ostream& operator<<(std::ostream& os, const Obj& obj) {
			Tcl_Size length;
			os << std::string(Tcl_GetStringFromObj(obj.obj, &length));
			return os;
		}
	};

	void ThreadSend(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex);

	struct CallEvent {
		Tcl_Event ev;            /* Must be first for extension */
		TkApp* app;
		std::vector<Obj>* argv;
		int flags;
		std::string* res;
		int* exc;
		Tcl_Condition* done;
	};
	int CallProc(CallEvent* ev, int flags);

	struct CmdClientData {
		TkApp* app;
		Func function;
	};

	int CommandWrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

	struct CommandEvent {
		Tcl_Event ev;            /* Must be first for extension */
		TkApp* app;
		std::string* name;
		int create;
		int* status;
		ClientData data;
		Tcl_Condition* done;
	};

	void CommandDelete(ClientData clientData);

	int CommandProc(CommandEvent* ev, int flags);

	struct VarEvent {
		Tcl_Event ev;            /* Must be first for extension */
		TkApp* app;
		std::string name1;
		std::string name2;
		Obj* obj;
		int flags;
		int mode; // 0=set, 1=get, 2=unset.
		Tcl_Condition* done;
	};

	void SetVar(TkApp* app, std::string name1, std::string name2, Obj* newval, int flags);

	Obj GetVar(TkApp* app, std::string name1, std::string name2, int flags);

	void UnsetVar(TkApp* app, std::string name1, std::string name2, int flags);

	int VarProc(VarEvent* ev, int flags);

	class TkApp {
	public:

		Tcl_Interp* interp = nullptr;
		Tcl_ThreadId thread_id;
		/* We cannot include tclInt.h, as this is internal.
		   So we cache interesting types here. */
		const Tcl_ObjType* OldBooleanType;
		const Tcl_ObjType* BooleanType;
		const Tcl_ObjType* ByteArrayType;
		const Tcl_ObjType* DoubleType;
		const Tcl_ObjType* IntType;
		const Tcl_ObjType* WideIntType;
		const Tcl_ObjType* BignumType;
		const Tcl_ObjType* ListType;
		const Tcl_ObjType* ProcBodyType;
		const Tcl_ObjType* StringType;
		const Tcl_ObjType* UTF32StringType;
		inline TclError tclerror() {
			std::string e = Tcl_GetStringResult(this->interp);
			printf("TclError[%s]\n", e.c_str());
			return TclError(e);
		}

		uint widCnt = 1;
		std::vector<std::pair<Id, std::string>> widgets;
		std::map<void*, void*> commands;

		/* Internal function.
		Get a new identifier. */
		Id newid(Id master = {}) {
			Id id = master;
			id.push_back(std::to_string(this->widCnt++));
			return id;
		}

		//template <class T>
		//inline Obj asobj(T obj) { return Obj(obj); }
		std::wstring unicode_fromobj(Obj object) {
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
		std::string string_fromobj(Obj object) {
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
		inline bool boolean_fromobj(Obj object) {
			Tcl_Obj* obj = object.obj;
			int boolValue;
			if (Tcl_GetBooleanFromObj(this->interp, obj, &boolValue) == TCL_ERROR) throw tclerror();
			return boolValue;
		}
		inline uint int_fromobj(Obj object) {
			Tcl_Obj* obj = object.obj;
			Tcl_WideInt wideValue;
			if (Tcl_GetWideIntFromObj(this->interp, obj, &wideValue) == TCL_OK) {
				if (sizeof(wideValue) <= sizeof(uint))
					return wideValue;
				throw OverflowError("wide int is too long");
			}
			throw tclerror();
		}
		inline double double_fromobj(Obj object) {
			Tcl_Obj* obj = object.obj;
			double doubleValue;
			Tcl_GetDoubleFromObj(this->interp, obj, &doubleValue);
			return doubleValue;
		}

		/* This mutex synchronizes inter-thread command calls. */
		Tcl_Mutex call_mutex;

		/* This is the main entry point for calling a Tcl command.
		   It supports three cases, with regard to threading:
		   Tcl is threaded.
		   1. Caller of the command is in the interpreter thread:
			  Execute the command in the calling thread.
		   2. Caller is in a different thread: Must queue an event to
			  the interpreter thread. */
			  /* Not sure: Allocation of Tcl objects needs to occur in the
				 interpreter thread. We ship the args to the target thread,
				 and perform processing there. */
		std::string call(std::vector<Obj> argv) {
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
			std::string res;
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
				Tcl_Obj** objs = (Tcl_Obj**)(new Tcl_Obj * [argc]);
				for (uint i = 0; i < argc; i++) objs[i] = argv[i].obj;
				if (Tcl_EvalObjv(interp, argc, objs, TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL) == TCL_ERROR) exc = 1;
				res = Tcl_GetStringResult(interp);
				delete[] objs;
			}
#ifdef _DEBUG
			if (res != "") printf("%s\n", res.c_str());
#endif
			if (exc) throw this->tclerror();
			return res;
		}

		/* This mutex synchronizes inter-thread command creation/deletion. */
		Tcl_Mutex command_mutex;

		void createcommand(std::string name, Func function) {
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

		void deletecommand(std::string name) {
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

		Tcl_Mutex var_mutex;

		Obj varinvoke(int mode, std::string name1, std::string name2, Obj obj, int flags) {
			if (Tcl_GetCurrentThread() != this->thread_id) {
				Obj object = obj;
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
		}

		template <class T>
		void setvar(std::string name, T var) {
			varinvoke(0, name, "", Obj(var), TCL_LEAVE_ERR_MSG);
		}
		template <class T>
		void setvar(std::string name1, std::string name2, T var) {
			varinvoke(0, name1, name2, Obj(var), TCL_LEAVE_ERR_MSG);
		}
		template <class T>
		void globalsetvar(std::string name, T var) {
			varinvoke(0, name, "", Obj(var), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}
		template <class T>
		void globalsetvar(std::string name1, std::string name2, T var) {
			varinvoke(0, name1, name2, Obj(var), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}
		Obj getvar(std::string name) {
			return varinvoke(1, name, "", Obj(), TCL_LEAVE_ERR_MSG);
		}
		Obj getvar(std::string name1, std::string name2) {
			return varinvoke(1, name1, name2, Obj(), TCL_LEAVE_ERR_MSG);
		}
		Obj globalgetvar(std::string name) {
			return varinvoke(1, name, "", Obj(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}
		Obj globalgetvar(std::string name1, std::string name2) {
			return varinvoke(1, name1, name2, Obj(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}
		void unsetvar(std::string name) {
			varinvoke(2, name, "", Obj(), TCL_LEAVE_ERR_MSG);
		}
		void unsetvar(std::string name1, std::string name2) {
			varinvoke(2, name1, name2, Obj(), TCL_LEAVE_ERR_MSG);
		}
		void globalunsetvar(std::string name) {
			varinvoke(2, name, "", Obj(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}
		void globalunsetvar(std::string name1, std::string name2) {
			varinvoke(2, name1, name2, Obj(), TCL_LEAVE_ERR_MSG | TCL_GLOBAL_ONLY);
		}

		inline sint getint(Obj obj) {
			return int_fromobj(obj);
		}
		inline sint getint(std::string str) {
			CHECK_STRING_LENGTH(str);
			return int_fromobj(Obj(str));
		}
		inline double getdouble(Obj obj) {
			return double_fromobj(obj);
		}
		inline double getdouble(std::string str) {
			CHECK_STRING_LENGTH(str);
			return double_fromobj(Obj(str));
		}
		inline bool getboolean(Obj obj) {
			return boolean_fromobj(obj);
		}
		inline bool getboolean(std::string str) {
			CHECK_STRING_LENGTH(str);
			int v;
			Tcl_GetBoolean(this->interp, str.c_str(), &v);
			return v == 1;
		}

		std::vector<Obj> splitlist(Obj obj) {
			Tcl_Size objc;
			Tcl_Obj** objv;
			if (Tcl_ListObjGetElements(this->interp, obj.obj, &objc, &objv) == TCL_ERROR) {
				throw this->tclerror();
			}
			std::vector<Obj> v;
			for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
			return v;
		}
		std::vector<Obj> splitlist(std::string str) {
			Tcl_Size objc;
			const char** objv;
			if (Tcl_SplitList(this->interp, str.c_str(), &objc, &objv) == TCL_ERROR) {
				throw this->tclerror();
			}
			std::vector<Obj> v;
			for (Tcl_Size i = 0; i < objc; i++) v.push_back(objv[i]);
			return v;
		}

		TkApp() {
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
		TkApp(const TkApp&) = delete;

		~TkApp() {
			Tcl_DeleteInterp(this->interp);
			Tcl_Finalize();
		}

	};

	inline void ThreadSend(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex) {
		Tcl_MutexLock(mutex);
		Tcl_ThreadQueueEvent(app->thread_id, ev, TCL_QUEUE_TAIL);
		Tcl_ThreadAlert(app->thread_id);
		Tcl_ConditionWait(cond, mutex, nullptr);
		Tcl_MutexUnlock(mutex);
	}

	/* CallProc is the event procedure that is executed in the context of
	   the Tcl interpreter thread. */
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
		*(ev->res) = Tcl_GetStringResult(ev->app->interp);
		/* Wake up calling thread. */
		Tcl_MutexLock(&ev->app->call_mutex);
		Tcl_ConditionNotify(ev->done);
		Tcl_MutexUnlock(&ev->app->call_mutex);
	CLEANUP:
		delete[] objs;
		return 1;
	}

	/* This is the Tcl command that acts as a wrapper for functions or methods. */
	int CommandWrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
		CmdClientData& cd = *(CmdClientData*)clientData;
		std::vector<Obj> objs;
		for (Tcl_Size i = 0; i < objc; i++) {
			objs.push_back(objv[i]);
		}
		Obj res = cd.function(objs);
		if (res.obj)
			Tcl_SetObjResult(cd.app->interp, res.obj);
		else
			Tcl_ResetResult(cd.app->interp);
		return 0;
	}

	void CommandDelete(ClientData clientData) {
		CmdClientData* data = (CmdClientData*)clientData;
		delete data;
	}

	/* CommandProc is the event procedure that is executed to create or 
	   delete a Tcl command. */
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

	void SetVar(TkApp* app, std::string name1, std::string name2, Obj* newval, int flags) {
		Obj ok;
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

	Obj GetVar(TkApp* app, std::string name1, std::string name2, int flags) {
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
		Obj ok;
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

#endif