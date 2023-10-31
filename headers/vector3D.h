#ifndef VECTOR3D
#define VECTOR3D
    #include "point3D.h"
    #include <math.h>

    template <class T>
    class vector3D{
        public:
            point3D<T> dir;
            T len;
            
            vector3D();
            vector3D(const point3D<T>& dir);

            T operator*(const vector3D<T>& s); //dot product
            vector3D operator*(const T m); //dot product
            vector3D operator^(const vector3D<T>& s); //cross product
            vector3D operator+(const vector3D<T>& s);
            vector3D operator-(const vector3D<T>& s);
            vector3D operator/(const T& s);
            vector3D normal();
            void operator=(const vector3D<T>& s);

            friend std::ostream &operator<<(std::ostream &os, const vector3D<T> &obj){
                os << obj.dir;
                // os << obj.dir << " len: " << obj.len;
                return os;
            }

    };

    template <class T>
    vector3D<T>::vector3D(){
        dir = point3D<T>((T)0,(T)0,(T)0);
        len = 0;
    }

    template <class T>
    vector3D<T>::vector3D(const point3D<T>& s){
        dir = point3D<T>(s.x,s.y,s.z);
        len = sqrt(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z);
    }

    template <class T>
    T vector3D<T>::operator*(const vector3D<T>& s){
        return (dir.x*s.dir.x+dir.y*s.dir.y+dir.z*s.dir.z);
    }

    template <class T>
    vector3D<T> vector3D<T>::operator*(const T s){
        return vector3D<T> (point3D<T>(dir.x*s,dir.y*s,dir.z*s));
    }
    
    
    template <class T>
    vector3D<T> vector3D<T>::operator^(const vector3D<T>& s){
        point3D<T> new_dir(dir.y*s.dir.z-dir.z*s.dir.y, dir.z*s.dir.x-dir.x*s.dir.z, dir.x*s.dir.y-dir.y*s.dir.x);

        return vector3D<T> (new_dir);
    }

    template <class T>
    vector3D<T> vector3D<T>::operator+(const vector3D<T>& s){
        point3D<T> new_dir(dir.x+s.dir.x, dir.y+s.dir.y, dir.z+s.dir.z);
        return vector3D<T> (new_dir);
    }

    template <class T>
    vector3D<T> vector3D<T>::operator-(const vector3D<T>& s){
        point3D<T> new_dir(dir.x-s.dir.x, dir.y-s.dir.y, dir.z-s.dir.z);
        return vector3D<T> (new_dir);
    }

    template <class T>
    void vector3D<T>::operator=(const vector3D<T>& s){
        dir = s.dir;
        len = s.len;
    }

    template <class T>
    vector3D<T> vector3D<T>::operator/(const T& s){
        return vector3D<T> (point3D<T> (dir/s));
    }

    template <class T>
    vector3D<T> vector3D<T>::normal(){
        return vector3D<T>(dir/len);
    }

#endif