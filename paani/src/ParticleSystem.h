//
//  ParticleSystem.h
//  paani
//
//  Created by Sanchit Garg on 2/28/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__ParticleSystem__
#define __paani__ParticleSystem__

#include <stdio.h>
#include "Particle.h"
#include <vector>
#include <utility>
#include "utilities.h"


//This class stores the list of particles and handles their interactions

class ParticleSystem
{

private:
    std::vector <Particle> particles;   //List of all particles in the system

    const float poly6Const = 315.0 / (64 * PI);
    const float spikyConst = 45.0 / (PI);
    const float restDensity = 1000; //1000kg/m3
    const float smoothingRadius = 1.0f;
    const int solverIterations = 5;
    const float relaxation = 0.0001f;
    
public:
    
    typedef std::pair<int, glm::vec3> Neighbor;
    
    //Getter functions
    std::vector<Particle> getAllParticles();   //Returns the list of all particles in the system
    float getRestDensity();
    unsigned int getParticleCount();
    void update();

    //Other functions
    void addParticle(Particle);               //Add a particle to the system
    Particle getParticle(int index);            //Returns a particlular add some index
    
    //Function to return a list of all the neighbors within the specified distance
    // Stored as a pair of index and vector to the neighboring particle
    std::vector<Neighbor> findNeighbors(int index, float radius);
    
    //function that returns the density of a particle
    float getDensity(int index);
    
    float wPoly6Kernel(glm::vec3 distance, float smoothingRadius);
    glm::vec3 gradientWSpikyKernel(glm::vec3 distance, float smoothingRadius);

    glm::vec3 gradientConstraintAtParticle(int index);
    glm::vec3 gradientConstraintForNeighbor(int index, int neighborIndex);
        
    //apply external forces (gravity)
    void applyForces();
};

#endif
