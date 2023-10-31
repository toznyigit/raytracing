#ifndef OBJECT3D
#define OBJECT3D

    #include <iostream>
    #include <cmath>
    #include <vector>
    #include "point3D.h"
    #include "ray3D.h"
    #include "color3D.h"
    #include "material3D.h"
    #include "light3D.h"

    enum objectType {SPHERE, TRIANGLE, MESH, IMAGE};
    struct F {int A; int B; int C;};
    int _newid = 0;

    template <class T>
    class object3D{
        public:
            int id;
            objectType type;

            object3D();
            object3D(int _id, objectType _type);
            void operator=(const object3D<T>& s);
            T intersect(const ray3D<T>& _ray);
            T diffusion(const light3D<T>& _light, const point3D<T>& _intersect);
            T specular(const light3D<T>& _light, const point3D<T>& _intersect, const point3D<T>& _camera, T spc);

    };

    template <class T>
    object3D<T>::object3D(){}

    template <class T>
    object3D<T>::object3D(int _id, objectType _type){
        id = _id;
        type = _type;
    }

    template <class T>
    class triangleObject: public object3D<T>{
        public:
            point3D<T> A,B,C;

            triangleObject();
            triangleObject(const point3D<T>& _A, const point3D<T>& _B, const point3D<T>& _Cr);
            T intersect(const ray3D<T>& _ray);
            T diffusion(const light3D<T>& _light, const point3D<T>& _intersect);
            T specular(const light3D<T>& _light, const point3D<T>& _intersect, const point3D<T>& _camera, T spc);
            ray3D<T> reflect(const ray3D<T>& _ray, const point3D<T>& _intersect, T sre);
            vector3D<T> normal();

    };

    template <class T>
    triangleObject<T>::triangleObject(){}

    template <class T>
    triangleObject<T>::triangleObject(const point3D<T>& _A, const point3D<T>& _B, const point3D<T>& _C): object3D<T>(++_newid, TRIANGLE){
        A=_A; B=_B; C=_C;
    }

    template <class T>
    T triangleObject<T>::intersect(const ray3D<T>& _ray){
        point3D<T> o(_ray.origin.x,_ray.origin.y,_ray.origin.z);
        point3D<T> d(_ray.direction.x,_ray.direction.y,_ray.direction.z);
        vector3D<T> AB(B-A);
        vector3D<T> AC(C-A);
        vector3D<T> normal = (AB^AC);
        vector3D<T> rtv(d-o);
        if(normal*rtv > 0){
            return -1.0;
        }

        T abx=A.x-B.x;T aby=A.y-B.y;T abz=A.z-B.z;
        T acx=A.x-C.x;T acy=A.y-C.y;T acz=A.z-C.z;
        T aox=A.x-o.x; T aoy=A.y-o.y; T aoz = A.z-o.z;
        T area = abx*(acy*d.z-acz*d.y)+aby*(acz*d.x-acx*d.z)+abz*(acx*d.y-acy*d.x);
        T beta = aox*(acy*d.z-acz*d.y)+aoy*(acz*d.x-acx*d.z)+aoz*(acx*d.y-acy*d.x);
        T gamma = abx*(aoy*d.z-aoz*d.y)+aby*(aoz*d.x-aox*d.z)+abz*(aox*d.y-aoy*d.x);
        T t = abx*(acy*aoz-acz*aoy)+aby*(acz*aox-acx*aoz)+abz*(acx*aoy-acy*aox);

        beta /= area;
        gamma /= area;
        t /= area;

        if(0<t && (beta+gamma)<=1 && 0<=beta && 0<=gamma){
            return t;
        }
        else{
            return -1.0;
        }
    }

    template <class T>
    T triangleObject<T>::diffusion(const light3D<T>& _light, const point3D<T>& _intersect){
        point3D<T> tmpi = _intersect;
        point3D<T> tmpl = _light.position;
        vector3D<T> AB(B-A);
        vector3D<T> AC(C-A);
        vector3D<T> norm = (AB^AC);
        vector3D<T> l(tmpl-tmpi);
        norm = norm.normal();
        l = l.normal();
        T cosT = norm*l;
        if(cosT >= 0) return cosT;
        else return 0;
        // point3D<T> tmpi = _intersect;
        // point3D<T> tmpl = _light.position;
        // vector3D<T> AB(B-A);
        // vector3D<T> AC(C-A);
        // vector3D<T> normal = (AB^AC);
        // vector3D<T> lup(tmpi-tmpl);
        // T cosT = (normal*lup)/(normal.len*lup.len);
        // if(cosT<0) return -cosT;
        // else return 0;
    }

    template <class T>
    T triangleObject<T>::specular(const light3D<T>& _light, const point3D<T>& _intersect, const point3D<T>& _camera, T spc){
        point3D<T> tmpi = _intersect;
        point3D<T> tmpl = _light.position;
        point3D<T> tmpc = _camera;
        vector3D<T> AB(B-A);
        vector3D<T> AC(C-A);
        vector3D<T> norm = (AB^AC);
        vector3D<T> v(tmpc-tmpi);
        vector3D<T> l(tmpl-tmpi);
        norm = norm.normal();
        v = v.normal();
        l = l.normal();
        vector3D<T> h(v+l);
        h = h.normal();
        T cosT = norm*h;
        if(cosT > 0) return pow(cosT,spc);
        else return 0;
        // point3D<T> tmpi = _intersect;
        // point3D<T> tmpl = _light.position;
        // point3D<T> tmpc = _camera;
        // vector3D<T> AB(B-A);
        // vector3D<T> AC(C-A);
        // vector3D<T> normal = (AB^AC);
        // vector3D<T> tmpn((tmpl+tmpc)/2-tmpi);
        // T cosT = -(normal*tmpn)/(normal.len*tmpn.len);
        // if(cosT < 0) return pow(-cosT,spc);
        // else return 0;
    }

    template <class T>
    vector3D<T> triangleObject<T>::normal(){
        vector3D<T> AB(B-A);
        vector3D<T> AC(C-A);
        return (AB^AC).normal();
    }

    template <class T>
    ray3D<T> triangleObject<T>::reflect(const ray3D<T>& _ray, const point3D<T>& _intersect, T sre){
        vector3D<T> AB(B-A);
        vector3D<T> AC(C-A);
        vector3D<T> normal = (AB^AC);
        point3D<T> tmpi = _intersect;
        normal = normal.normal();
        point3D<T> tmpo = _ray.origin;
        point3D<T> tmpd = _ray.direction;
        vector3D<T> I (tmpd);
        I = I.normal();
        T dot = 2*(I*normal);
        point3D<T> direction(I.dir-normal.dir*dot);
        return ray3D<T> (tmpi+normal.dir*sre,direction);
    }

    template <class T>
    class meshObject: public object3D<T>{
        public:
            std::vector<F> faces;
            material3D material;

            meshObject();
            meshObject(const std::vector<F> _faces, const material3D& _mat);

    };

    template <class T>
    meshObject<T>::meshObject(const std::vector<F> _faces, const material3D& _mat){
        material = _mat;
        faces = _faces;
    }


    template <class T>
    class sphereObject: public object3D<T>{
        public:
            point3D<T> center;
            T radius;
            material3D material;

            sphereObject();
            sphereObject(const point3D<T>& _c, const T& _r, const material3D& _mat);
            T intersect(const ray3D<T>& _ray);
            T diffusion(const light3D<T>& _light, const point3D<T>& _intersect);
            T specular(const light3D<T>& _light, const point3D<T>& _intersect, const point3D<T>& _camera, T spc);
            ray3D<T> reflect(const ray3D<T>& _ray, const point3D<T>& _intersect, T sre);
            vector3D<T> normal(const point3D<T>& _intersect);

    };

    template <class T>
    sphereObject<T>::sphereObject(const point3D<T>& _c, const T& _r, const material3D& _mat) : object3D<T>(++_newid, SPHERE){
        center = _c;
        radius = _r;
        material = _mat;
    }

    template <class T>
    T sphereObject<T>::intersect(const ray3D<T>& _ray){
        // point3D<T> d = _ray.direction;
        // point3D<T> o = _ray.origin;
        // point3D<T> c = center;
        // T delta = ((d*(o-c))*(d*(o-c)))-(d*d)*((o-c)*(o-c)-pow(radius,2));
        // if(delta<0.1) return -1.0;
        // else{
        //     delta = sqrt(abs(delta));
        //     T t1 = (c-o)*d-delta;
        //     T t2 = (c-o)*d+delta;
        //     if(t1<t2) return t1/(d*d);
        //     else return t2/(d*d);
        // }
        point3D<T> dir = _ray.direction;
        point3D<T> orj = _ray.origin;
        T a = dir*dir;
        T b = (dir*(orj-center))*2;
        T c = (orj-center)*(orj-center)-radius*radius;
        T delta = b*b-4*a*c+0.001;
        if(delta<0) return -1.0;
        else {
            T t1 = (sqrt(delta)-b)/(2*a);
            T t2 = (-sqrt(delta)-b)/(2*a);
            if(t1<t2) return t1;
            else return t2;
        }
    }

    template <class T>
    T sphereObject<T>::diffusion(const light3D<T>& _light, const point3D<T>& _intersect){
        point3D<T> tmpi = _intersect;
        point3D<T> tmpl = _light.position;
        vector3D<T> norm(tmpi-center);
        vector3D<T> l(tmpl-tmpi);
        norm = norm.normal();
        l = l.normal();
        T cosT = norm*l;
        if(cosT >= 0) return cosT;
        else return 0;
        // point3D<T> tmpi = _intersect;
        // point3D<T> tmpl = _light.position;
        // T dli = tmpl.d(tmpi);
        // T cosT = ((tmpl-tmpi)*(tmpi-center))/(dli*radius);
        // if(cosT > 0) return cosT;
        // else return 0;
    }

    template <class T>
    T sphereObject<T>::specular(const light3D<T>& _light, const point3D<T>& _intersect, const point3D<T>& _camera, T spc){
        point3D<T> tmpi = _intersect;
        point3D<T> tmpl = _light.position;
        point3D<T> tmpc = _camera;
        vector3D<T> norm(tmpi-center);
        vector3D<T> v(tmpc-tmpi);
        vector3D<T> l(tmpl-tmpi);
        norm = norm.normal();
        v = v.normal();
        l = l.normal();
        vector3D<T> h(v+l);
        h = h.normal();
        T cosT = norm*h;
        if(cosT > 0) return pow(cosT,spc);
        else return 0;
        // point3D<T> tmpi = _intersect;
        // point3D<T> tmpl = _light.position;
        // point3D<T> tmpc = _camera;
        // vector3D<T> norm (tmpi-center);
        // vector3D<T> wi (tmpl-tmpi);
        // vector3D<T> wo (tmpc-tmpi);
        // vector3D<T> h (wi+wo);
        // h = h/h.len;
        // norm = norm.normal();
        // T cosT = h*norm;
        // if(cosT>0) return pow(cosT,spc);
        // else return 0;

        // point3D<T> tmpi = _intersect;
        // point3D<T> tmpl = _light.position;
        // point3D<T> tmpc = _camera;
        // point3D<T> tmpn = (tmpl+tmpc)/2;
        // T dni = tmpn.d(tmpi);
        // T cosT = ((tmpn-tmpi)*(tmpi-center))/(dni*radius);
        // if(cosT > 0) return pow(cosT,spc);
        // else return 0;
    }

    template <class T>
    ray3D<T> sphereObject<T>::reflect(const ray3D<T>& _ray, const point3D<T>& _intersect, T sre){
        point3D<T> tmpi = _intersect;
        vector3D<T> norm (tmpi-center);
        norm = norm.normal();
        point3D<T> tmpo = _ray.origin;
        point3D<T> tmpd = _ray.direction;
        vector3D<T> I (tmpi-tmpo);
        // I = I.normal();
        T dot = 2*(I*norm);
        point3D<T> direction(I.dir-norm.dir*dot);
        return ray3D<T> (tmpi+norm.dir*sre,direction);
        // if(direction*tmpi<0) return ray3D<T> (tmpi+norm.dir*sre,direction);
        // else return ray3D<T> (tmpi+norm.dir*sre,direction);
        // point3D<T> tmpi = _intersect;
        // vector3D<T> normal (center-tmpi);
        // point3D<T> tmpo = _ray.origin;
        // point3D<T> tmpd = _ray.direction;
        // vector3D<T> I (tmpd);
        // T dot = 2*(I*normal)/I.len/normal.len;
        // point3D<T> direction(I.dir-normal.dir*dot);
        // return ray3D<T> (tmpi+(normal.dir*sre),direction);
    }

    template <class T>
    vector3D<T> sphereObject<T>::normal(const point3D<T>& _intersect){
        point3D<T> tmpi = _intersect;
        vector3D<T> norm(tmpi-center);
        return norm.normal();
    }

#endif