#include <stdio.h>
#include <iostream>
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

struct PointsManager {
    Points *points;
};

extern "C" {
    //Points *points;

    void addPoints(int x, int y) {
        emscripten_console_log("starting addPoints...");
        PointsManager pm;
        int id;
        Point2d point;
        point.x = x;
        point.y = y;      
        id = 0;
        pm.points->point.push_back(point);
        pm.points->id = id;
        emscripten_console_log("Added points...");
    };

    void readVec() {
        emscripten_console_log("reading vector of points...");
        PointsManager pm;
        std::cout << "x = { ";
        for (Point2d p : pm.points->point) {
            std::cout << p.x << ", ";
        }
        std::cout << "}; \n";
    }

  Point2d getPointAtIndex(int index) {
        emscripten_console_log("reading vector of points at index");
        PointsManager pm;
        return pm.points[index].point.at(index);
    }

}

#include "vectors-bindings.cpp"