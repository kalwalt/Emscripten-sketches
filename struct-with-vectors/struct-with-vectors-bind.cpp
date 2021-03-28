#include <stdio.h>
#include <string>
#include <vector>
#include <emscripten.h>
#include <emscripten/html5.h>

struct Point2d {
    int x;
    int y;
};

struct Points {
      std::vector<Point2d> point;
      int id;
};

extern "C" {

    void addPoints(int x, int y) {
        emscripten_console_log("starting addPoints...");
        int id;
        Point2d point;
        point.x = x;
        point.y = y;
        Points *points;
        id = 0;
        points->point.push_back(point);
        points->id = id;
        emscripten_console_log("Added points...");
    };

}

#include "vectors-bindings.cpp"