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

}
