#ifndef LIGHT3D
#define LIGHT3D
    #include "point3D.h"
    #include "color3D.h"

    template <class T>
    class light3D{
        public:
            irgb intensity;
            point3D<T> position;

            light3D();
            light3D(const point3D<T>& _pos, const irgb _ins);
    };

    template <class T>
    light3D<T>::light3D(){
        intensity = irgb(0,0,0);
        position = point3D<T>();
    }

    template <class T>
    light3D<T>::light3D(const point3D<T>& _pos, const irgb _ins){
        intensity = _ins;
        position = _pos;
    }
#endif