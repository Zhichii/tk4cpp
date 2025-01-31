/* The wrapper of Tcl objects. */

#ifndef TK4CPP_INTERNAL_OBJECT
#define TK4CPP_INTERNAL_OBJECT

#include "Basic.hpp"

// Ignore the error of "..." here if it does. 
#define _EXPAND_ARGS(args) (std::vector<Object>{ Object(args)... })

namespace tk4cpp {

    // Increase reference count when object isn't NULL. 
    void safe_incr_refcnt(Tcl_Obj* object);
    // Decrease reference count when object isn't NULL. 
    void safe_decr_refcnt(Tcl_Obj* object);
    
    struct Object;
    typedef Object _Func(std::vector<Object>&);
    typedef std::function<_Func> Func;
    struct Object {
        Tcl_Obj* object = NULL;
        bool not_func = true;
        Func function;
        operator bool();
        ~Object();
        Object();
        Object(const Object& n);
        // Will increase reference count. 
        Object(Tcl_Obj* n);
        bool operator==(Object n);
        bool operator!=(Object n);
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
        operator std::string();
        friend std::ostream& operator<<(std::ostream& os, Object& obj);
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