/* Interface to evaluate Tcl/Tk commands. Thanks to Python! */

#ifndef TK4CPP_INTERNAL_EVAL
#define TK4CPP_INTERNAL_EVAL

#include "Basic.hpp"
#include "Object.hpp"

namespace tk4cpp {

	struct CallEvent {
		Tcl_Event ev;            /* Must be first for extension */
		TkApp* app;
		std::vector<Object>* argv;
		int flags;
		Object* res;
		int* exc;
		Tcl_Condition* done;
	};
	int cb_call_proc(CallEvent* ev, int flags);

}

#endif