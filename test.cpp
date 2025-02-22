#define TKI_TCL_ERROR
#include <internal/Basic.hpp>
#include <internal/Object.hpp>
#include <internal/TkApp.hpp>

int main() {
	tki::TkApp tkapp;
	{
		tki::Func f = [&] LAMBDA {
			if (args.size() != 3) {
				err = 1;
				return ("wrong # args: should be \"" + args[0].str() + " numberA number B\"");
			}
			return { args[1].str() + args[2].str() };
		};
		tki::Object connect = f;
		Tcl_ThreadId thri;
		Tcl_CreateThread(&thri, [](ClientData cd)->unsigned {
			tki::TkApp& tkapp = *(tki::TkApp*)cd;
			tkapp.setvar("ccb", "a b");
			std::cout << "ccb: " << tkapp.getvar("ccb") << "\n";
			tkapp.eval({ "font","create","font1","-family","Unifont" });
			tkapp.eval({ "ttk::label",".a","-font","font1" });
			tkapp.eval({ "pack",".a" });
			size_t yyy = 0;
			tkapp.eval({ ".a","config","-text",yyy++ });
			Tcl_Sleep(500);
			size_t zzz = -1;
			for (; yyy <= zzz; yyy++) {
				tkapp.eval({ ".a","config","-text",yyy });
				Tcl_Sleep(10);
			}
			return 0;
			}, &tkapp, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
		Tk_MainLoop();
	}
	return 0;
}