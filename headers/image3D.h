#ifndef IMAGE3D
#define IMAGE3D
    #include <string>
    #include "point3D.h"
    #include "camera3D.h"
    #include "ray3D.h"
    #include "object3D.h"
    #include "color3D.h"

    template <class T>
    class image3D{
        public:
            int height, width;
            T left, right, top, bottom;
            T distance;
            point3D<T> middle;
            point3D<T> start_pixel;
            point3D<T> cur_pixel;
            camera3D<T> static_camera;
            std::string image_name;

            rgb** image_plane;

            image3D(int height, int width, T left, T right, T top, T bottom, T distance, camera3D<T>& camera, rgb background, std::string name);
            //~image3D();
            ray3D<T> getRay(const int i, const int j);
            int compareRGB(int val);
    };

    template <class T>
    image3D<T>::image3D(int h, int w, T l, T r, T t, T b, T dist, camera3D<T>& cam, rgb bg, std::string _n){
        static_camera = cam;
        height = h;width = w;
        left=l;right=r;top=t;bottom=b;distance=dist;
        image_plane = new rgb*[width];
        for(int i=0;i<width;i++){
            image_plane[i] = new rgb[height];
        }
        middle = cam.position+cam.getGaze().dir*dist;
        start_pixel = middle+cam.u.dir*left+cam.v.dir*top;
        image_name = _n;
    }

    // template <class T>
    // image3D<T>::~image3D(){
    //     for(int i=0;i<height;i++){
    //         delete[] image_plane[i];
    //     }
    //     delete[] image_plane;
    // }

    template <class T>
    ray3D<T> image3D<T>::getRay(const int i, const int j){
        T s_u = (i+0.5)*(right-left)/width;
        T s_v = (j+0.5)*(top-bottom)/height;
        point3D<T> s = start_pixel+static_camera.u.dir*s_u-static_camera.v.dir*s_v;
        return ray3D<T> (static_camera.position,s-static_camera.position);
    }

    template <class T>
    int image3D<T>::compareRGB(int val){
        return val>255?255:val;
    }

#endif