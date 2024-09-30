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
    Eigen::Vector3d origin(4,4,4);
    Camera c(m, origin);

    while(true){
      std::cout << "\033[2J\033[H";
      c.rayTrace();
      c.print();
      m.rotate (M_PI/20);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << std::endl;






//TODO calculate normals
//TODO "brightness" of letters (is there a better font?)

    return EXIT_SUCCESS;
}