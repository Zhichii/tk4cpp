/* The wrapper of Tcl_Interp* and some other things. */

#ifndef TK4CPP_INTERNAL_TKAPP
#define TK4CPP_INTERNAL_TKAPP

#include "Basic.hpp"
#include "Object.hpp"

#ifndef TKI_TCL_ERROR
// Will crash if there's an Tcl error.
#define THROW_TCL_ERROR(PTKAPP) { printf("%s[%s]", "TclError", Tcl_GetStringResult(PTKAPP->interp)); throw std::exception(); }
#else
// Won't crash.
#define THROW_TCL_ERROR(PTKAPP) { printf("%s[%s]", "TclError", Tcl_GetStringResult(PTKAPP->interp)); }
#endif

namespace tki {

	struct Event : Tcl_Event {
		Tcl_Condition* done;
		TkApp* app;
	};

	void thread_send_if(TkApp* app, Tcl_Event* ev_, Tcl_Mutex* mutex);

	struct CallEvent : Event {
		std::vector<Object>* argv;
		Object* ret;
		int* err;
		int flags;
	};
	int call_proc(CallEvent* ev, int flags);

	struct CmdClientData {
		TkApp* app;
		Func function;
	};
	int command_wrapper(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);
	
	struct CommandEvent : Event {
		std::string* name;
		ClientData data;
		int* err;
		int create;
	};
	void command_delete(ClientData clientData);
	int command_proc(CommandEvent* ev, int flags);

	struct TkApp {

		Tcl_Interp* interp = nullptr;
		Tcl_ThreadId thread_id;
		/* We cannot include tclInt.h, as this is internal.
		   So we cache interesting types here. */
		const Tcl_ObjType* old_boolean_type;
		const Tcl_ObjType* boolean_type;
		const Tcl_ObjType* byte_array_type;
		const Tcl_ObjType* double_type;
		const Tcl_ObjType* int_type;
		const Tcl_ObjType* wide_int_type;
		const Tcl_ObjType* bignum_type;
		const Tcl_ObjType* list_type;
		const Tcl_ObjType* proc_body_type;
		const Tcl_ObjType* string_type;
		const Tcl_ObjType* utf32_string_type;

		std::wstring unicode_fromobj(Object object);
		std::string string_fromobj(Object object);
		bool boolean_fromobj(Object object);
		sint int_fromobj(Object object);
		double double_fromobj(Object object);

		/* This mutex synchronizes inter-thread command calls. */
		Tcl_Mutex call_mutex;

		/* This is the main entry point for calling a Tcl command.
		   It supports three cases, with regard to threading:
		   Tcl is threaded.
		   1. Caller of the command is in the interpreter thread:
			  Execute the command in the calling thread.
		   2. Caller is in a different thread: Must queue an event to
			  the interpreter thread. 
		   Returns void. */
		Object eval(std::vector<Object> argv);

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

#endif