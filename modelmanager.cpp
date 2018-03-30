#define TINYOBJLOADER_IMPLEMENTATION
#include "modelmanager.h"
#include "../tinyobjloader-master/tiny_obj_loader.h"

using namespace std;

ModelManager::ModelManager()
{
    modelIndex = 0;
}

int ModelManager::readObj(char* filename)
{
    ifstream reader;
    reader.open(filename);
    if(reader.is_open())
    {
        char buf[100];
        int vertCount = 0;
        int indexCount = 0;
        int texCount = 0;
        while(!reader.eof())
        {
            reader.getline(buf, 99);
            switch(buf[0])
            {
                case 'v':
                    switch(buf[1])
                    {
                    case ' ':
                        vertCount++;
                        break;
                    case 't':
                        texCount++;
                        break;
                    }
                    break;
                case 'f':
                    indexCount++;
                    break;
                case '#':
                default:
                    break;
            }
        }
        models[modelIndex].vertices = new float[vertCount * 3];
        models[modelIndex].vertLen = vertCount * 3;
        models[modelIndex].vertIdx = new unsigned int[indexCount * 3];
        models[modelIndex].idxLen = indexCount * 3;
        models[modelIndex].colors = new float[vertCount * 3];
        models[modelIndex].colorLen = vertCount * 3;
        models[modelIndex].texture = new float[texCount * 3];
        models[modelIndex].texLen = texCount * 3;


        reader.clear();
        reader.seekg(0, ios::beg);
        int vertIdx = 0;
        int idxIdx = 0;
        int texIdx = 0;
        while(!reader.eof())
        {
            reader.getline(buf, 99);
            switch(buf[0])
            {
                case 'v':
                    switch(buf[1])
                    {
                    case ' ':
                        strtok(buf, " ");
                        models[modelIndex].colors[vertIdx] =  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        models[modelIndex].vertices[vertIdx++] = stof(strtok(NULL, " "), NULL);
                        models[modelIndex].colors[vertIdx] =  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        models[modelIndex].vertices[vertIdx++] = stof(strtok(NULL, " "), NULL);
                        models[modelIndex].colors[vertIdx] =  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                        models[modelIndex].vertices[vertIdx++] = stof(strtok(NULL, " "), NULL);
                        break;
                    case 't':
                        strtok(buf, " ");
                        models[modelIndex].texture[texIdx++] = stof(strtok(NULL, " "), NULL);
                        models[modelIndex].texture[texIdx++] = stof(strtok(NULL, " "), NULL);
                        models[modelIndex].texture[texIdx++] = stof(strtok(NULL, " "), NULL);

                        break;
                    default:
                        break;
                    }
                    break;
                case 'f':
                    strtok(buf, " ");
                    models[modelIndex].vertIdx[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
                    strtok(NULL, " /");
                    models[modelIndex].vertIdx[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
                    strtok(NULL, " /");
                    models[modelIndex].vertIdx[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
                    strtok(NULL, " /");
                    break;
                case '#':
                default:
                    break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: Couldn't open .obj file.\n");
        return -1;
    }

    reader.close();
    return modelIndex++;
}

int ModelManager::readObjLoader(char* filename)
{
    tinyobj::attrib_t modelHold;
    std::vector<tinyobj::shape_t> shapeHold;
    std::vector<tinyobj::material_t> materialHold;
    std::string err;

    bool ret = tinyobj::LoadObj(&modelHold, &shapeHold, &materialHold, &err, filename, NULL, true);
    if(!err.empty())
    {
        fprintf(stderr, err.c_str());
    }
    if(!ret) return -1;
    models[modelIndex].vertLen = modelHold.vertices.size();
    models[modelIndex].colorLen = modelHold.colors.size();
    models[modelIndex].normLen = modelHold.normals.size();
    models[modelIndex].texLen = modelHold.texcoords.size();
    models[modelIndex].vertices = new float[models[modelIndex].vertLen];
    models[modelIndex].colors = new float[models[modelIndex].colorLen];
    models[modelIndex].normals = new float[models[modelIndex].normLen];
    models[modelIndex].texture = new float[models[modelIndex].texLen];
    models[modelIndex].idxLen = shapeHold[0].mesh.indices.size();
    models[modelIndex].vertIdx = new unsigned int[models[modelIndex].idxLen];
    models[modelIndex].cIdxLen = shapeHold[0].mesh.indices.size();
    models[modelIndex].colIdx = new unsigned int[models[modelIndex].idxLen];
    models[modelIndex].nIdxLen = shapeHold[0].mesh.indices.size();
    models[modelIndex].normIdx = new unsigned int[models[modelIndex].idxLen];
    models[modelIndex].tIdxLen = shapeHold[0].mesh.indices.size();
    models[modelIndex].texIdx = new unsigned int[models[modelIndex].idxLen];
    for(int i = 0; i < modelHold.vertices.size(); i++)
    {
        models[modelIndex].vertices[i] = modelHold.vertices[i];
    }
    for(int i = 0; i < modelHold.colors.size(); i++)
    {
        models[modelIndex].colors[i] = modelHold.colors[i];
    }
    for(int i = 0; i < modelHold.normals.size(); i++)
    {
        models[modelIndex].normals[i] = modelHold.normals[i];
    }
    for(int i = 0; i < modelHold.texcoords.size(); i++)
    {
        models[modelIndex].texture[i] = modelHold.texcoords[i];
    }
    for(int i = 0; i < shapeHold[0].mesh.indices.size(); i++)
    {
        models[modelIndex].vertIdx[i] = shapeHold[0].mesh.indices[i].vertex_index;
        models[modelIndex].normIdx[i] = shapeHold[0].mesh.indices[i].normal_index;
        models[modelIndex].texIdx[i] = shapeHold[0].mesh.indices[i].texcoord_index;
        if(models[modelIndex].cIdxLen == models[modelIndex].idxLen) models[modelIndex].colIdx[i] = shapeHold[0].mesh.indices[i].vertex_index;
    }

    return modelIndex++;
}

model* ModelManager::getModel(int idx)
{
    return &models[idx];
}

