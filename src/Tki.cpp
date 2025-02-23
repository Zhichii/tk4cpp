#include "Tki.hpp"

namespace tki {

	Id::operator std::string() {
		std::string _Output;
		for (const std::string& _Iter : (*this)) {
			_Output += "." + _Iter;
		}
		return _Output;
	}

	void NoDefaultRoot() {
		_support_default_root = false;
		_default_root = NULL;
	}

	Misc* _get_default_root(std::string what) {
		if (!_support_default_root) {
			TKI_ERR("RuntimeError", 
				"No master specified and tk4cpp is "
				"configured to not support default root");
		}
		if (_default_root == NULL) {
			if (what != "")
				TKI_ERR("RuntimeError",
					"Too early to " + what + ": no default root window");
			//Misc* root = new Tk(NULL);
			assert(_default_root != NULL);
		}
		return _default_root;
	}

	CallWrapper::CallWrapper(Func func, Func subst, Misc* widget) {
		this->func = func;
		this->subst = subst;
		this->widget = widget;
	}

	Object CallWrapper::operator()(std::vector<Object> args) {
		if (this->subst) this->subst(args);
		this->func(args);
		return {};
	}

	std::string Variable::_default = "";
	Variable::Variable(Misc* master, Object value, std::string name) {
		if (master == NULL)
			master = _get_default_root("create variable");
		//this->_root = master->_root();
		this->_tk = master->tk;
		if (name != "")
			this->_name = name;
		else
			this->_name = "CPP_VAR" + std::to_string(_varnum++);
		if (value.is()) {
			this->initialize(value);
		}
		else
			this->initialize(Variable::_default);
	}
	Variable::~Variable() {
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
	Variable::operator std::string() { return this->_name; }
	void Variable::set(Object value) {
		this->_tk->globalsetvar(this->_name, value);
	}
	void Variable::initialize(Object value) {
		this->_tk->globalsetvar(this->_name, value);
	}
	Object Variable::get() {
		return this->_tk->globalgetvar(this->_name);
	}
	std::string Variable::_register(Func callback) {
		CallWrapper f(callback, {}, NULL);
		std::string cbname = std::to_string((uint)callback.target<_Func>());
		this->_tk->createcommand(cbname, f);
		this->_tclCommands.push_back(cbname);
		return cbname;
	}
	std::string Variable::trace_add(std::string mode, Func callback) {
		std::string cbname = this->_register(callback);
		this->_tk->call({ "trace","add","variable",
			this->_name, mode, std::vector<Object>{ cbname } });
		return cbname;
	}
	void Variable::trace_remove(std::string mode, std::string cbname) {
		this->_tk->call({ "trace","remove","variable",
			this->_name, mode, std::vector<Object>{ cbname } });
	}
	std::vector<std::pair<std::vector<Object>, Object>> Variable::trace_info() {
		std::vector<std::pair<std::vector<Object>, Object>> vvo;
		Object info = this->_tk->call({ "trace","info","variable",this->_name });
		std::vector<Object> vo = this->_tk->splitlist(info);
		for (auto& i : vo) {
			std::vector<Object> item = this->_tk->splitlist(info);
			vvo.push_back(std::make_pair(this->_tk->splitlist(item[0]), item[1]));
		}
		return vvo;
	}

	void Misc::destroy() {
		if (this->_tclCommands.size() != 0) {
			for (const std::string& name : this->_tclCommands) {
				this->tk->deletecommand(name);
			}
			this->_tclCommands = {};
		}
	}

	void Misc::deletecommand(std::string name) {
		this->deletecommand(name);
		for (uint i = 0; i < this->_tclCommands.size(); i++) {
			if (this->_tclCommands[i] == name) {
				this->_tclCommands.erase(this->_tclCommands.begin() + i);
			}
		}
	}

	bool Misc::tk_strictMotif() {
		return this->tk->boolean_fromobj(this->tk->call({ "set","tk_strictMotif" }));
	}

	bool Misc::tk_strictMotif(bool boolean) {
		return this->tk->boolean_fromobj(this->tk->call({ "set","tk_strictMotif",std::to_string(boolean) }));
	}

	void Misc::tk_bisque() {
		this->tk->call({ "tk_bisque" });
	}

	void Misc::wait_variable(std::string name) {
		this->tk->call({ "tkwait","variable",name });
	}
	void Misc::waitvar(std::string name) {
		wait_variable(name);
	}
	
	void Misc::wait_window() {
		this->tk->call({ "tkwait","window",std::string(this->_w) });
	}
	void Misc::wait_window(Misc* window) {
		this->tk->call({ "tkwait","window",std::string(window->_w) });
	}

	void Misc::wait_visibility() {
		this->tk->call({ "tkwait","window",std::string(this->_w) });
	}
	void Misc::wait_visibility(Misc* window) {
		this->tk->call({ "tkwait","window",std::string(window->_w) });
	}

	void Misc::setvar(std::string name, Object value) {
		this->tk->setvar(name, value);
	}
	Object Misc::getvar(std::string name) {
		return this->tk->getvar(name);
	}

}
