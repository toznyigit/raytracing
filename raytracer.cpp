#include <iostream>
#include "headers/scene3D.h"
//#include <chrono>

int main(int argc, char* argv[])
{
    auto begin = std::chrono::high_resolution_clock::now(); 
    // Sample usage for reading an XML scene file
    Scene3D<double> scene(argv[1]);

    // for(auto i: scene.vertex_data){
    //     std::cout << i.x << "," << i.y << "," << i.z << std::endl;
    // }

    for(int i=0;i<scene.images.size();i++){
        //scene.printAll();
        scene.draw(i);
        char *filename = strcpy(new char[scene.images[i].image_name.length() + 1], scene.images[i].image_name.c_str());
        writePPMfromObject(filename,scene.images[i],scene.images[i].width,scene.images[i].height);
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    // printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.

    // const RGB BAR_COLOR[8] =
    // {
    //     { 255, 255, 255 },  // 100% White
    //     { 255, 255,   0 },  // Yellow
    //     {   0, 255, 255 },  // Cyan
    //     {   0, 255,   0 },  // Green
    //     { 255,   0, 255 },  // Magenta
    //     { 255,   0,   0 },  // Red
    //     {   0,   0, 255 },  // Blue
    //     {   0,   0,   0 },  // Black
    // };

    // int width = 640, height = 480;
    // int columnWidth = width / 8;

    // unsigned char* image = new unsigned char [width * height * 3];

    // int i = 0;
    // for (int y = 0; y < height; ++y)
    // {
    //     for (int x = 0; x < width; ++x)
    //     {
    //         int colIdx = x / columnWidth;
    //         image[i++] = BAR_COLOR[colIdx][0];
    //         image[i++] = BAR_COLOR[colIdx][1];
    //         image[i++] = BAR_COLOR[colIdx][2];
    //     }
    // }

    // write_ppm("test.ppm", image, width, height);

}
