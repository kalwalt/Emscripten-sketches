#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(struct_bindings) {

    function("addPoints", &addPoints);
    value_array<Point2d>("Point2d")
    .element(&Point2d::x)
    .element(&Point2d::y);
    value_object<Points>("Points")
    .field("point", &Points::point)
    .field("id", &Points::id);
    register_vector<Point2d>("vector<Point2d>");
    
}