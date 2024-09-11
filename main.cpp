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
    std::ifstream f("../Face.obj");
    if (!f.is_open()){
        std::cerr << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }

    Model m(f);

//    std::cout << m.toOBJ();





    Camera c(m);

    c.rayTrace();
    c.print();



//TODO intersect triangle
//TODO calculate normals
//TODO "brightness" of letters (is there a better font?)


    return EXIT_SUCCESS;
}