#ifndef SCENE3D
#define SCENE3D
    #include <iostream>
    #include <string>
    #include <vector>
    #include <sstream>
    #include <stdexcept>
    #include <float.h>
    #include "camera3D.h"
    #include "image3D.h"
    #include "light3D.h"
    #include "material3D.h"
    #include "object3D.h"
    #include "point3D.h"
    #include "ray3D.h"
    #include "vector3D.h"
    #include "tinyxml2.h"

    struct shadow{
        int i;
        int j;
        double len;
        double cosT;
    };

    int compareRGB(int val){
        return val>255?255:val;
    }

    template <class T>
    void writePPMfromObject(const char* filename, image3D<T> image, int nx, int ny){
    int i,j;
    FILE *fp;

    if ((fp = fopen(filename, "w")) == NULL) 
    {
        throw std::runtime_error("Error: The ppm file cannot be opened for writing.");
    }
    
    fp = fopen(filename,"w");
    
    fprintf(fp,"P3\n");
    fprintf(fp,"#%s\n",filename);
    fprintf(fp,"%d %d\n",nx,ny);
    fprintf(fp,"255\n");
    for (j=0;j<ny;j++){
        for (i=0;i<nx;i++){
            fprintf(fp,"%d %d %d\t",image.image_plane[i][j].red,image.image_plane[i][j].green,image.image_plane[i][j].blue);
        }
        fprintf(fp,"\n");
    }
    (void) fclose(fp);
}

    template <class T>
    class Scene3D{
        public:
            rgb background;
            rgb ambient;
            T shadow_ray_epsilon;
            int max_recursion_depth;
            std::vector<camera3D<T>> cameras;
            std::vector<image3D<T>> images;
            std::vector<light3D<T>> lights;
            std::vector<material3D> materials;
            std::vector<point3D<T>> vertex_data;
            std::vector<meshObject<T>> meshes;
            std::vector<meshObject<T>> triangles;
            std::vector<sphereObject<T>> spheres;

            Scene3D(const std::string &filepath);
            // void draw_helper(image3D<T>& image, object3D<T>* object);
            void draw(int i);
            void printAll();
            bool isShadow(point3D<T> intersect, point3D<T> light, point3D<T> epsilon);
            irgb draw_helper(point3D<T> camPos, ray3D<T> ray, T minParam, int i, int j, int rec);
            // void draw_helper2(image3D<T>& image, ray3D<T> ray, T minParam, int i, int j);
    };

    template <class T>
    Scene3D<T>::Scene3D(const std::string &filepath){
        tinyxml2::XMLDocument file;
        std::stringstream stream;
        std::string name;
        T x,y,z,u,v,w,t;


        auto res = file.LoadFile(filepath.c_str());
        if(res) throw std::runtime_error("Error: The xml file cannot be loaded.");

        auto root = file.FirstChild();
        if (!root) throw std::runtime_error("Error: Root is not found.");

        auto element = root->FirstChildElement("BackgroundColor"); //bg
        if(element) stream << element->GetText() << std::endl;
        else stream << "0 0 0" << std::endl;
        stream >> x >> y >> z;
        background = irgb(x,y,z);

        element = root->FirstChildElement("ShadowRayEpsilon"); //sre
        if(element) stream << element->GetText() << std::endl;
        else stream << "0.001" << std::endl;
        stream >> shadow_ray_epsilon;

        element = root->FirstChildElement("MaxRecursionDepth"); //mrd
        if(element) stream << element->GetText() << std::endl;
        else stream << "0" << std::endl;
        stream >> max_recursion_depth;

        element = root->FirstChildElement("Cameras"); //camera and image
        element = element->FirstChildElement("Camera");
        while(element){
            auto child = element->FirstChildElement("Position");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("Gaze");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("Up");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("NearPlane");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("NearDistance");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("ImageResolution");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("ImageName");
            stream << child->GetText() << std::endl;


            stream >> x >> y >> z;
            point3D<T> e(x,y,z);
            stream >> x >> y >> z;
            vector3D<T> gaze(point3D<T>(x,y,z));
            stream >> x >> y >> z;
            vector3D<T> up(point3D<T>(x,y,z));
            camera3D<T> camera(e,gaze,up);
            stream >> x >> y >> z >> w;
            stream >> t;
            stream >> u >> v;
            stream >> name;
            image3D<T> image(v,u,x,y,w,z,t,camera,background,name);

            cameras.push_back(camera);
            images.push_back(image);
            element = element->NextSiblingElement("Camera");
        }

        element = root->FirstChildElement("Lights"); //light
        auto child = element->FirstChildElement("AmbientLight");
        stream << child->GetText() << std::endl;
        stream >> x >> y >> z;
        ambient = irgb(x,y,z);
        element = element->FirstChildElement("PointLight");
        while(element){
            child = element->FirstChildElement("Position");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("Intensity");
            stream << child->GetText() << std::endl;

            stream >> x >> y >> z;
            stream >> u >> v >> w;

            light3D<T> light(point3D<T> (x,y,z), irgb(u,v,w));
            lights.push_back(light);
            element = element->NextSiblingElement("PointLight");
        }

        element = root->FirstChildElement("Materials"); //material
        element = element->FirstChildElement("Material");
        while (element){
            child = element->FirstChildElement("AmbientReflectance");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("DiffuseReflectance");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("SpecularReflectance");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("MirrorReflectance");
            stream << child->GetText() << std::endl;
            child = element->FirstChildElement("PhongExponent");
            stream << child->GetText() << std::endl;

            stream >> x >> y >> z;
            frgb refAmbient(x,y,z);
            stream >> x >> y >> z;
            frgb refDiffuse(x,y,z);
            stream >> x >> y >> z;
            frgb refSpecular(x,y,z);
            stream >> x >> y >> z;
            frgb refMirror(x,y,z);
            stream >> t;
            material3D material(refAmbient,refDiffuse,refSpecular,refMirror,t);
            materials.push_back(material);
            element = element->NextSiblingElement("Material");
        }
        
        element = root->FirstChildElement("VertexData"); //vertex
        stream << element->GetText() << std::endl;
        while (!(stream >> x).eof())
        {
            stream >> y >> z;
            point3D<T> vertex(x,y,z);
            vertex_data.push_back(vertex);
        }
        stream.clear();

        element = root->FirstChildElement("Objects"); //mesh
        element = element->FirstChildElement("Mesh");
        while (element){
            child = element->FirstChildElement("Material");
            stream << child->GetText() << std::endl;
            stream >> t;

            child = element->FirstChildElement("Faces");
            stream << child->GetText() << std::endl;
            std::vector<F> faces;
            while (!(stream >> x).eof()){
                stream >> y >> z;
                F face; face.A = x; face.B = y; face.C = z;
                faces.push_back(face);
            }
            stream.clear();

            meshObject<T> mesh(faces,materials[t-1]);
            meshes.push_back(mesh);
            faces.clear();
            element = element->NextSiblingElement("Mesh");
        }
        stream.clear();

        element = root->FirstChildElement("Objects"); //triangle
        element = element->FirstChildElement("Triangle");
        while (element){
            child = element->FirstChildElement("Material");
            stream << child->GetText() << std::endl;
            stream >> t;

            child = element->FirstChildElement("Indices");
            stream << child->GetText() << std::endl;
            std::vector<F> faces;
            stream >> x >> y >> z;
            F face; face.A = x; face.B = y; face.C = z;
            faces.push_back(face);
            stream.clear();

            meshObject<T> triangle(faces,materials[t-1]);
            triangles.push_back(triangle);
            faces.clear();
            element = element->NextSiblingElement("Triangle");
        }

        element = root->FirstChildElement("Objects"); //sphere
        element = element->FirstChildElement("Sphere");
        while (element){
            child = element->FirstChildElement("Material");
            stream << child->GetText() << std::endl;
            stream >> t;

            child = element->FirstChildElement("Center");
            stream << child->GetText() << std::endl;
            stream >> x;

            child = element->FirstChildElement("Radius");
            stream << child->GetText() << std::endl;
            stream >> y;

            sphereObject<T> sphere(vertex_data[x-1],y,materials[t-1]);
            spheres.push_back(sphere);
            element = element->NextSiblingElement("Sphere");
        }

    }
    
    // template <class T>
    // void Scene3D<T>::draw(int e){

    //     // std::vector<shadow> shadowArray;

    //     for(int j=0;j<images[e].height;j++){
    //         for(int i=0;i<images[e].width;i++){
    //             T minParam = DBL_MAX;
    //             ray3D<T> tmp = images[e].getRay(i,j);
    //             for(int m=0;m<meshes.size();m++){
    //                 material3D cMat = meshes[m].material;
    //                 for(int k=0;k<meshes[m].faces.size();k++){
    //                     F currPoints = meshes[m].faces[k];
    //                     triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
    //                     T param = currTriangle.intersect(tmp);
    //                     if(param>0 && param<minParam){
    //                         minParam = param;
    //                         int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //                         for(int l=0;l<lights.size();l++){
    //                             T diffusion = currTriangle.diffusion(lights[l],tmp.parametric(param));
    //                             T specular = currTriangle.specular(lights[l],tmp.parametric(param),images[e].static_camera.position,cMat.phongExp);
    //                             T dli = pow(tmp.parametric(param).d(lights[l].position),2);
    //                             T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                             R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                             G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                             B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                         }
    //                         images[e].image_plane[i][j].red = images[e].compareRGB(R);
    //                         images[e].image_plane[i][j].green = images[e].compareRGB(G);
    //                         images[e].image_plane[i][j].blue = images[e].compareRGB(B);  
    //                     }
    //                 }
    //             }
    //             for(int m=0;m<triangles.size();m++){
    //                 material3D cMat = triangles[m].material;
    //                 for(int k=0;k<triangles[m].faces.size();k++){
    //                     F currPoints = triangles[m].faces[k];
    //                     triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
    //                     T param = currTriangle.intersect(tmp);
    //                     if(param>0 && param<minParam){
    //                         minParam = param;
    //                         int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //                         for(int l=0;l<lights.size();l++){
    //                             T diffusion = currTriangle.diffusion(lights[l],tmp.parametric(param));
    //                             T specular = currTriangle.specular(lights[l],tmp.parametric(param),images[e].static_camera.position,cMat.phongExp);
    //                             T dli = pow(tmp.parametric(param).d(lights[l].position),2);
    //                             T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                             R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                             G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                             B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                         }
    //                         images[e].image_plane[i][j].red = images[e].compareRGB(R);
    //                         images[e].image_plane[i][j].green = images[e].compareRGB(G);
    //                         images[e].image_plane[i][j].blue = images[e].compareRGB(B);  
    //                     }
    //                 }
    //             }
    //             for(int m=0;m<spheres.size();m++){
    //                 T param = spheres[m].intersect(tmp);
    //                 material3D cMat = spheres[m].material;
    //                 if(param>0 && param<minParam){
    //                     minParam = param;
    //                     int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //                     // std::cout << "R: " << R << "G: " << G << "B: " << B << std::endl;
    //                     for(int l=0;l<lights.size();l++){
    //                         // std::cout << lights[l].position << std::endl;
    //                         T diffusion = spheres[m].diffusion(lights[l],tmp.parametric(param));
    //                         T specular = spheres[m].specular(lights[l],tmp.parametric(param),images[e].static_camera.position,cMat.phongExp);
    //                         T dli = pow(tmp.parametric(param).d(lights[l].position),2);
    //                         T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                         R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                         G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                         B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                     }
    //                     images[e].image_plane[i][j].red = images[e].compareRGB(R);
    //                     images[e].image_plane[i][j].green = images[e].compareRGB(G);
    //                     images[e].image_plane[i][j].blue = images[e].compareRGB(B);                        
    //                 }
    //             }
    //         }
    //     }
    // }
    
    template <class T>
    bool Scene3D<T>::isShadow(point3D<T> isc, point3D<T> poslight, point3D<T> epsilon){
        ray3D<T> shadowRay(isc+epsilon, poslight-isc);
        T mp = DBL_MAX;
        for(int i=0;i<meshes.size();i++){
            for(int j=0;j<meshes[i].faces.size();j++){
                F cp = meshes[i].faces[j];
                triangleObject<T> tp(vertex_data[cp.A-1],vertex_data[cp.B-1],vertex_data[cp.C-1]);
                T sp = tp.intersect(shadowRay);
                if(sp>0){
                    mp = sp;
                    if(isc.d(shadowRay.parametric(sp))<isc.d(poslight)) return true;
                }

            }
        }
        for(int i=0;i<triangles.size();i++){
            for(int j=0;j<triangles[i].faces.size();j++){
                F cp = triangles[i].faces[j];
                triangleObject<T> tp(vertex_data[cp.A-1],vertex_data[cp.B-1],vertex_data[cp.C-1]);
                T sp = tp.intersect(shadowRay);
                if(sp>0){
                    mp = sp;
                    if(isc.d(shadowRay.parametric(sp))<isc.d(poslight)) return true;
                }

            }
        }
        for(int i=0;i<spheres.size();i++){
            T sp = spheres[i].intersect(shadowRay);
            if(sp>0){
                mp = sp;
                if(isc.d(shadowRay.parametric(sp))<isc.d(poslight)) return true;
            }
        }
        return false;

    }

    template <class T>  
    irgb Scene3D<T>::draw_helper(point3D<T> camPos, ray3D<T> ray, T minParam, int i, int j, int rec){
        int pixR=background.red,pixG=background.green,pixB=background.blue;
        if(rec > max_recursion_depth) return irgb(pixR,pixG,pixB);
        // if(rec > recLimit) return irgb(compareRGB(pixR),compareRGB(pixG),compareRGB(pixB));
        bool reflectionTrue = true;
        if(rec>0) reflectionTrue = false;
        for(int m=0;m<meshes.size();m++){
            material3D cMat = meshes[m].material;
            for(int k=0;k<meshes[m].faces.size();k++){
                F currPoints = meshes[m].faces[k];
                triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
                T param = currTriangle.intersect(ray);
                if(param>0 && param<minParam){
                    reflectionTrue = true;
                    minParam = param;
                    point3D<T> intersect = ray.parametric(param);
                    int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
                    if(cMat.isMirror){
                        ray3D<T> reflectRay = currTriangle.reflect(ray,intersect,shadow_ray_epsilon);
                        irgb reflection = draw_helper(reflectRay.origin, reflectRay, DBL_MAX, i, j, rec+1);
                        R+=cMat.refMirror.redf*reflection.red;G+=cMat.refMirror.greenf*reflection.green;B+=cMat.refMirror.bluef*reflection.blue;
                    }
                    for(int l=0;l<lights.size();l++){
                        bool shadowExists = isShadow(intersect,lights[l].position,currTriangle.normal().dir*shadow_ray_epsilon);
                        if(!shadowExists){
                            T diffusion = currTriangle.diffusion(lights[l],intersect);
                            T specular = currTriangle.specular(lights[l],intersect,camPos,cMat.phongExp);
                            T dli = pow(intersect.d(lights[l].position),2);
                            T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
                            R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
                            G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
                            B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
                        }
                    }
                    pixR=R;pixG=G;pixB=B;
                }
            }
        }
        for(int m=0;m<triangles.size();m++){
            material3D cMat = triangles[m].material;
            for(int k=0;k<triangles[m].faces.size();k++){
                F currPoints = triangles[m].faces[k];
                triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
                T param = currTriangle.intersect(ray);
                if(param>0 && param<minParam){
                    reflectionTrue = true;
                    minParam = param;
                    point3D<T> intersect = ray.parametric(param);
                    int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
                    if(cMat.isMirror){
                        ray3D<T> reflectRay = currTriangle.reflect(ray,intersect,shadow_ray_epsilon);
                        irgb reflection = draw_helper(reflectRay.origin, reflectRay, DBL_MAX, i, j, rec+1);
                        R+=cMat.refMirror.redf*reflection.red;G+=cMat.refMirror.greenf*reflection.green;B+=cMat.refMirror.bluef*reflection.blue;
                    }
                    for(int l=0;l<lights.size();l++){
                        bool shadowExists = isShadow(intersect,lights[l].position,currTriangle.normal().dir*shadow_ray_epsilon);
                        if(!shadowExists){
                            T diffusion = currTriangle.diffusion(lights[l],intersect);
                            T specular = currTriangle.specular(lights[l],intersect,camPos,cMat.phongExp);
                            T dli = pow(intersect.d(lights[l].position),2);
                            T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
                            R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
                            G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
                            B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
                        }
                    }
                    pixR=R;pixG=G;pixB=B;
                }
            }
        }
        for(int m=0;m<spheres.size();m++){
            T param = spheres[m].intersect(ray);
            material3D cMat = spheres[m].material;
            if(param>0 && param<minParam){
                reflectionTrue = true;
                minParam = param;
                point3D<T> intersect = ray.parametric(param);
                int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
                if(cMat.isMirror){
                    ray3D<T> reflectRay = spheres[m].reflect(ray,intersect,shadow_ray_epsilon);
                    irgb reflection = draw_helper(reflectRay.origin, reflectRay, DBL_MAX, i, j, rec+1);
                    R+=cMat.refMirror.redf*reflection.red;G+=cMat.refMirror.greenf*reflection.green;B+=cMat.refMirror.bluef*reflection.blue;
                };                
                for(int l=0;l<lights.size();l++){
                    bool shadowExists = isShadow(intersect,lights[l].position,spheres[m].normal(intersect).dir*shadow_ray_epsilon);
                    if(!shadowExists){
                        T diffusion = spheres[m].diffusion(lights[l],intersect);
                        T specular = spheres[m].specular(lights[l],intersect,camPos,cMat.phongExp);
                        T dli = pow(intersect.d(lights[l].position),2);
                        T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
                        R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
                        G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
                        B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
                    }
                }
                pixR=R;pixG=G;pixB=B;                       
            }
        }
        if(!reflectionTrue) return irgb(0,0,0);
        return irgb(compareRGB(pixR),compareRGB(pixG),compareRGB(pixB));
    }

    // template <class T>  
    // void Scene3D<T>::draw_helper2(image3D<T>& image, ray3D<T> ray, T minParam, int i, int j){
    //     int pixR,pixG,pixB;
    //     for(int m=0;m<meshes.size();m++){
    //         material3D cMat = meshes[m].material;
    //         for(int k=0;k<meshes[m].faces.size();k++){
    //             F currPoints = meshes[m].faces[k];
    //             triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
    //             T param = currTriangle.intersect(ray);
    //             if(param>0 && param<minParam){
    //                 minParam = param;
    //                 int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //                 for(int l=0;l<lights.size();l++){
    //                     bool shadowExists = isShadow(ray.parametric(param),lights[l].position,m,k,meshes[m].type);
    //                     if(!shadowExists){
    //                         T diffusion = currTriangle.diffusion(lights[l],ray.parametric(param));
    //                         T specular = currTriangle.specular(lights[l],ray.parametric(param),image.static_camera.position,cMat.phongExp);
    //                         T dli = pow(ray.parametric(param).d(lights[l].position),2);
    //                         T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                         R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                         G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                         B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                     }
    //                 }
    //                 image.image_plane[i][j].red = compareRGB(R);
    //                 image.image_plane[i][j].green = compareRGB(G);
    //                 image.image_plane[i][j].blue = compareRGB(B); 
    //             }
    //         }
    //     }
    //     for(int m=0;m<triangles.size();m++){
    //         material3D cMat = triangles[m].material;
    //         for(int k=0;k<triangles[m].faces.size();k++){
    //             F currPoints = triangles[m].faces[k];
    //             triangleObject<T> currTriangle(vertex_data[currPoints.A-1],vertex_data[currPoints.B-1],vertex_data[currPoints.C-1]);
    //             T param = currTriangle.intersect(ray);
    //             if(param>0 && param<minParam){
    //                 minParam = param;
    //                 int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //                 for(int l=0;l<lights.size();l++){
    //                     bool shadowExists = isShadow(ray.parametric(param),lights[l].position,m,k,meshes[m].type);
    //                     if(!shadowExists){
    //                         T diffusion = currTriangle.diffusion(lights[l],ray.parametric(param));
    //                         T specular = currTriangle.specular(lights[l],ray.parametric(param),image.static_camera.position,cMat.phongExp);
    //                         T dli = pow(ray.parametric(param).d(lights[l].position),2);
    //                         T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                         R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                         G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                         B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                     }
    //                 }
    //                 image.image_plane[i][j].red = compareRGB(R);
    //                 image.image_plane[i][j].green = compareRGB(G);
    //                 image.image_plane[i][j].blue = compareRGB(B); 
    //             }
    //         }
    //     }
    //     for(int m=0;m<spheres.size();m++){
    //         T param = spheres[m].intersect(ray);
    //         material3D cMat = spheres[m].material;
    //         if(param>0 && param<minParam){
    //             minParam = param;
    //             int R=cMat.refAmbient.redf*ambient.red,G=cMat.refAmbient.greenf*ambient.green,B=cMat.refAmbient.bluef*ambient.blue;
    //             // std::cout << "R: " << R << "G: " << G << "B: " << B << std::endl;
    //             for(int l=0;l<lights.size();l++){
    //                 bool shadowExists = isShadow(ray.parametric(param),lights[l].position,m,0,meshes[m].type);
    //                 if(!shadowExists){
    //                     T diffusion = spheres[m].diffusion(lights[l],ray.parametric(param));
    //                     T specular = spheres[m].specular(lights[l],ray.parametric(param),image.static_camera.position,cMat.phongExp);
    //                     T dli = pow(ray.parametric(param).d(lights[l].position),2);
    //                     T pR = lights[l].intensity.red/dli; T pG = lights[l].intensity.green/dli; T pB = lights[l].intensity.blue/dli;
    //                     R+=pR*(cMat.refDiffuse.redf*diffusion+cMat.refSpecular.redf*specular);
    //                     G+=pG*(cMat.refDiffuse.greenf*diffusion+cMat.refSpecular.greenf*specular);
    //                     B+=pB*(cMat.refDiffuse.bluef*diffusion+cMat.refSpecular.bluef*specular);
    //                 }
    //             }
    //             image.image_plane[i][j].red = compareRGB(R);
    //             image.image_plane[i][j].green = compareRGB(G);
    //             image.image_plane[i][j].blue = compareRGB(B);                        
    //         }
    //     }
    // }


    template <class T>
    void Scene3D<T>::draw(int e){
        for(int j=0;j<images[e].height;j++){
            for(int i=0;i<images[e].width;i++){
                ray3D<T> tmp = images[e].getRay(i,j);
                //draw_helper2(images[e], tmp, DBL_MAX, i, j);
                irgb tmpC = draw_helper(tmp.origin, tmp, DBL_MAX, i, j, 0);
                images[e].image_plane[i][j].red = tmpC.red;
                images[e].image_plane[i][j].green = tmpC.green;
                images[e].image_plane[i][j].blue = tmpC.blue; 
            }
        }       
    }
    

    template <class T>
    void Scene3D<T>::printAll(){

        std::cout << "Background: " << background << std::endl;
        std::cout << "Ambient: " << ambient << std::endl;
        std::cout << "Shadow Ray Epsilon: " << shadow_ray_epsilon << std::endl;
        std::cout << "Max Recursion Depth: " << max_recursion_depth << std::endl;

        for(int i=0;i<cameras.size();i++){
            std::cout << "Camera Position: " << cameras[i].position << std::endl;
            std::cout << "Camera Gaze: " << cameras[i].getGaze() << std::endl;
            std::cout << "Camera Up: " << cameras[i].v << std::endl;
        }

        for(int i=0;i<images.size();i++){
            std::cout << images[i].height << "x" << images[i].width << std::endl;
            std::cout << "Left: " << images[i].left << " Right: " << images[i].right 
            << " Top: " << images[i].top << " Bottom: " << images[i].bottom 
            << " Distance: " << images[i].distance << std::endl;
        }

        for(int i=0;i<lights.size();i++){
            std::cout << "Light: " << i+1 << std::endl;
            std::cout << "  Light Intensity: " << lights[i].intensity << std::endl;
            std::cout << "  Light Position: " << lights[i].position << std::endl;
            std::cout << std::endl;
        }

        for(int i=0;i<materials.size();i++){
            std::cout << "Material: " << i+1 << std::endl;
            std::cout << "  Ambient Reference: " << materials[i].refAmbient << std::endl;
            std::cout << "  Diffuse Reference: " << materials[i].refDiffuse << std::endl;
            std::cout << "  Specular Reference: " << materials[i].refSpecular << std::endl;
            std::cout << "  Mirror Reference: " << materials[i].refMirror << std::endl;
            std::cout << "  Phong Exponent: " << materials[i].phongExp << std::endl;
            std::cout << std::endl;
        }

        for(int i=0;i<meshes.size();i++){
            std::cout << "Mesh: " << i+1 << std::endl;
            std::cout << "  Ambient Reference: " << meshes[i].material.refAmbient << std::endl;
            std::cout << "  Diffuse Reference: " << meshes[i].material.refDiffuse << std::endl;
            std::cout << "  Specular Reference: " << meshes[i].material.refSpecular << std::endl;
            std::cout << "  Mirror Reference: " << meshes[i].material.refMirror << std::endl;
            std::cout << "  Phong Exponent: " << meshes[i].material.phongExp << std::endl;
            std::cout << std::endl;
        }

        for(int i=0;i<triangles.size();i++){
            std::cout << "Triangle: " << i+1 << std::endl;
            std::cout << "  Ambient Reference: " << meshes[i].material.refAmbient << std::endl;
            std::cout << "  Diffuse Reference: " << meshes[i].material.refDiffuse << std::endl;
            std::cout << "  Specular Reference: " << meshes[i].material.refSpecular << std::endl;
            std::cout << "  Mirror Reference: " << meshes[i].material.refMirror << std::endl;
            std::cout << "  Phong Exponent: " << meshes[i].material.phongExp << std::endl;
            std::cout << std::endl;
        }

        for(int i=0;i<spheres.size();i++){
            std::cout << "Sphere: " << i+1 << std::endl;
            std::cout << "  Ambient Reference: " << spheres[i].material.refAmbient << std::endl;
            std::cout << "  Diffuse Reference: " << spheres[i].material.refDiffuse << std::endl;
            std::cout << "  Specular Reference: " << spheres[i].material.refSpecular << std::endl;
            std::cout << "  Mirror Reference: " << spheres[i].material.refMirror << std::endl;
            std::cout << "  Phong Exponent: " << spheres[i].material.phongExp << std::endl;
            std::cout << std::endl;
        }

        
    }
#endif
