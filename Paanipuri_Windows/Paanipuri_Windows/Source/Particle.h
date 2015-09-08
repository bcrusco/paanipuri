//
//  Particle.h
//  Paanipuri
//
//  Bradley Crusco, Sanchit Garg, Debanshu Singh
//  Copyright (c) 2015 Crusco, Gar, Singh. All rights reserved.
//

#ifndef paani_Particle_h
#define paani_Particle_h

#include "glm/glm/glm.hpp"
#include <vector>

// This class stores the properties of individual particles

class Particle {
private:
	float mass = 1.0f;    // Mass of all particles will be same
	float radius = 0.5f;  // Radius of all particles will be same
	float density = 0.0f;
	int phase;
	float predictedMass;

	// Current values
	glm::vec3 velocity;     // Stores the current velocity of the particle
	glm::vec3 forces;       // Stores the accumulated forces acting on the particle

	// Predicted values

	glm::ivec3 hashPosition; // Stores the grid index where the particle predicted position is
	std::vector <int> neighborIndices; // Stores the indices of all the neighbors
	float lambda;

	// Particle offset position vector from center of mass at rest configuration (will need to be calculated during set up. only used for rigid bodies)
	glm::vec3 restOffset;

public:
	glm::vec3 position;     // Stores the current position of the particle
	glm::vec3 predictedPosition;    // Predicted particle position
	glm::vec3 deltaPi;

	// Constructors
	Particle();             // Default
	Particle(glm::vec3);    // Construct a particle at a specific position
	Particle(glm::vec3, glm::vec3); // Construct the particle with pos and vel
	Particle(glm::vec3, glm::vec3, int, float); // Construct the particle with pos, vel, phase, mass

	// Getter funcitons
	float getMass();
	void setMass(float mass);
	float getRadius();
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	glm::vec3 getForces();
	glm::vec3 getPredictedPosition();
	float getLambda();
	glm::vec3 getDeltaPi();
	glm::ivec3 getHashPosition();
	float getDensity();

	std::vector<int> getNeighborIndices();

	// Setter functions
	void setPosition(glm::vec3);
	void setVelocity(glm::vec3);
	void setForces(glm::vec3);
	void setPredictedPosition(glm::vec3);
	void setLambda(float);
	void setDeltaPi(glm::vec3);
	void setHashPosition(glm::ivec3);
	void setDensity(float d);

	void addNeighborIndex(int index);
	void clearNeighbors();

	float getPredictedMass();
	void setPredictedMass(float predictedMass);

	int getPhase();
	void setPhase(int phase);

	glm::vec3 getRestOffset();
	void setRestOffset(glm::vec3 restOffset);
};

#endif
