#include <cstdio>
#include "Classes/Classes.h"
#include <iostream>

/*
TODO take into account:
    Model Size
    pixel Canvas (is there a size of window option?)
    Are materials relevant?
    freeform curves/surfaces are a thing (find an example to use as well maybe the blender chimp?) NURBS command starts with vp
    more than 3 vertexes per face

    Sobel Filter for edge detection

TODO things to add:
    CameraObject
    - gets object to look at
    - lightsource
    - rotational functions



*/

int main(){
  std::ios::sync_with_stdio(false);
    std::ifstream f("../Cube.obj");
    if (!f.is_open()){
        std::cerr << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }

    Model m(f);

//    std::cout << m.toOBJ();



//    int x = 120;
//    int y = 30;
//    Canvas cvs(x, y);
//    std::cout << "Canvas Tests:\n" << x << ":" << y << std::endl <<
//    cvs.getNDCx(0) << std::endl <<
//    cvs.getNDCx(x) << std::endl <<
//    cvs.getNDCy(0) << std::endl <<
//    cvs.getNDCy(y) << std::endl <<
//    cvs.getNDCx(x/2) << std::endl <<
//    cvs.getNDCy(y/2) << std::endl <<
//    cvs.getNDCy(y - 3) << std::endl;

    Camera c(m);
    c.rayTrace (1);
    c.print();



    return EXIT_SUCCESS;
}