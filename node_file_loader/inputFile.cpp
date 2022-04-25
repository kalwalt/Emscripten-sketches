#include <emscripten.h>
#include <emscripten/bind.h>
#include <fstream>
#include <iostream>

void InputFile(const std::string &fileName) {
   EM_ASM({
     FS.mkdir('/temp'); 
     FS.mount(NODEFS, {root : '.'}, '/temp');
     var fn = UTF8ToString($0);
     var test = FS.readFile('/temp/' + fn, {encoding: 'utf8'});
     console.log("Text from test file: ", test);
     },
     fileName.c_str()
     );
   std::ifstream is(std::string("/temp/") + fileName, std::ifstream::binary);
   if (is.fail())
   {
      std::cerr << "ERROR, Could not open " << fileName << std::endl;
      exit(1);
   } else {
      std::cout << "Reading file..." << std::endl;
   }
}

EMSCRIPTEN_BINDINGS(Module) { emscripten::function("InputFile", &InputFile); }