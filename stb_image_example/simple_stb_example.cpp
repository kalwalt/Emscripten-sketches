#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <emscripten/bind.h>

emscripten::val loadImage(std::string imagePath) {
  int width, height, nrChannels;
  // we flip the image vertically
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    emscripten::val result = emscripten::val::object();
    result.set("data", emscripten::val(emscripten::memory_view<unsigned char>(width * height * nrChannels, data)));
    result.set("width", width);
    result.set("height", height);
    result.set("channels", nrChannels);
    return result;
  } else {
    std::cerr << "Failed to load texture" << std::endl;
    return emscripten::val::null();
  }
}



EMSCRIPTEN_BINDINGS(stb_example) {
  // Register the function with Emscripten. The function name is "loadImage",
  // and it takes a string argument. The function returns a JavaScript object
  // containing the image data, width, height, and number of channels.
  emscripten::function("loadImage", &loadImage);
}


