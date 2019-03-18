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
#include "grid.h"
#include "volumelight.h"

class volumerenderer
{
public:
    volumerenderer();

    void initColBuf(int w, int h);

    void passToDisplay();

    void setCamera(camera* c);
    void setDisplayBuf(Buffer2D* d);
    void setEnvironmentBuf(Buffer2D* e, glm::vec3 n1, glm::vec3 n2, float inten);
    void setUseEnvironmentBuf(bool y);
    void setScalarFields(const Field<float>* fields, int len);
    void setColorFields(const Field<color>* fields, int len);
    void setLensField(const Field<float>* field);
    void setLights(VolumeLight* l, int len);
    void setBoundingBox(bbox* b);
    void setTCoeff(float t);
    void setAA(int aa);
    void setMarchSize(float s);

    void renderFrame();
    color castRayMarch(glm::vec3 Xc, glm::vec3 Np, float Snear, float Sfar, float rand);
    color calculateLights(glm::vec3 Xc);
    color sampleEnvMap(glm::vec3 dir);
    float lightMarch(glm::vec3 Xc, glm::vec3 Nl, float Sl);
    bool checkBoundingBox(glm::vec3& Xc, glm::vec3& Np, bbox* b, float Snear, float Sfar, isect& hitPoints);
    glm::vec3 getNextStepDir(glm::vec3 Xc, glm::vec3 Np, float arcLen);

    void calcDSM(Grid<float> &g, glm::vec3 pos);
    float tan2(float x);

private:
    camera* cam;
    Buffer2D* env;
    Buffer2D* disp;
    color* colBuf;
    VolumeLight* lights;
    const Field<float>* scalarFields;
    const Field<color>* colorFields;
    const Field<float>* lensField;
    bbox* boundingBox;

    bool useEnvMap;
    int numSFields;
    int numCFields;
    int numLights;
    int AA;
    float marchSize;
    float Kt;
    float envIntensity;
    glm::vec3 envUp;
    glm::vec3 envPole;
};

#endif // VOLUMERENDERER_H
