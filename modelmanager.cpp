#include "modelmanager.h"

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
        models[modelIndex].indicies = new unsigned int[indexCount * 3];
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
                    models[modelIndex].indicies[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
                    strtok(NULL, " /");
                    models[modelIndex].indicies[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
                    strtok(NULL, " /");
                    models[modelIndex].indicies[idxIdx++] = (stoi(strtok(NULL, " /"), NULL) - 1);
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

void ModelManager::readObjLoader(char* filename)
{
    loader.LoadFile(filename);
}

model* ModelManager::getModel(int idx)
{
    return &models[idx];
}

