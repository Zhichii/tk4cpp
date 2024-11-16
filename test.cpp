
#include "tk4cpp._tk4cpp.hpp"
#include "tk4cpp.hpp"
using namespace tk4cpp::constants;

int main() {
	tk4cpp::TkApp tkapp;
	auto n = [&]LAMBDA{
		for (int i = 1; i < argv.size(); i++) printf("%s ", tkapp.string_fromobj(argv[i]).c_str());
		printf("\n");
		return NoneObject;
	};
	tkapp.call({ _tk4cpp::Object{n},"just","a","test"});
	//tk4cpp::Toplevel t(&tkapp, "-width", 50, "-height", 100);
	Tcl_ThreadId test;
	size_t a[2] = { 0,200 };
	Tcl_CreateThread(&test, [](ClientData cd)->unsigned {
		tk4cpp::TkApp& tkapp = *(tk4cpp::TkApp*)cd;
		tkapp.setvar("testText", "333");
		_tk4cpp::Object obj = tkapp.getvar("testText");
		std::cout << "[" << obj << std::endl;
		tkapp.call({ "font","create","font1","-family","Unifont" });
		tkapp.call({ "ttk::label",".a" });
		tkapp.call({ "ttk::label",".b","-textvariable","testText","-font","font1" });
		tkapp.call({ "grid",".a" });
		tkapp.call({ "grid",".b" });
		size_t yyy = 0;
		tkapp.call({ ".a","config","-text",yyy++ });
		Tcl_Sleep(500);
		size_t zzz = 50;
		for (; yyy <= zzz; yyy++) {
			tkapp.call({ ".a","config","-text",yyy });
			Tcl_Sleep(1);
		}
		return 0;
		}, &tkapp, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
	//auto tk = tk4cpp::Tk(NULL);
	//auto frame = tk4cpp::Frame(tk, "-relief", RIDGE, "-borderwidth", 2);
	//frame.pack("-fill", BOTH, "-expand", 1);
	//auto label = tk4cpp::Label(frame, "-text", "Hello, World");
	//label.pack("-fill", X, "-expand", 1);
	//auto button = tk4cpp::Button(frame, "-text", "Exit", "-command", [&]LAMBDA{ tk.destroy(); });
	//button.pack("-side", BOTTOM);
	//tk.mainloop();
	Tk_MainLoop();
	return 0;
}