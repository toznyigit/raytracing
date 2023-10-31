#ifndef POINT3D
#define POINT3D
    #include <iostream>
    #include <cmath>

    template <class T>
    class point3D{
        public:
            T x,y,z;

            point3D();
            point3D(const T x, const T y, const T z);

            point3D<T> operator*(const T m);
            T operator*(const point3D<T>& s);
            point3D<T> operator^(const point3D<T>& s); //cross product
            point3D<T> operator+(const point3D<T>& s);
            point3D<T> operator-(const point3D<T>& s);
            point3D<T> operator/(T s);
            bool operator==(const point3D<T>& s);
            void operator=(const point3D<T>& s);

            T d(const point3D<T>& s);

            friend std::ostream &operator<<(std::ostream &os, const point3D<T> &obj){
                os << "[" << obj.x << "," << obj.y << "," << obj.z << "]";
                return os;
            }

    };

    template <class T>
    point3D<T>::point3D(){
        x=y=z=(T)0;
    }

    template <class T>
    point3D<T>::point3D(const T cx, const T cy, const T cz){
        x=(T)cx; y=(T)cy; z=(T)cz;
    }

    template <class T>
    point3D<T> point3D<T>::operator*(const T m){
        return point3D<T> (x*m, y*m, z*m);
    }

    template <class T>
    T point3D<T>::operator*(const point3D<T>& s){
        return (x*s.x+y*s.y+z*s.z);
    }

    template <class T>
    point3D<T> point3D<T>::operator^(const point3D<T>& s){
        return point3D<T> (y*s.z-z*s.y, z*s.x-x*s.z, x*s.y-y*s.x);
    }

    template <class T>
    point3D<T> point3D<T>::operator+(const point3D<T>& s){
        return point3D<T> (x+s.x,y+s.y,z+s.z);
    }

    template <class T>
    point3D<T> point3D<T>::operator-(const point3D<T>& s){
        return point3D<T> (x-s.x,y-s.y,z-s.z);
    }

    template <class T>
    point3D<T> point3D<T>::operator/(T s){
        return point3D<T> (x/s,y/s,z/s);
    }

    template <class T>
    bool point3D<T>::operator==(const point3D<T>& s){
        if(x==s.x && y==s.y && z==s.z) return true;
        return false;
    }

    template <class T>
    void point3D<T>::operator=(const point3D<T>& s){
        x=s.x; y=s.y; z=s.z;
    }

    template <class T>
    T point3D<T>::d(const point3D<T>& s){
        return sqrt((x-s.x)*(x-s.x)+(y-s.y)*(y-s.y)+(z-s.z)*(z-s.z));
    }

#endif