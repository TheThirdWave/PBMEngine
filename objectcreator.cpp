#include "objectcreator.h"

ObjectCreator::ObjectCreator()
{
}

void ObjectCreator::setEdgeArray(EdgeObject ptr[], int len)
{
    edges = ptr;
    maxEdges = len;
    numEdges = 0;
}

void ObjectCreator::setPolyArray(PolygonObject  ptr[], int len)
{
    polys = ptr;
    maxPolys = len;
    numPolys = 0;
}

void ObjectCreator::setVertArray(ParticleObject ptr[], int len)
{
    verticies = ptr;
    maxVerts = len;
    numVerts = 0;
}

void ObjectCreator::setColArray(ObjCollection ptr[], int len)
{
    collections = ptr;
    maxCols = len;
    numCols = 0;
}

void ObjectCreator::setPhysManager(PhysicsManager * man)
{
    manager = man;
}

void ObjectCreator::setDefaultVModel(RenderObject * mod)
{
    vModel = mod;
}

void ObjectCreator::setDefaultEModel(RenderObject * mod)
{
    eModel = mod;
}

void ObjectCreator::setDefaultPModel(RenderObject * mod)
{
    pModel = mod;
}

void ObjectCreator::setSpringAttribs(float l, float d, float k)
{
    springL = l;
    springD = d;
    springK = k;
}

int ObjectCreator::createVertex(glm::vec3 pos)
{
    if(numVerts < maxVerts)
    {
        verticies[numVerts].addToManager((void*)manager);
        verticies[numVerts].setID(PARTICLE);
        verticies[numVerts].setPosition(pos);
        verticies[numVerts].setGeometry(glm::normalize((glm::vec3(0.0f) - verticies[numVerts].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
        verticies[numVerts].setRenderObject(vModel);
        verticies[numVerts].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        verticies[numVerts].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        verticies[numVerts].setTTL(-0);
        return numVerts++;
    }
    return -1;
}

int ObjectCreator::createEdge(glm::vec3 pt1, glm::vec3 pt2)
{
    int pt1Idx = createVertex(pt1);
    int pt2Idx = createVertex(pt2);
    if(pt1Idx >= 0 && pt2Idx >=0) return createEdge(pt1Idx, pt2Idx);
    else return -1;
}

int ObjectCreator::createEdge(int pt1, int pt2)
{
    if(numEdges < maxEdges)
    {
        verticies[pt1].addParent(&edges[numEdges]);
        verticies[pt2].addParent(&edges[numEdges]);
        edges[numEdges].addToManager((void*)manager);
        edges[numEdges].setID(EDGE);
        edges[numEdges].addChild(&verticies[pt1]);
        edges[numEdges].addChild(&verticies[pt2]);
        edges[numEdges].setSpring(springL, springD, springK);
        return numEdges++;
    }
    return -1;
}

int ObjectCreator::createSTriangle(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3)
{
    int pt1Idx = createVertex(pt1);
    int pt2Idx = createVertex(pt2);
    int pt3Idx = createVertex(pt3);
    if(pt1Idx >= 0 && pt2Idx >= 0 && pt3Idx >= 0)
    {
        return createSTriangle(pt1Idx, pt2Idx, pt3Idx);
    }
    return -1;
}

int ObjectCreator::createSTriangle(int pt1, int pt2, int pt3)
{
    if(numPolys < maxPolys && numCols < maxCols)
    {
        int e1 = createEdge(pt1, pt2);
        int e2 = createEdge(pt1, pt3);
        int e3 = createEdge(pt2, pt3);
        if(e1 >= 0 && e2 >= 0 && e3 >= 0)
        {
            edges[e1].setActive(false);
            edges[e2].setActive(false);
            edges[e3].setActive(false);
            verticies[pt1].addParent(&polys[numPolys]);
            verticies[pt2].addParent(&polys[numPolys]);
            verticies[pt3].addParent(&polys[numPolys]);
            verticies[pt1].setActive(false);
            verticies[pt2].setActive(false);
            verticies[pt3].setActive(false);
            polys[numPolys].addToManager((void*)manager);
            polys[numPolys].setID(POLYGON);
            polys[numPolys].addChild(&verticies[pt1]);
            polys[numPolys].addChild(&verticies[pt2]);
            polys[numPolys].addChild(&verticies[pt3]);
            polys[numPolys].setGeometry(glm::normalize(glm::normalize(glm::cross(verticies[pt1].getPosition() - verticies[pt2].getPosition(), verticies[pt1].getPosition() - verticies[pt3].getPosition()))), glm::vec3(0.0f, 0.0f, -1.0f));
            polys[numPolys].setRenderObject(pModel);
            polys[numPolys].setScale(glm::vec3(1.0f, 1.0f, 1.0f));
            polys[numPolys].setActive(false);
            numPolys++;
            collections[numCols].addToManager(manager);
            collections[numCols].setID(COLLECTION);
            collections[numCols].addChild(&verticies[pt1]);
            collections[numCols].addChild(&verticies[pt2]);
            collections[numCols].addChild(&verticies[pt3]);
            collections[numCols].addChild(&edges[e1]);
            collections[numCols].addChild(&edges[e2]);
            collections[numCols].addChild(&edges[e3]);
            collections[numCols].addChild(&polys[numPolys]);
            verticies[pt1].addParent(&collections[numCols]);
            verticies[pt2].addParent(&collections[numCols]);
            verticies[pt3].addParent(&collections[numCols]);
            edges[e1].addParent(&collections[numCols]);
            edges[e2].addParent(&collections[numCols]);
            edges[e3].addParent(&collections[numCols]);
            polys[numPolys].addParent(&collections[numCols]);
            return numCols++;
        }
    }
    return -1;
}

int ObjectCreator::createPoly(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3)
{
    int pt1idx = createVertex(pt1);
    int pt2idx = createVertex(pt2);
    int pt3idx = createVertex(pt3);
    if(pt1idx >= 0 && pt2idx >= 0 && pt3idx >= 0)
    {
        return createPoly(pt1idx, pt2idx, pt3idx);
    }
    return -1;
}

int ObjectCreator::createPoly(int pt1, int pt2, int pt3)
{
    if(numPolys < maxPolys)
    {
        polys[numPolys].addToManager((void*)manager);
        polys[numPolys].setID(POLYGON);
        polys[numPolys].setRenderObject(pModel);
        polys[numPolys].setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        polys[numPolys].setGeometry(glm::normalize(glm::normalize(glm::cross(verticies[pt1].getPosition() - verticies[pt2].getPosition(), verticies[pt1].getPosition() - verticies[pt3].getPosition()))), glm::vec3(0.0f, 0.0f, -1.0f));
        verticies[pt1].addParent(&polys[numPolys]);
        verticies[pt2].addParent(&polys[numPolys]);
        verticies[pt3].addParent(&polys[numPolys]);
        polys[numPolys].addChild(&verticies[pt1]);
        polys[numPolys].addChild(&verticies[pt2]);
        polys[numPolys].addChild(&verticies[pt3]);
        return numPolys++;
    }
    return -1;
}

int ObjectCreator::createCube(glm::vec3 pos, float scale)
{
    if(numEdges < maxEdges - 24 && numPolys < maxPolys - 11 && numCols < maxCols)
    {
        int p1 = createVertex(pos + (glm::vec3(-0.5f, 0.5f, -0.5f) * scale));
        int p2 = createVertex(pos + (glm::vec3(-0.5f, 0.5f, 0.5f) * scale));
        int p3 = createVertex(pos + (glm::vec3(0.5f, 0.5f, 0.5f) * scale));
        int p4 = createVertex(pos + (glm::vec3(0.5f, 0.5f, -0.5f) * scale));
        int p5 = createVertex(pos + (glm::vec3(-0.5f, -0.5f, -0.5f) * scale));
        int p6 = createVertex(pos + (glm::vec3(-0.5f, -0.5f, 0.5f) * scale));
        int p7 = createVertex(pos + (glm::vec3(0.5f, -0.5f, 0.5f) * scale));
        int p8 = createVertex(pos + (glm::vec3(0.5f, -0.5f, -0.5f) * scale));

        int e1 = createEdge(p1, p2);
        int e2 = createEdge(p2, p3);
        int e3 = createEdge(p3, p4);
        int e4 = createEdge(p4, p1);
        int e5 = createEdge(p1, p3);
        int e6 = createEdge(p2, p4);

        int e7 = createEdge(p5, p6);
        int e8 = createEdge(p6, p7);
        int e9 = createEdge(p7, p8);
        int e10 = createEdge(p8, p5);
        int e11 = createEdge(p5, p7);
        int e12 = createEdge(p6, p8);

        int e13 = createEdge(p1, p5);
        int e14 = createEdge(p1, p8);
        int e15 = createEdge(p5, p4);

        int e16 = createEdge(p4, p8);
        int e17 = createEdge(p4, p7);
        int e18 = createEdge(p8, p3);

        int e19 = createEdge(p3, p7);
        int e20 = createEdge(p3, p6);
        int e21 = createEdge(p7, p2);

        int e22 = createEdge(p2, p6);
        int e23 = createEdge(p2, p5);
        int e24 = createEdge(p6, p1);

        int pol1 = createPoly(p1, p2, p3);
        int pol2 = createPoly(p3, p4, p1);

        int pol3 = createPoly(p5, p6, p7);
        int pol4 = createPoly(p7, p8, p5);

        int pol5 = createPoly(p1, p5, p4);
        int pol6 = createPoly(p5, p8, p4);

        int pol7 = createPoly(p4, p8, p3);
        int pol8 = createPoly(p3, p7, p8);

        int pol9 = createPoly(p3, p7, p2);
        int pol10 = createPoly(p2, p6, p3);

        int pol11 = createPoly(p2, p6, p1);
        int pol12 = createPoly(p1, p5, p2);

        collections[numCols].addToManager(manager);
        collections[numCols].setID(COLLECTION);
        collections[numCols].addChild(&verticies[p1]);
        collections[numCols].addChild(&verticies[p2]);
        collections[numCols].addChild(&verticies[p3]);
        collections[numCols].addChild(&verticies[p4]);
        collections[numCols].addChild(&verticies[p5]);
        collections[numCols].addChild(&verticies[p6]);
        collections[numCols].addChild(&verticies[p7]);
        collections[numCols].addChild(&verticies[p8]);
        verticies[p1].addParent(&collections[numCols]);
        verticies[p2].addParent(&collections[numCols]);
        verticies[p3].addParent(&collections[numCols]);
        verticies[p4].addParent(&collections[numCols]);
        verticies[p5].addParent(&collections[numCols]);
        verticies[p6].addParent(&collections[numCols]);
        verticies[p7].addParent(&collections[numCols]);
        verticies[p8].addParent(&collections[numCols]);

        collections[numCols].addChild(&edges[e1]);
        collections[numCols].addChild(&edges[e2]);
        collections[numCols].addChild(&edges[e3]);
        collections[numCols].addChild(&edges[e4]);
        collections[numCols].addChild(&edges[e5]);
        collections[numCols].addChild(&edges[e6]);
        collections[numCols].addChild(&edges[e7]);
        collections[numCols].addChild(&edges[e8]);
        collections[numCols].addChild(&edges[e9]);
        collections[numCols].addChild(&edges[e10]);
        collections[numCols].addChild(&edges[e11]);
        collections[numCols].addChild(&edges[e12]);
        collections[numCols].addChild(&edges[e13]);
        collections[numCols].addChild(&edges[e14]);
        collections[numCols].addChild(&edges[e15]);
        collections[numCols].addChild(&edges[e16]);
        collections[numCols].addChild(&edges[e17]);
        collections[numCols].addChild(&edges[e18]);
        collections[numCols].addChild(&edges[e19]);
        collections[numCols].addChild(&edges[e20]);
        collections[numCols].addChild(&edges[e21]);
        collections[numCols].addChild(&edges[e22]);
        collections[numCols].addChild(&edges[e23]);
        collections[numCols].addChild(&edges[e24]);
        edges[e1].addParent(&collections[numCols]);
        edges[e2].addParent(&collections[numCols]);
        edges[e3].addParent(&collections[numCols]);
        edges[e4].addParent(&collections[numCols]);
        edges[e5].addParent(&collections[numCols]);
        edges[e6].addParent(&collections[numCols]);
        edges[e7].addParent(&collections[numCols]);
        edges[e8].addParent(&collections[numCols]);
        edges[e9].addParent(&collections[numCols]);
        edges[e10].addParent(&collections[numCols]);
        edges[e11].addParent(&collections[numCols]);
        edges[e12].addParent(&collections[numCols]);
        edges[e13].addParent(&collections[numCols]);
        edges[e14].addParent(&collections[numCols]);
        edges[e15].addParent(&collections[numCols]);
        edges[e16].addParent(&collections[numCols]);
        edges[e17].addParent(&collections[numCols]);
        edges[e18].addParent(&collections[numCols]);
        edges[e19].addParent(&collections[numCols]);
        edges[e20].addParent(&collections[numCols]);
        edges[e21].addParent(&collections[numCols]);
        edges[e22].addParent(&collections[numCols]);
        edges[e23].addParent(&collections[numCols]);
        edges[e24].addParent(&collections[numCols]);

        collections[numCols].addChild(&polys[pol1]);
        collections[numCols].addChild(&polys[pol2]);
        collections[numCols].addChild(&polys[pol3]);
        collections[numCols].addChild(&polys[pol4]);
        collections[numCols].addChild(&polys[pol5]);
        collections[numCols].addChild(&polys[pol6]);
        collections[numCols].addChild(&polys[pol7]);
        collections[numCols].addChild(&polys[pol8]);
        collections[numCols].addChild(&polys[pol9]);
        collections[numCols].addChild(&polys[pol10]);
        collections[numCols].addChild(&polys[pol11]);
        collections[numCols].addChild(&polys[pol12]);
        polys[pol1].addParent(&collections[numCols]);
        polys[pol2].addParent(&collections[numCols]);
        polys[pol3].addParent(&collections[numCols]);
        polys[pol4].addParent(&collections[numCols]);
        polys[pol5].addParent(&collections[numCols]);
        polys[pol6].addParent(&collections[numCols]);
        polys[pol7].addParent(&collections[numCols]);
        polys[pol8].addParent(&collections[numCols]);
        polys[pol9].addParent(&collections[numCols]);
        polys[pol10].addParent(&collections[numCols]);
        polys[pol11].addParent(&collections[numCols]);
        polys[pol12].addParent(&collections[numCols]);
        return numCols++;
    }
    return -1;
}

int ObjectCreator::createTriangle(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3)
{
    int pt1Idx = createVertex(pt1);
    int pt2Idx = createVertex(pt2);
    int pt3Idx = createVertex(pt3);
    if(pt1Idx >= 0 && pt2Idx >= 0 && pt3Idx >= 0)
    {
        return createTriangle(pt1Idx, pt2Idx, pt3Idx);
    }
    return -1;
}

int ObjectCreator::createTriangle(int pt1, int pt2, int pt3)
{
    if(numPolys < maxPolys && numCols < maxCols)
    {
        int e1 = createEdge(pt1, pt2);
        int e2 = createEdge(pt1, pt3);
        int e3 = createEdge(pt2, pt3);
        if(e1 >= 0 && e2 >= 0 && e3 >= 0)
        {
            verticies[pt1].addParent(&polys[numPolys]);
            verticies[pt2].addParent(&polys[numPolys]);
            verticies[pt3].addParent(&polys[numPolys]);
            polys[numPolys].addToManager((void*)manager);
            polys[numPolys].setID(POLYGON);
            polys[numPolys].addChild(&verticies[pt1]);
            polys[numPolys].addChild(&verticies[pt2]);
            polys[numPolys].addChild(&verticies[pt3]);
            polys[numPolys].setGeometry(glm::normalize(glm::normalize(glm::cross(verticies[pt1].getPosition() - verticies[pt2].getPosition(), verticies[pt1].getPosition() - verticies[pt3].getPosition()))), glm::vec3(0.0f, 0.0f, -1.0f));
            polys[numPolys].setRenderObject(pModel);
            polys[numPolys].setScale(glm::vec3(1.0f, 1.0f, 1.0f));
            numPolys++;
            collections[numCols].addToManager(manager);
            collections[numCols].setID(COLLECTION);
            collections[numCols].addChild(&verticies[pt1]);
            collections[numCols].addChild(&verticies[pt2]);
            collections[numCols].addChild(&verticies[pt3]);
            collections[numCols].addChild(&edges[e1]);
            collections[numCols].addChild(&edges[e2]);
            collections[numCols].addChild(&edges[e3]);
            collections[numCols].addChild(&polys[numPolys]);
            verticies[pt1].addParent(&collections[numCols]);
            verticies[pt2].addParent(&collections[numCols]);
            verticies[pt3].addParent(&collections[numCols]);
            edges[e1].addParent(&collections[numCols]);
            edges[e2].addParent(&collections[numCols]);
            edges[e3].addParent(&collections[numCols]);
            polys[numPolys].addParent(&collections[numCols]);
            return numCols++;
        }
    }
    return -1;
}

int ObjectCreator::getNumCollections()
{
    return numCols;
}

int ObjectCreator::getNumEdges()
{
    return numEdges;
}

int ObjectCreator::getNumPolys()
{
    return numPolys;
}

int ObjectCreator::getNumVerts()
{
    return numVerts;
}

void ObjectCreator::clearAllObjects()
{
    numEdges = 0;
    numPolys = 0;
    numVerts = 0;
    numCols = 0;
}
