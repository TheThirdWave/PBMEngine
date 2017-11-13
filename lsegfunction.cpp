#include "lsegfunction.h"

LSegFunction::LSegFunction()
{
    pointIdx = 0;
}

float LSegFunction::getRelative(glm::vec2 pt)
{
    int numCrossings = 0;
    if(pointIdx >= 3)
    {
        for(int i = 0; i < pointIdx; i++)
        {
            if(i < pointIdx - 1) numCrossings += pointLSeg2D(pt, *points[i], *points[i+1]);
            else numCrossings +=  pointLSeg2D(pt, *points[i], *points[0]);
        }
    }
    if(numCrossings % 2 == 0) return 1;
    else return -1;
}

void LSegFunction::addPoint(glm::vec2 pt, image* screen)
{
    points[pointIdx] = new glm::vec2(pt);
    points[pointIdx]->x = screen->width - points[pointIdx]->x;
    points[pointIdx]->x *= screen->unitbytes;
    points[pointIdx++]->y *= screen->unitbytes;
}

void LSegFunction::clearPoints()
{
    pointIdx = 0;
}

int LSegFunction::pointLSeg2D(glm::vec2 pt, glm::vec2 l0, glm::vec2 l1)
{
    if(l0.x < pt.x && l1.x < pt.x) return 0;
    if(l0.y < pt.y && l1.y < pt.y) return 0;
    if(l0.y > pt.y && l1.y > pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y >= pt.y && l1.y <= pt.y) return 1;
    if(l0.x > pt.x && l1.x > pt.x && l0.y <= pt.y && l1.y >= pt.y) return 1;
    if(l0.y == l1.y && pt.y == l0.y) return 1;
    glm::vec2 lineVec = l1 - l0;
    float f;
    //if(lineVec.y == 0) f = 0;
    f = (pt.y - l0.y)/lineVec.y;
    float xf = l0.x + lineVec.x * f;
    if(pt.x <= xf) return 1;
    else return 0;

}


