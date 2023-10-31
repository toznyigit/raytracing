#ifndef CAMERA3D
#define CAMERA3D
    #include "point3D.h"
    #include "vector3D.h"

    template <class T>
    class camera3D{
        public:
            point3D<T> position;
            vector3D<T> u, v, w; // x, y, z

            camera3D();
            camera3D(point3D<T>& pos, vector3D<T>& gaze, vector3D<T>& up);
            vector3D<T> getGaze();
            
            void operator=(const camera3D<T>& s);

    };


    template <class T>
    camera3D<T>::camera3D(){
        position = point3D<T> ();
        w = vector3D<T> ();
        v = vector3D<T> ();
        u = v^w;
    }

    template <class T>
    camera3D<T>::camera3D(point3D<T>& pos, vector3D<T>& gaze, vector3D<T>& up){
        position = pos;
        w = vector3D<T> ()-gaze;
        v = up;
        u = v^w;
    }

    template <class T>
    vector3D<T> camera3D<T>::getGaze(){
        return vector3D<T> ()-w;
    }

    template <class T>
    void camera3D<T>::operator=(const camera3D<T>& s){
        position = s.position;
        u = s.u; v = s.v; w = s.w;
    }

#endif