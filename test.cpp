#define TKI_TCL_ERROR
#include <internal/Basic.hpp>
#include <internal/Object.hpp>
#include <internal/TkApp.hpp>

int main() {
	tki::TkApp app;
	{
		tki::Func f = [&] LAMBDA {
			if (args.size() != 3) {
				err = 1;
				return ("wrong # args: should be \"" + args[0].str() + " numberA number B\"");
			}
			return { args[1].str() + args[2].str() };
		};
		tki::Object connect = f;
		tki::Object m = 1.145;
		tki::Object n = tki::make_list("puts", 1.145);
		tki::Object l = tki::make_list("puts", 1.145);
		std::cout << m << "\n" << n << "\n";
		app.eval({ "ttk::label", ".x", "-text", "999" });
		app.eval({ "pack", ".x" });
		app.eval({ connect, "114", "514" });
		Tcl_Eval(app.interp, ("set v ["+connect.str()+" aaa bbb]").c_str());
		Tcl_Eval(app.interp, ("puts $v"));
		Tk_MainLoop();
	}
	return 0;
}