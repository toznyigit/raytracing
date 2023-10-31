#ifndef MATERIAL3D
#define MATERIAL3D
    #include "color3D.h"

    class material3D{
        public:
            frgb refAmbient, refDiffuse, refSpecular, refMirror;
            bool isMirror;
            int phongExp;
            material3D();
            material3D(const frgb& _A, const frgb& _D, const frgb& _S, const frgb& _M, int P);
    };
    
    material3D::material3D(){}
    material3D::material3D(const frgb& _A, const frgb& _D, const frgb& _S, const frgb& _M, int P){
        refAmbient = _A;
        refDiffuse = _D;
        refSpecular = _S;
        refMirror = _M;
        phongExp = P;
        if(_M.redf==0 && _M.greenf==0 && _M.bluef==0) isMirror = false;
        else isMirror = true;
    }
    
#endif