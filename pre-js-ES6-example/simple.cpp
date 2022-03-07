#include <iostream>

class Simple {
    
public:
    
    Simple() { 
        std::cout << "This is a simple message!\n" << std::endl;
        }

    void saySomething() {
        std::cout << "something" << std::endl;
    }
    
};

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(simple) {
    class_<Simple>("Simple")
        .constructor<>()
        .function("saySomething", &Simple::saySomething);
}