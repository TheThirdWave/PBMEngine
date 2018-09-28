#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include <random>
#include <../glm-0.9.9.1/glm/glm.hpp>
#include <../glm-0.9.9.1/glm/gtc/matrix_transform.hpp>
#include "structpile.h"

#include "camera.h"
#include "buffer2d.h"
#include "field.h"
#include "scalarfields.h"

class volumerenderer
{
public:
    volumerenderer();

    void initColBuf(int w, int h);

    void passToDisplay();

    void setCamera(camera* c);
    void setDisplayBuf(Buffer2D* d);
    void setScalarFields(const Field<float>* fields, int len);
    void setColorFields(const Field<color>* fields, int len);
    void setLights(light* l, int len);
    void setBoundingBox(bbox* b);
    void setTCoeff(float t);
    void setMarchSize(float s);

    void renderFrame();
    color castRayMarch(glm::vec3 Xc, glm::vec3 Np, float Snear, float Sfar, float rand);
    color calculateLights(glm::vec3 Xc);
    float calcDSM(glm::vec3 Xc, glm::vec3 Nl, float Sl);
    bool checkBoundingBox(glm::vec3& Xc, glm::vec3& Np, bbox* b, float Snear, float Sfar, isect& hitPoints);

private:
    camera* cam;
    Buffer2D* disp;
    color* colBuf;
    light* lights;
    const Field<float>* scalarFields;
    const Field<color>* colorFields;
    bbox* boundingBox;

    int numSFields;
    int numCFields;
    int numLights;
    float marchSize;
    float Kt;
};

#endif // VOLUMERENDERER_H
