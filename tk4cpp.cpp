
#include "tk4cpp.hpp"

namespace tk4cpp {

	template <class... Us>
	std::vector<Object> readargs(Us... a) {
		std::vector<Object> vs;
		__readargs(vs, a...);
		return vs;
	}

	void NoDefaultRoot() {
		_support_default_root = false;
		_default_root = NULL;
	}

	Misc* _get_default_root(std::string what) {
		if (!_support_default_root) {
			throw RuntimeError("No master specified and tk4cpp is "
				"configured to not support default root");
		}
		if (_default_root == NULL) {
			if (what != "")
				throw RuntimeError("Too early to " + what + ": no default root window");
			Misc* root = new Tk(NULL);
			assert(_default_root != NULL);
		}
		return _default_root;
	}

	CallWrapper::CallWrapper(Func func, Func subst, Misc* widget) {
		this->func = func;
		this->subst = subst;
		this->widget = widget;
	}
	Object CallWrapper::operator()(std::vector<_tk4cpp::Object> args) {
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
			this->_name = std::to_string(_varnum++);
		if (value != NoneObject)
			this->initialize(value);
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
		return this->_tk->globalsetvar(this->_name, value);
	}
	void Variable::initialize(Object value) {
		return this->_tk->globalsetvar(this->_name, value);
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
	void Variable::trace_info() {

	}

}