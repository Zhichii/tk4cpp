/* The wrapper of Tcl objects. */

#ifndef TK4CPP_INTERNAL_OBJECT
#define TK4CPP_INTERNAL_OBJECT

#include "Basic.hpp"

// Ignore the error of "..." here if it does. 
#define _EXPAND_ARGS(args) (std::vector<Object>{ Object(args)... })

namespace tki {

    // Increase reference count when object isn't NULL. 
    void safe_incr_refcnt(Tcl_Obj* object);
    // Decrease reference count when object isn't NULL. 
    void safe_decr_refcnt(Tcl_Obj* object);
    
    template <class T>
    class _FuncWrap : std::function<T> {
    public:
        const void* force_get() {
            return this->_Target(this->target_type());
        }
    };

    struct Object;
    typedef Object _Func(int&,std::vector<Object>&);
    typedef std::function<_Func> Func;
    typedef _FuncWrap<_Func> FuncWrapper;
    struct Object {
        Tcl_Obj* object = NULL;
        bool no_func = true;
        Func function;
        ~Object();
        Object();
        Object(const Object& n);
        Object& operator=(const Object& n);
        // Will increase reference count. 
        Object(Tcl_Obj* n);
        bool operator==(Object n) const;
        bool operator!=(Object n) const;
        Object(bool n);
        Object(sint n);
        Object(uint n);
        Object(slll n);
        Object(ulll n);
        Object(double n);
        Object(const char* n);
        Object(const wchar_t* n);
        Object(std::string n);
        Object(std::wstring n);
        Object(std::vector<Object> obj);
        Object(Func function);
        std::string str() const;
        bool is() const;
        friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    };

    Object copy_object(const Object& o);

    template <class... Ts>
    std::vector<Object> make_list(Ts... arguments) {
        std::vector<Object> ret = { Object(arguments)... };
        // Ignore the error of "..." here if it does. 
        return ret;
    }

}

#endif