#include "tk4cpp.hpp"
using namespace tk4cpp::constants;

template<class T>
std::function<T> func(T a) {
	return a;
}

int taest(int a, long long b, std::string c) {
	printf("taest\n");
	return 0;
}

class FFF {
public:
	void* func;
	template <class T>
	FFF(T n) { this->func = (void*)n; }
};
typedef int TTT(long long, int, std::string, int);

int main() {
	tk4cpp::TkApp tkapp;
	auto n = [&]LAMBDA{
		for (int i = 1; i < argv.size(); i++) printf("%s ", tkapp.string_fromobj(argv[i]).c_str());
		printf("\n");
		return NONE;
	};
	tkapp.call({ _tk4cpp::Obj{n},"just","a","test"});
	//tk4cpp::Toplevel t(&tkapp, "-width", 50, "-height", 100);
	Tcl_ThreadId test;
	size_t a[2] = { 0,200 };
	Tcl_CreateThread(&test, [](ClientData cd)->unsigned {
		tk4cpp::TkApp& tkapp = *(tk4cpp::TkApp*)cd;
		tkapp.setvar("testText", "333");
		_tk4cpp::Obj obj = tkapp.getvar("testText");
		std::cout << "aaa[" << obj << std::endl;
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
	//auto frame = tk4cpp::Frame(tk, "-relief", RIDGE, "-borderwidth", S(2));
	//frame.pack("-fill", BOTH, "-expand", S(1));
	//auto label = tk4cpp::Label(frame, "-text", "Hello, World");
	//label.pack("-fill", X, "-expand", S(1));
	//auto button = tk4cpp::Button(frame, "-text", "Exit", "-command", SF(tk.destroy));
	//button.pack("-side", BOTTOM);
	//tk.mainloop();
	Tk_MainLoop();
	return 0;
}