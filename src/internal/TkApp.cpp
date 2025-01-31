#include <internal/TkApp.hpp>

namespace tk4cpp {

	void TkApp::tcl_error() {
		std::string msg = Tcl_GetStringResult(this->interp);
		throw_error("TclError", msg);
	}

	void thread_send(TkApp* app, Tcl_Event* ev, Tcl_Condition* cond, Tcl_Mutex* mutex) {
		Tcl_MutexLock(mutex);
		Tcl_ThreadQueueEvent(app->thread_id, ev, TCL_QUEUE_TAIL);
		Tcl_ThreadAlert(app->thread_id);
		Tcl_ConditionWait(cond, mutex, nullptr);
		Tcl_MutexUnlock(mutex);
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
			safe_decr_ref(value);

			// "bytearray" type is not registered in Tcl 9.0
			value = Tcl_NewByteArrayObj(NULL, 0);
			this->byte_array_type = value->typePtr;
			safe_decr_ref(value);

			/* TIP 484 suggests retrieving the "int" type without Tcl_GetObjType("int")
				since it is no longer registered in Tcl 9.0. But even though Tcl 8.7
				only uses the "wideInt" type on platforms with 32-bit long, it still has
				a registered "int" type, which FromObj() should recognize just in case. */
			value = Tcl_NewIntObj(0);
			this->int_type = value->typePtr;
			safe_decr_ref(value);
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