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
		Tk_MainLoop();
	}
	return 0;
}