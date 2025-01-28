#include <internal/Basic.hpp>

namespace tk4cpp {

    void throw_exception(std::string exception, std::string msg) {
        printf("%s[%s]", exception.c_str(), msg.c_str());
        throw std::exception();
    }

}