#pragma once

#include "framework.h"

class Material
{
public:

	//you can access this variables directly, do not need a setter/getter

	Vector3 ambient; //reflected ambient light
	Vector3 diffuse; //reflected diffuse light
	Vector3 specular; //reflected specular light
	float shininess; //glosiness coefficient (plasticity)

	Material();
	
};

