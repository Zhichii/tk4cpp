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
#define NoneObject _tk4cpp::Object()

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

    //  Stores the identifier for widgets.
    class Id : public std::vector<std::string> {
    public:
        operator std::string() {
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
    typedef Error TypeError;
    typedef Error TclError;

    class TkApp;
    class Object;

    typedef Object _Func(std::vector<Object>&);
    typedef std::function<_Func> Func;

	class Object {
	public:
		Tcl_Obj* obj = nullptr;
		bool is_func = false;
		Func function;
		Object();
		Object(const Object& n);
		Object(Tcl_Obj* n);
		~Object();
		bool operator==(Object o);
		bool operator!=(Object o);
		Object(bool n);
		Object(int n);
		Object(unsigned int n);
		Object(long long n);
		Object(unsigned long long n);
		Object(double n);
		Object(char* n);
		Object(const char* n);
		Object(wchar_t* n);
		Object(const wchar_t* n);
		Object(std::string n);
        Object(std::wstring n);
        Object(Func function);
        Object(std::vector<Object> obj);
		operator std::string();
		operator bool();
		friend std::ostream& operator<<(std::ostream& os, Object& obj);
	};

	void __readargs(std::vector<Object>& vs);
	template <class A>
	void __readargs(std::vector<Object>& vs, A a);
	template <class A, class ... B>
	void __readargs(std::vector<Object>& vs, A a, B... b);

	void ThreadSend(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex);

	struct CallEvent {
		Tcl_Event ev;            /* Must be first for extension */
		TkApp* app;
		std::vector<Object>* argv;
		int flags;
		Object* res;
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
		Object* obj;
		int flags;
		int mode; // 0=set, 1=get, 2=unset.
		Tcl_Condition* done;
	};

	void SetVar(TkApp* app, std::string name1, std::string name2, Object* newval, int flags);

	Object GetVar(TkApp* app, std::string name1, std::string name2, int flags);

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
		TclError tclerror();

		uint widCnt = 1;
		std::vector<std::pair<Id, std::string>> widgets;
		std::map<void*, void*> commands;

		/* Internal function.
		Get a new identifier. */
		Id newid(Id master = {});

		std::wstring unicode_fromobj(Object object);
		std::string string_fromobj(Object object);
		bool boolean_fromobj(Object object);
		uint int_fromobj(Object object);
		double double_fromobj(Object object);

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
		Object call(std::vector<Object> argv);

		/* This mutex synchronizes inter-thread command creation/deletion. */
		Tcl_Mutex command_mutex;

		void createcommand(std::string name, Func function);

		void deletecommand(std::string name);

		Tcl_Mutex var_mutex;

		Object varinvoke(int mode, std::string name1, std::string name2, Object obj, int flags);

		void setvar(std::string name, Object var);
		void setvar(std::string name1, std::string name2, Object var);
		void globalsetvar(std::string name, Object var);
		void globalsetvar(std::string name1, std::string name2, Object var);
		Object getvar(std::string name);
		Object getvar(std::string name1, std::string name2);
		Object globalgetvar(std::string name);
		Object globalgetvar(std::string name1, std::string name2);
		void unsetvar(std::string name);
		void unsetvar(std::string name1, std::string name2);
		void globalunsetvar(std::string name);
		void globalunsetvar(std::string name1, std::string name2);

		sint getint(Object obj);
		sint getint(std::string str);
		double getdouble(Object obj);
		double getdouble(std::string str);
		bool getboolean(Object obj);
		bool getboolean(std::string str);

		std::vector<Object> splitlist(Object obj);
		std::vector<Object> splitlist(std::string str);

		TkApp();
		TkApp(const TkApp&) = delete;

		~TkApp();

	};

}

#endif // TK4CPP__TK4CPP_HEADER