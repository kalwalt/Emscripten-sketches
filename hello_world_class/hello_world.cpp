#include <iostream>

class Hello {
    
public:
    
    Hello() { 
        std::cout << "Hello world!!!\n" << std::endl;
        }

    void saySomething() {
        std::cout << "something" << std::endl;
    }
    
};

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(jshello) {
    class_<Hello>("Hello")
        .constructor<>()
        .function("saySomething", &Hello::saySomething);
}