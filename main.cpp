#include <cstdio>
#include <iostream>
#include <fstream>
#include <thread>
#include "Classes/Model.h"
#include "Classes/Camera.h"

/*
TODO take into account:
    Sobel Filter for edge detection




*/

int main(){
  std::ios::sync_with_stdio(false);
    std::ifstream f("../Assets/Cube.obj");
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