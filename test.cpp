
#include <internal/Basic.hpp>
#include <internal/Object.hpp>
#include <internal/TkApp.hpp>

int main() {
	tk4cpp::TkApp app;
	{
		tk4cpp::Object m = 1.145;
		tk4cpp::Object n = tk4cpp::make_list("puts", 1.145);
		std::cout << m << "\n" << n << "\n";
		app.eval("ttk::label", ".x", "-text", "999");
		app.eval("pack", ".x");
		//Tcl_EvalObjEx(app.interp, tk4cpp::Object(tk4cpp::make_list("ttk::label", ".x", "-text", "999")).object, 0);
		//Tcl_EvalObjEx(app.interp, tk4cpp::Object(tk4cpp::make_list("pack", ".x")).object, 0);
		Tk_MainLoop();
	}
	return 0;
}