#include <Tki.hpp>

int main() {
	tki::TkApp tkapp;
	{
		tki::Func f = [&] TKIL {
			if (args.size() != 3) {
				TKI_THROW(&tkapp, 
					"wrong # args: should be \"" + args[0].str() + " numberA number B\"");
				return {};
			}
			return { args[1].str() + args[2].str() };
		};
		tki::Object connect = f;
		Tcl_ThreadId thri;
		Tcl_CreateThread(&thri, [](ClientData cd)->unsigned {
			tki::TkApp& tkapp = *(tki::TkApp*)cd;
			tkapp.setvar("ccb", "a b");
			std::cout << "ccb: " << tkapp.getvar("ccb") << "\n";
			tkapp.call({ "font","create","font1","-family","Unifont" });
			tkapp.call({ "ttk::label",".a","-font","font1" });
			tkapp.call({ "pack",".a" });
			size_t yyy = 0;
			tkapp.call({ ".a","config","-text",yyy++ });
			Tcl_Sleep(500);
			size_t zzz = -1;
			for (; yyy <= zzz; yyy++) {
				tkapp.call({ ".a","config","-text",yyy });
				Tcl_Sleep(10);
			}
			return 0;
			}, &tkapp, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
		Tk_MainLoop();
	}
	return 0;
}