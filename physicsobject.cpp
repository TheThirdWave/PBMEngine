#include "physicsobject.h"

PhysicsObject::PhysicsObject()
{
    rotation = glm::vec3(0.0f);
    position = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    newPosition = position;
    newVelocity = velocity;
    mass = 1;
    ttl = -1;
    rendrPtr = NULL;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos, glm::vec3 vel, glm::vec3 accel)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    position = pos;
    scale = glm::vec3(1.0f);
    velocity = vel;
    acceleration = accel;
    mass = 1;
    ttl = -1;
    newPosition = position;
    newVelocity = velocity;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos, glm::vec3 vel)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    position = pos;
    scale = glm::vec3(1.0f);
    velocity = vel;
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    position = pos;
    scale = glm::vec3(1.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, float x, float y, float z)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    position = glm::vec3(x, y, z);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(glm::vec3 pos, glm::vec3 vel, glm::vec3 accel)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    position = pos;
    velocity = vel;
    scale = glm::vec3(1.0f);
    acceleration = accel;
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(glm::vec3 pos, glm::vec3 vel)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    position = pos;
    scale = glm::vec3(1.0f);
    velocity = vel;
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(glm::vec3 pos)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    position = pos;
    scale = glm::vec3(1.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

PhysicsObject::PhysicsObject(float x, float y, float z)
{
    rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    position = glm::vec3(x, y, z);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newPosition = position;
    newVelocity = velocity;
    ttl = -1;
}

void PhysicsObject::updateRenderObject()
{
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * sca);
}

void PhysicsObject::setRenderObject(RenderObject* ptr)
{
    rendrPtr = ptr;
}

void PhysicsObject::setRotation(glm::vec3 rot)
{
    rotation = rot;
}

void PhysicsObject::setPosition(glm::vec3 pos)
{
    position = pos;
}

void PhysicsObject::setScale(glm::vec3 sca)
{
    scale = sca;
}

void PhysicsObject::setNewPosition(glm::vec3 pos)
{
    newPosition = pos;
}

void PhysicsObject::setVelocity(glm::vec3 vel)
{
    velocity = vel;
}

void PhysicsObject::setNewVelocity(glm::vec3 vel)
{
    newVelocity = vel;
}

void PhysicsObject::setAcceleration(glm::vec3 accel)
{
    acceleration = accel / mass;
}

void PhysicsObject::setMass(float ma)
{
    mass = ma;
}

void PhysicsObject::setTTL(double t)
{
    ttl = t;
}

void PhysicsObject::setGeometry(geometry geo)
{
    geoDescription = geo;
}

void PhysicsObject::addRotation(glm::vec3 nr)
{
    rotation = rotation + nr;
}

void PhysicsObject::addPosition(glm::vec3 np)
{
    position = position + np;
}

void PhysicsObject::addVelocity(glm::vec3 nv)
{
    velocity = velocity + nv;
}

void PhysicsObject::addScale(glm::vec3 sc)
{
    scale = scale + sc;
}

void PhysicsObject::addAcceleration(glm::vec3 na)
{
    acceleration = acceleration + na;
}

void PhysicsObject::getNextState(float ts)
{
    newVelocity = velocity + (acceleration * ts);
    newPosition = position + (newVelocity * ts);
}

void PhysicsObject::updateState()
{
    velocity = newVelocity;
    position = newPosition;
}

float PhysicsObject::getVelMag()
{
    return glm::length(velocity);
}

float PhysicsObject::getAccelMag()
{
    return glm::length(acceleration);
}


glm::vec3 PhysicsObject::getPos(){
    return position;
}

RenderObject* PhysicsObject::getRenderObj()
{
    return rendrPtr;
}

void PhysicsObject::initRenderObj()
{
}