#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <math.h>

class Pow {
    int e, b, result;
    void setExponent(int exponent){
        if (exponent<=0 || exponent>100) {
            puts("exponent not in range!");
            throw std::range_error("exponent should be in [0, 100]") ;
        }    
        e = exponent ;
    }
    
    public:
    Pow(int base, int exponent){
        setExponent(exponent);
        result = pow(base, e);
    }
    auto getResult(){
        return result;
    }
};

namespace error {
std::string getExceptionMessage(int exceptionPtr) {
  return std::string(reinterpret_cast<std::exception *>(exceptionPtr)->what());
}
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(range_error) {
    class_<Pow>("Pow")
        .constructor<int, int>()
        .function("getResult", &Pow::getResult);
    
    function("getExceptionMessage", &error::getExceptionMessage);
}