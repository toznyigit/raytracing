#ifndef COLOR3D
#define COLOR3D
    class rgb{
            public:
                int red, green, blue;

                // rgb(){red=20;green=20;blue=20;}
                rgb(){red=0;green=0;blue=0;}
                rgb(int _r, int _g, int _b){red=_r;green=_g;blue=_b;}
                friend std::ostream &operator<<(std::ostream &os, const rgb& _clr){
                    os << "[" << _clr.red << "," << _clr.green << "," << _clr.blue << "]";
                    return os;
                }
        };

        class irgb: public rgb{
            public:
                irgb() : rgb(){};
                irgb(int _r, int _g, int _b) : rgb(_r,_g,_b){};
                irgb(const irgb& _obj) : rgb(_obj.red,_obj.green,_obj.blue){};
                void operator=(const irgb& s){red = s.red; green = s.green; blue = s.blue;}
        };

        class frgb: public rgb{
            public:
                double redf, greenf, bluef;
                frgb() : rgb(){};
                frgb(double _r, double _g, double _b): rgb((int)255*_r,(int)255*_g,(int)255*_b){redf=_r;greenf=_g,bluef=_b;};
                friend std::ostream &operator<<(std::ostream &os, const frgb& _clr){
                    os << "[" << _clr.redf << "," << _clr.greenf << "," << _clr.bluef << "]";
                    return os;
                }
        };
#endif