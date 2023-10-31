#ifndef RAY3D
#define RAY3D
    #include "point3D.h"

    template <class T>
    class ray3D{
        public:
            point3D<T> origin;
            point3D<T> direction;
            
            ray3D();
            ray3D(const point3D<T>& direction);
            ray3D(const point3D<T>& origin, const point3D<T>& direction);
            ray3D<T> operator-();
            void operator=(const ray3D<T>& s);
            point3D<T> parametric(T param);

    };

    template <class T>
    ray3D<T>::ray3D(){
        origin = point3D<T>((T)0,(T)0,(T)0);
        direction = point3D<T>((T)0,(T)0,(T)0);
    }

    template <class T>
    ray3D<T>::ray3D(const point3D<T>& obj_direction){
        origin = point3D<T>((T)0,(T)0,(T)0);
        direction = obj_direction;
    }

    template <class T>
    ray3D<T>::ray3D(const point3D<T>& obj_origin, const point3D<T>& obj_direction){
        origin = obj_origin;
        direction = obj_direction;
    }

    template <class T>
    ray3D<T> ray3D<T>::operator-(){
        return ray3D<T>(origin,point3D<T> ()-direction);
    }

    template <class T>
    void ray3D<T>::operator=(const ray3D<T>& s){
        origin = s.origin;
        direction = s.direction;
    }

    template <class T>
    point3D<T> ray3D<T>::parametric(const T t){
        return point3D<T>(origin.x+t*direction.x,origin.y+t*direction.y,origin.z+t*direction.z);
    }

#endif