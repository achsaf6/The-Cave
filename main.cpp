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

    std::ifstream f("../Ico.obj");
    if (!f.is_open()){
        std::cerr << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }

    Model m(f);

//    std::cout << m.toOBJ("../Test");
    FILE* pipe = popen("ls", "r");

    char buffer[128];

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
      std::cout << buffer;
    }

    pclose(pipe);

    return EXIT_SUCCESS;
}