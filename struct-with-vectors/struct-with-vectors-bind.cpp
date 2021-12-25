#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
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

std::unordered_map<int, Points> PointsManager;
static int gPointsID = 0;

extern "C" {

    int init() {
        int id = gPointsID++;
		Points *pm =  &(PointsManager[id]);
		pm->id = id;
        return pm->id;
    }

    int addPoints(int id, int x, int y) {
        if (PointsManager.find(id) == PointsManager.end()) { return -1; }
		Points *pm = &(PointsManager[id]);
        emscripten_console_log("starting addPoints...");
        Point2d point;
        point.x = x;
        point.y = y;      
        pm->point.push_back(point);
        pm->id = id;
        emscripten_console_log("Added points...");
        return -1;
    };

    int readVec(int id) {
        if (PointsManager.find(id) == PointsManager.end()) { return -1; }
		Points *pm = &(PointsManager[id]);
        emscripten_console_log("reading vector of points...");
        std::cout << "x = { ";
        for (Point2d p : pm->point) {
            std::cout << p.x << ", ";
            std::cout << p.y;
        }
        std::cout << "}; \n";
        return -1;
    }

  Point2d getPointAtIndex(int id, int index) {
        if (PointsManager.find(id) == PointsManager.end()) { return {x: 0, y:0}; }
		Points *pm = &(PointsManager[id]);
        emscripten_console_log("reading vector of points at index");
        return pm->point.at(index);
    }

}

#include "vectors-bindings.cpp"