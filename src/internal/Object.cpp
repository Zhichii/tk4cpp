#include <internal/Object.hpp>

namespace tki {

	void safe_incr_refcnt(Tcl_Obj* object) {
		if (object) { Tcl_IncrRefCount(object); }
	}
	void safe_decr_refcnt(Tcl_Obj* object) {
		if (object) { Tcl_DecrRefCount(object); }
	}

	Object::operator bool() { return this->object; }
	Object::~Object() {
		safe_decr_refcnt(this->object);
	}
	Object::Object() {
		this->object = NULL;
		this->no_func = true;
	}
	Object::Object(const Object& n) {
		this->object = n.object;
		this->no_func = n.no_func;
		this->function = n.function;
		safe_incr_refcnt(this->object);
	}
	Object& Object::operator=(const Object& n) {
		this->object = n.object;
		this->no_func = n.no_func;
		this->function = n.function;
		safe_incr_refcnt(this->object);
		return *this;
	}
	Object::Object(Tcl_Obj* n) {
		this->object = n;
		this->no_func = true;
		safe_incr_refcnt(this->object);
	}
	bool Object::operator==(Object n) {
		if (this->no_func) return this->object == n.object;
		THROW_EXCEPTION("UnreachableError", "You shouldn't do this. ");
		return false;
	}
	bool Object::operator!=(Object n) {
		if (this->no_func) return this->object != n.object;
		THROW_EXCEPTION("UnreachableError", "You shouldn't do this. ");
		return false;
	}
	Object::Object(bool n) {
		this->object = Tcl_NewBooleanObj(n);
		this->no_func = true;
		safe_incr_refcnt(this->object);
	}
	Object::Object(sint n) {
		mp_int i;
		mp_init(&i);
		mp_set_ll(&i, n);
		this->object = Tcl_NewBignumObj(&i);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		mp_clear(&i);
	}
	Object::Object(uint n) {
		mp_int i;
		mp_init(&i);
		mp_set_ull(&i, n);
		this->object = Tcl_NewBignumObj(&i);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		mp_clear(&i);
	}
	Object::Object(slll n) {
		mp_int i;
		mp_init(&i);
		mp_set_ll(&i, n);
		this->object = Tcl_NewBignumObj(&i);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		mp_clear(&i);
	}
	Object::Object(ulll n) {
		mp_int i;
		mp_init(&i);
		mp_set_ull(&i, n);
		this->object = Tcl_NewBignumObj(&i);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		mp_clear(&i);
	}
	Object::Object(double n) {
		this->object = Tcl_NewDoubleObj(n);
		this->no_func = true;
		safe_incr_refcnt(this->object);
	}
	Object::Object(const char* n) {
		this->object = Tcl_NewStringObj(n, -1);
		this->no_func = true;
		safe_incr_refcnt(this->object);
	}
	Object::Object(const wchar_t* n) {
		ulll len = wcslen(n);
		Tcl_UniChar* m = new Tcl_UniChar[len];
		for (ulll i = 0; i < len; i++) m[i] = n[i];
		this->object = Tcl_NewUnicodeObj(m, -1);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		delete[] m;
	}
	Object::Object(std::string n) {
		this->object = Tcl_NewStringObj(n.c_str(), n.size());
		this->no_func = true;
		safe_incr_refcnt(this->object);
	}
	Object::Object(std::wstring n) {
		ulll len = n.size();
		Tcl_UniChar* m = new Tcl_UniChar[len];
		for (ulll i = 0; i < len; i++) m[i] = n[i];
		this->object = Tcl_NewUnicodeObj(m, -1);
		this->no_func = true;
		safe_incr_refcnt(this->object);
		delete[] m;
	}
	Object::Object(std::vector<Object> n) {
		ulll len = n.size();
		Tcl_Obj** m = new Tcl_Obj*[len];
		for (ulll i = 0; i < len; i++) m[i] = n[i].object;
		this->object = Tcl_NewListObj(len, m);
		delete[] m;
		safe_incr_refcnt(this->object);
	}
	Object::Object(Func function) {
		FuncWrapper* func = (FuncWrapper*)&function;
		std::string id = "CPP_FUNC" + std::to_string((ulll)(func->force_get()));
		this->object = Tcl_NewStringObj(id.c_str(), id.size());
		this->no_func = true;
		safe_incr_refcnt(this->object);
		this->function = function;
		this->no_func = false;
	}
	std::string Object::str() {
		if (!this->object) return "";
		int len;
		char* m = Tcl_GetStringFromObj(this->object, &len);
		return std::string(m, len);
	}
	std::ostream& operator<<(std::ostream& os, Object& obj) {
		os << obj.str();
		return os;
	}

	Object copy_object(const Object& o) {
		return Object(Tcl_DuplicateObj(o.object));
	}

}