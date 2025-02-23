#include <ctime>
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
		tki::Tk t((tki::Misc*)&tkapp);
		struct STRUCT { tki::TkApp* tkapp; };
		STRUCT cd;
		cd.tkapp = &tkapp;
		Tcl_ThreadId thri;
		Tcl_CreateThread(&thri, [](ClientData _cd)->unsigned {
			STRUCT* cd = (STRUCT*)_cd;
			tki::TkApp& tkapp = *(cd->tkapp);
			tkapp.call({ "font","create","font1","-family","Unifont" });
			tkapp.call({ "ttk::label",".a","-font","font1" });
			tkapp.call({ "pack",".a" });
			size_t start = time(NULL);
			for (;;) {
				tkapp.call({ ".a","config","-text", time(NULL)-start });
				Tcl_Sleep(10);
			}
			return 0;
			}, &cd, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
		Tk_MainLoop();
	}
	return 0;
}