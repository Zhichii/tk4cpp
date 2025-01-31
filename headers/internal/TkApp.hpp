/*  */

#ifndef TK4CPP_INTERNAL_TKAPP
#define TK4CPP_INTERNAL_TKAPP

#include "Basic.hpp"
#include "Object.hpp"
#include "Eval.hpp"

namespace tk4cpp {

	void thread_send(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex);

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
		void tcl_error();

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
			  the interpreter thread. */
			  /* Not sure: Allocation of Tcl objects needs to occur in the
				 interpreter thread. We ship the args to the target thread,
				 and perform processing there. */
		template <class... Ts>
		Object eval(Ts... argx) {
			std::vector<Object> argv = { (argx)... };
			for (const auto& i : argv) {
				if (i.not_func) continue;
				//this->createcommand(i.str, i.function);
			}
			ulll argc = argv.size();
#ifdef _DEBUG
			printf("> ");
			for (ulll i = 0; i < argc; i++) {
				printf("%s ", std::string(argv[i]).c_str());
			}
			printf("\n");
#endif
			Object res;
			int exc = 0;
			CallEvent* ev = (CallEvent*)attemptckalloc(sizeof(CallEvent));
			if (ev == nullptr) throw_error("MemoryError", "no memory");
			memset(ev, 0, sizeof(CallEvent));
			ev->ev.proc = (Tcl_EventProc*)cb_call_proc;
			ev->app = this;
			ev->argv = &argv;
			ev->flags = TCL_EVAL_DIRECT | TCL_EVAL_GLOBAL;
			ev->res = &res;
			ev->exc = &exc;
			ev->done = NULL;
			if (Tcl_GetCurrentThread() != this->thread_id) {
				Tcl_Condition cond = NULL;
				ev->done = &cond;
				thread_send(this, (Tcl_Event*)ev, &cond, &this->call_mutex);
				Tcl_ConditionFinalize(&cond);
			}
			else {
				cb_call_proc(ev, 0);
				ckfree(ev);
			}
#ifdef _DEBUG
			if (res) printf("%s\n", std::string(res).c_str());
#endif
			if (res) Tcl_IncrRefCount(res.object);
			if (exc) this->tcl_error();
			return res;
		}

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