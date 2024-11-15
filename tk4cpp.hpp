/* Wrapper functions for and interface to Tcl/Tk. Thanks to Python! 

Tk4cpp provides classes which allow the display, positioning and
control of widgets. Toplevel widgets are Tk and Toplevel. Other
widgets are Frame, Label, Entry, Text, Canvas, Button, Radiobutton,
Checkbutton, Scale, Listbox, Scrollbar, OptionMenu, Spinbox
LabelFrame and PanedWindow.

Widgets are positioned with one of the geometry managers Place, Pack
or Grid. These managers can be called with methods place, pack, grid
available in every Widget.

Actions are bound to events by resources (e.g. keyword argument
command) or with the method bind.

Example (Hello, World):
#include "tk4cpp.hpp"
using namespace tk4cpp::constants;
int main()
{
	tk4cpp::initialize();
	auto tk = tk4cpp::Tk(NULLID);
	auto frame = tk4cpp::Frame(tk, "-relief", RIDGE, "-borderwidth", S(2));
	frame.pack("-fill", BOTH, "-expand", S(1));
	auto label = tk4cpp::Label(frame, "-text", "Hello, World");
	label.pack("-fill", X, "-expand", S(1));
	auto button = tk4cpp::Button(frame,"-text","Exit","-command",S(tk.destroy));
	button.pack("-side", Bottom);
	tk.mainloop();
	tk4cpp::finalize();
	return 0;
}
 */

#ifndef TK4CPP_HEADER
#define TK4CPP_HEADER

#include "tk4cpp.constants.hpp"
#include "tk4cpp._tk4cpp.hpp"

#define LAMBDA (std::vector<_tk4cpp::Obj> argv)->_tk4cpp::Obj

namespace tk4cpp {

	using sint = _tk4cpp::sint;
	using uint = _tk4cpp::uint;
	using TkApp = _tk4cpp::TkApp;
	using Obj = _tk4cpp::Obj;
	using Id = _tk4cpp::Id;
	using _Func = _tk4cpp::_Func;
	using Func = _tk4cpp::Func;

	using RuntimeError = _tk4cpp::RuntimeError;

	class Tk;
	class Misc;

	bool _support_default_root = true;
	Misc* _default_root = NULL;

	/* Inhibit setting of default root window.

	Call this function to inhibit that the first instance of
	Tk is used for windows without an explicit parent window.
	*/
	void NoDefaultRoot() {
		_support_default_root = false;
		_default_root = NULL;
	}

	Misc* _get_default_root(std::string what = "");

	inline void __readargs(std::vector<Obj>& vs) {}
	template <class A>
	inline void __readargs(std::vector<Obj>& vs, A a) {
		vs.push_back(a);
	}
	template <class A, class ... B>
	inline void __readargs(std::vector<Obj>& vs, A a, B... b) {
		vs.push_back(a);
		__readargs(vs, b...);
	}
	template <class... Us>
	inline std::vector<Obj> readargs(Us... a) {
		std::vector<Obj> vs;
		__readargs(vs, a...);
		return vs;
	}

	uint _varnum = 0;

	/* Internal class. Stores function to call when some user
    defined Tcl function is called e.g. after an event occurred. */
	class CallWrapper {
	public:

		Func func;
		Func subst;
		Misc* widget;

		// Store FUNC, SUBST and WIDGET as members.
		CallWrapper(Func func, Func subst, Misc* widget) {
			this->func = func;
			this->subst = subst;
			this->widget = widget;
		}

		// Apply first function SUBST to arguments, than FUNC.
		Obj operator()(std::vector<_tk4cpp::Obj> args) {
			if (this->subst) this->subst(args);
			this->func(args);
			return {};
		}

	};

	/* Class to define value holders for e.g. buttons.

    Subclasses StringVar, IntVar, DoubleVar, BooleanVar are specializations
    that constrain the type of the value returned from get(). */
	class Variable {
	public:
		static std::string _default;
		Tk* _root = NULL;
		TkApp* _tk = NULL;
		std::string _name;
		std::vector<std::string> _tclCommands;

		/* Construct a variable

        MASTER can be given as master widget.
        VALUE is an optional value (defaults to "")
        NAME is an optional Tcl name (defaults to PY_VARnum).

        If NAME matches an existing variable and VALUE is omitted
        then the existing value is retained.
		*/
		template <class T>
		Variable(Misc* master, T value = {}, std::string name = "");

		// Unset the variable in Tcl. 
		~Variable() {
			if (this->_tk == NULL) return;
			if (this->_tk->getboolean(this->_tk->call({ "info", "exists", this->_name })))
				this->_tk->globalunsetvar(this->_name);
			if (this->_tclCommands.size() != 0) {
				for (const auto& i : this->_tclCommands) {
					this->_tk->deletecommand(i);
				}
				this->_tclCommands = {};
			}
		}

		// Return the name of the variable in Tcl. 
		operator std::string() { return this->_name; }

		// Set the variable to VALUE.
		template <class T>
		void set(T value) {
			return this->_tk->globalsetvar(this->_name, value);
		}
		template <class T>
		void initialize(T value) {
			return this->_tk->globalsetvar(this->_name, value);
		}

		// Return value of variable.
		Obj get() {
			return this->_tk->globalgetvar(this->_name);
		}

		std::string _register(Func callback) {
			CallWrapper f(callback, {}, NULL);
			std::string cbname = std::to_string((uint)callback.target<_Func>());
			this->_tk->createcommand(cbname, f);
			this->_tclCommands.push_back(cbname);	
			return cbname;
		}

		/* Define a trace callback for the variable.

        Mode is one of "read", "write", "unset", or a list or tuple of
        such strings.
        Callback must be a function which is called when the variable is
        read, written or unset.

        Return the name of the callback. */
		std::string trace_add(std::string mode, Func callback) {
			std::string cbname = this->_register(callback);
			this->_tk->call({ "trace","add","variable",
				this->_name, mode, std::vector<Obj>{ cbname } });
			return cbname;
		}

		/* Delete the trace callback for a variable.

        Mode is one of "read", "write", "unset" or a list or tuple of
        such strings.  Must be same as were specified in trace_add().
        cbname is the name of the callback returned from trace_add(). */

	};
	std::string Variable::_default = "";

	/* Internal class.
	Base class which defines methods common for interior widgets. */
	class Misc {
	public:

		TkApp* tk;

		// used for generating child widget names
		int _last_child_ids = -1;

		// XXX font command?
		std::vector<std::string> _tclCommands;

		Id _w;

		/* Internal function.
		Delete all Tcl commands created for this widget in the Tcl interpreter. */
		void destroy() {
			if (this->_tclCommands.size() != 0) {
				for (const std::string& name : this->_tclCommands) {
					this->tk->deletecommand(name);
				}
				this->_tclCommands = {};
			}
		}

		/* Internal function.
		Delete the Tcl command provided in NAME. */
		void deletecommand(std::string name) {
			this->deletecommand(name);
			for (uint i = 0; i < this->_tclCommands.size(); i++) {
				if (this->_tclCommands[i] == name) {
					this->_tclCommands.erase(this->_tclCommands.begin() + i);
				}
			}
		}
		
		/* Set Tcl internal variable, whether the look and feel
		should adhere to Motif.
		A parameter of 1 means adhere to Motif (e.g. no color
		change if mouse passes over slider). */
		bool tk_strictMotif() {
			return std::atoi(this->tk->call({ "set","tk_strictMotif" }).c_str());
		}

		/* Set Tcl internal variable, whether the look and feel
		should adhere to Motif.
		Returns the set value. */
		bool tk_strictMotif(int boolean) {
			return std::atoi(this->tk->call({ "set","tk_strictMotif",std::to_string(boolean)}).c_str());
		}

		// Change the color scheme to light brown as used in Tk 3.6 and before.
		void tk_bisque() {
			this->tk->call({ "tk_bisque" });
		}

		/* Set a new color scheme for all widget elements.
        A single color as argument will cause that all colors of Tk
        widget elements are derived from this.
        Alternatively several keyword parameters and its associated
        colors can be given. The following keywords are valid:
        activeBackground, foreground, selectColor,
        activeForeground, highlightBackground, selectBackground,
        background, highlightColor, selectForeground,
        disabledForeground, insertBackground, troughColor. */
		template <class ... Us>
		void tk_setPalette(Us... args) {
			std::vector<Obj> v;
			v = { "tk_setPalette" };
			std::vector<Obj> argv = readargs(args...);;
			for (const auto& i : argv) v.push_back(i);
			this->tk->call(v);
		}

		/* Wait until the variable is modified.
        A parameter of type IntVar, StringVar, DoubleVar or
        BooleanVar must be given. */
		void wait_variable(std::string name = "CPP_VAR") {
			this->tk->call({ "tkwait","variable",name });
		}
		inline void waitvar(std::string name = "CPP_VAR") {
			wait_variable(name);
		}
		// XXX b/w compat

		// Wait until this is destroyed.
		void wait_window() {
			this->tk->call({ "tkwait","window",this->_w.to_string()});
		}
		// Wait until a WIDGET is destroyed.
		template <class T>
		void wait_window(T window) {
			this->tk->call({ "tkwait","window",window._w });
		}

		/* Wait until the visibility of this changes
		(e.g. this appears). */
		void wait_visibility() {
			this->tk->call({ "tkwait","window",this->_w.to_string() });
		}
		/* Wait until the visibility of a WIDGET changes
		(e.g. it appears). */
		template <class T>
		void wait_visibility(T window) {
			this->tk->call({ "tkwait","window",window._w });
		}
		
		// Set Tcl variable NAME to VALUE.
		template <class T>
		void setvar(std::string name = "CPP_VAR", T value = "1") {
			this->tk->setvar(name, value);
		}
		// Return value of Tcl variable NAME.
		Obj getvar(std::string name = "CPP_VAR") {
			return this->tk->getvar(name);
		}
		

	};

	template <class T>
	Variable::Variable(Misc* master, T value, std::string name) {
		if (master == NULL)
			master = _get_default_root("create variable");
		//this->_root = master->_root();
		this->_tk = master->tk;
		if (name != "")
			this->_name = name;
		else
			this->_name = std::to_string(_varnum++);
		if (value != NULL)
			this->initialize(value);
		else
			this->initialize(Variable::_default);
	}

	class Tk : public Misc {
	public:
		template <class ... Us>
		Tk(Misc* tk, Us... args) {
			this->tk = tk->tk;
			this->_w = this->tk->newid();
			std::vector<Obj> v;
			v = { "toplevel", this->_w.to_string() };
			std::vector<Obj> argv = readargs(args...);
			for (const auto& i : argv) v.push_back(i);
			this->tk->call(v);
		}
	};

	class Toplevel : public Misc {
	public:
		template <class ... Us>
		Toplevel(Misc* tk, Us... args) {
			this->tk = tk->tk;
			this->_w = this->tk->newid();
			std::vector<Obj> v;
			v = { "toplevel", this->_w.to_string() };
			std::vector<Obj> argv = readargs(args...);
			for (const auto& i : argv) v.push_back(i);
			this->tk->call(v);
		}
	};

	inline Misc* _get_default_root(std::string what) {
		if (!_support_default_root) {
			throw RuntimeError("No master specified and tk4cpp is "
				"configured to not support default root");
		}
		if (_default_root == NULL) {
			if (what != "")
				throw RuntimeError("Too early to "+what+": no default root window");
			Misc* root = new Tk(NULL);
			assert(_default_root != NULL);
		}
		return _default_root;
	}

}

#endif