
#include <internal/Basic.hpp>
#include <internal/Object.hpp>

namespace {
	int n;
}

int main() {
	n = 5;
	Tcl_Interp* interp = Tcl_CreateInterp();
	Tcl_Init(interp);
	Tk_Init(interp);
	{
		tk4cpp::Object m = 1.145;
		tk4cpp::Object n = tk4cpp::_expand_args("puts", 1.145);
		std::cout << m << "\n" << n << "\n";
		//Tcl_EvalObjEx(interp, n.object, 0);
	}
	Tcl_Finalize();
	return 0;
}