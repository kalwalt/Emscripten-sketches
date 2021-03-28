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

void addPoints(int x, int y) {
        emscripten_console_log("starting addPoints...");
        int id;
        Point2d *point = new Point2d();
        point->x = x;
        point->y = y;
        Points *points = new Points();
        id = 0;
        points->point.push_back(*point);
        points->id = id;
        emscripten_console_log("Added points...");
    };

int main() {
    emscripten_console_log("hello !"); 
    addPoints(2, 2);
}