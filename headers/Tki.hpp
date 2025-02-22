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
#include <tki/Tki.h>
using namespace tki::constants;
int main()
{
	auto tk = tki::Tk(NULL);
	auto frame = tki::Frame(tk, "-relief", RIDGE, "-borderwidth", 2);
	frame.pack("-fill", BOTH, "-expand", 1);
	auto label = tki::Label(frame, "-text", "Hello, World");
	label.pack("-fill", X, "-expand", 1);
	auto button = tki::Button(frame,"-text","Exit","-command", TKIF(tk.destroy));
	button.pack("-side", Bottom);
	tk.mainloop();
	return 0;
}
 */

#ifndef TK4CPP_TKI
#define TK4CPP_TKI

#include "internal/Basic.hpp"
#include "internal/Object.hpp"
#include "internal/TkApp.hpp"
#include "Constants.hpp"

#define TKIL (std::vector<tki::Object>& args)->tki::Object

namespace tki {

	//  Stores the identifier for widgets.
	class Id : public std::vector<std::string> {
	public:
		operator std::string();
	};

	class Tk;
	class Misc;

	static bool _support_default_root = true;
	static Misc* _default_root = NULL;

	/* Inhibit setting of default root window.

	Call this function to inhibit that the first instance of
	Tk is used for windows without an explicit parent window.
	*/
	void NoDefaultRoot();

	Misc* _get_default_root(std::string what = "");

	static uint _varnum = 0;

	/* Internal class. Stores function to call when some user
	defined Tcl function is called e.g. after an event occurred. */
	class CallWrapper {
	public:

		Func func;
		Func subst;
		Misc* widget;

		// Store FUNC, SUBST and WIDGET as members.
		CallWrapper(Func func, Func subst, Misc* widget);

		// Apply first function SUBST to arguments, than FUNC.
		Object operator()(std::vector<Object> args);

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
		Variable(Misc* master, Object value = {}, std::string name = "");

		// Unset the variable in Tcl. 
		~Variable();

		// Return the name of the variable in Tcl. 
		operator std::string();

		// Set the variable to VALUE.
		void set(Object value);
		void initialize(Object value);

		// Return value of variable.
		Object get();

		std::string _register(Func callback);

		/* Define a trace callback for the variable.

		Mode is one of "read", "write", "unset", or a list or tuple of
		such strings.
		Callback must be a function which is called when the variable is
		read, written or unset.

		Return the name of the callback. */
		std::string trace_add(std::string mode, Func callback);

		/* Delete the trace callback for a variable.

		Mode is one of "read", "write", "unset" or a list or tuple of
		such strings.  Must be same as were specified in trace_add().
		cbname is the name of the callback returned from trace_add(). */
		void trace_remove(std::string mode, std::string cbname);

		// Return all trace callback information.
		std::vector<std::pair<std::vector<Object>, Object>> trace_info();

	};

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
			return this->tk->boolean_fromobj(this->tk->call({ "set","tk_strictMotif" }));
		}

		/* Set Tcl internal variable, whether the look and feel
		should adhere to Motif.
		Returns the set value. */
		bool tk_strictMotif(bool boolean) {
			return this->tk->boolean_fromobj(this->tk->call({ "set","tk_strictMotif",std::to_string(boolean) }));
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
			std::vector<Object> v;
			v = { "tk_setPalette" };
			std::vector<Object> argv = readargs(args...);;
			for (const auto& i : argv) v.push_back(i);
			this->tk->call(v);
		}

		/* Wait until the variable is modified.
		A parameter of type IntVar, StringVar, DoubleVar or
		BooleanVar must be given. */
		void wait_variable(std::string name = "CPP_VAR") {
			this->tk->call({ "tkwait","variable",name });
		}
		void waitvar(std::string name = "CPP_VAR") {
			wait_variable(name);
		}
		// XXX b/w compat

		// Wait until this is destroyed.
		void wait_window() {
			this->tk->call({ "tkwait","window",std::string(this->_w) });
		}
		// Wait until a WIDGET is destroyed.
		void wait_window(Misc* window) {
			this->tk->call({ "tkwait","window",std::string(window->_w) });
		}

		/* Wait until the visibility of this changes
		(e.g. this appears). */
		void wait_visibility() {
			this->tk->call({ "tkwait","window",std::string(this->_w) });
		}
		/* Wait until the visibility of a WIDGET changes
		(e.g. it appears). */
		void wait_visibility(Misc* window) {
			this->tk->call({ "tkwait","window",std::string(window->_w) });
		}

		// Set Tcl variable NAME to VALUE.
		void setvar(std::string name = "CPP_VAR", Object value = "1") {
			this->tk->setvar(name, value);
		}
		// Return value of Tcl variable NAME.
		Object getvar(std::string name = "CPP_VAR") {
			return this->tk->getvar(name);
		}


	};

}

#endif