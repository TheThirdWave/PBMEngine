#include <stdio.h>
#include "particlegrid.h"
#include "particle.h"

ParticleGrid::ParticleGrid()
{

}

void ParticleGrid::buildGrid(std::vector<Particle> &parts)
{
    //get initial values for the xMin and yMin
    float xMin = max.x;
    float yMin = max.y;
    float xMax = min.x;
    float yMax = min.y;

    //find the minimum and maximum particle positions within the simulation (we disregard any particles outside the minimum and maximum positions set by the user)
//#pragma omp parallel for
    for(int i = 0; i < parts.size(); i++)
    {
        glm::vec2 hPart = parts[i].getPosition();
        if(hPart.x <= max.x && hPart.y <= max.y && hPart.x >= min.x && hPart.y >= min.y)
        {
            xMin = std::min(xMin, hPart.x);
            yMin = std::min(yMin, hPart.y);
            xMax = std::max(xMax, hPart.x);
            yMax = std::max(yMax, hPart.y);
        }
    }

    //determine the lower left and upper right corners of the grid.
    LLC.x = xMin;
    LLC.y = yMin;
    URC.x = xMax;
    URC.y = yMax;
    glm::vec2 hold = URC - LLC;
    hold = hold / cellSize;
    //we move the upper right corner of the grid so the grid is evenly divided up into cellSize cells.
    URC.x = LLC.x + ceil(hold.x) * cellSize;
    URC.y = LLC.y + ceil(hold.y) * cellSize;
    //set the number of cells wide and high the grid is.
    width = (int)ceil(hold.x);
    height = (int)ceil(hold.y);
    //initialize the vector to hold the particles.
    grid.clear();
    grid.resize((width + 1) * (height + 1));

    //Here we assign each particle it's index in the grid and push the particle to the appropriate spot
    //in the grid vector.
//#pragma omp parallel for
    for(int i = 0; i < parts.size(); i++)
    {
        glm::vec2 hPart = parts[i].getPosition();
        if(hPart.x <= max.x && hPart.y <= max.y && hPart.x >= min.x && hPart.y >= min.y)
        {
            hPart -= LLC;
            hPart /= cellSize;
            parts[i].setGridPos((int)hPart.x, (int)hPart.y);
            int index = (int)hPart.x + (int)hPart.y * width;
            grid[index].push_back(&parts[i]);
        }
        //particles that are outside the minimum and maximum have their grid indicies set to -1 so we
        //know they're not included in the grid.
        else
        {
            parts[i].setGridPos(-1, -1);
        }
    }
}

std::vector<Particle*>* ParticleGrid::getPartsAt(int x, int y)
{
    if(x < 0 || y < 0 || x > width || y > height) return NULL;
    int index = x + y * width;
    return &grid[index];
}

void ParticleGrid::printGridStatus()
{
    fprintf(stderr, "=====================================\n");
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            int index = j + i * width;
            fprintf(stderr, "%d ", grid[index].size());
        }
        fprintf(stderr, "\n");
    }
}

void ParticleGrid::setMin(glm::vec2 m)
{
    min = m;
}

void ParticleGrid::setMax(glm::vec2 m)
{
    max = m;
}

void ParticleGrid::setCellSize(float cs)
{
    cellSize = cs;
}
