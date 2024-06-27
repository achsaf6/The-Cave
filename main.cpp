#include <cstdio>
#include "Classes.cpp"
#include <iostream>

/*
TODO take into account:
    Model Size
    pixel resolution (is there a size of window option?)
    Are materials relevant?
    freeform curves/surfaces are a thing (find an example to use as well maybe the blender chimp?) NURBS command starts with vp
    more than 3 vertexes per face

TODO things to add:
    CameraObject
    - gets object to look at
    - lightsource
    - rotational functions



*/

int main(){

    std::ifstream f("../Cube.obj");
    if (!f.is_open()){
        std::cerr << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }


    Model m(f);

    m.printModel();


    return EXIT_SUCCESS;
}