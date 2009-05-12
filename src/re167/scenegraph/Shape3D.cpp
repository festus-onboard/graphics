#include "Shape3D.h"
#include <cmath>
#include <algorithm>
#include "Camera.h"

using namespace RE167;

void Shape3D::draw(const Matrix4 &t, RenderContext *context, Camera * camera, bool cull) { 
    
    Matrix4 t_new = t * shape->getTransformation();
    
    // Check to see if the object's bounding sphere is completely outside
    // of the view frustum.  If it is, then we don't need to draw it.
    
    // See where the center of the bounding sphere is after transformations.
    Vector4 transformedCenter = t_new * shape->getSphereCenter();
    
    // It's possible that we've had some scaling within our transformations;
    // see how the unit x,y,z vectors are affected by transformation matrix
    static const Vector4 xVector(1,0,0,0);
    static const Vector4 yVector(0,1,0,0);
    static const Vector4 zVector(0,0,1,0);

    float xScale = (t_new * xVector).magnitude();
    float yScale = (t_new * yVector).magnitude();
    float zScale = (t_new * zVector).magnitude();
    float maxScale = std::max(xScale, std::max(yScale, zScale));        
    
    float transformedRadius = maxScale * shape->getSphereRadius();
    
    /*
    std::cout << "Original center: " << shape->getSphereCenter() 
        << " Transformed Center: " << transformedCenter 
        << " Original radius: " << shape->getSphereRadius() 
        << " Transformed radius: " << transformedRadius <<  std::endl;
    */    

    if (cull) {
        if (camera->getSphereClipStatus(transformedCenter, transformedRadius) == Camera::COMPLETELY_OUTSIDE) {
            return;
        }
        
       /* switch (camera->getSphereClipStatus(transformedCenter, transformedRadius)) {
            case Camera::COMPLETELY_OUTSIDE:
                std::cout << "Completely outside. Not drawing" << std::endl;
                return;
                break;
            case Camera::COMPLETELY_INSIDE:
                std::cout << "Completely inside." << std::endl;
                break;
            default:
                std::cout << "Intersecting." << std::endl;
                break;
                */
        
    }
    
        
    context->setModelViewMatrix(t_new);
    context->setMaterial(shape->getMaterial());
    //context->setMaterial(material);

    context->render(shape);
}