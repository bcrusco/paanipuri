//
//  Collision.cpp
//  Paanipuri
//
//  Bradley Crusco, Sanchit Garg, Debanshu Singh
//  Copyright (c) 2015 Crusco, Gar, Singh. All rights reserved.
//

#include "Collision.h"

Collision::Collision(const int id, const glm::vec3 normal, const double distance) {
	_id = id;
	_normal = normal;
	_distance = distance;
}

Collision::~Collision() {

}
