#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum LightType {
	DIRECTIONAL,
	POINT
};

enum Light_Movement {
    ELEVATION_UP,//up
    ELEVATION_DOWN,//down
    AZIMUTH_DOWN,//left
    AZIMUTH_UP//right
};

class DirectionalLight {
public:
	float azimuth;
	float elevation;
	glm::vec3 lightDir; // direction of light. If elevation is 90, it would be (0,-1,0)
	glm::vec3 lightColor; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)

	DirectionalLight(float azimuth, float elevation, glm::vec3 lightColor) {
		this->azimuth = azimuth;
		this->elevation = elevation;
		updateLightDir();
		this->lightColor = lightColor;
	}

	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor) {
		this->lightDir = lightDir;
		this->lightColor = lightColor;
	}

	glm::mat4 getViewMatrix(glm::vec3 cameraPosition) {
		// directional light has no light position. Assume fake light position depending on camera position.
		float lightDistance = 15.0f;
		glm::vec3 lightPos = cameraPosition - this->lightDir * lightDistance;
		return glm::lookAt(this->lightDir, cameraPosition, glm::vec3(0, 1, 0));
	}

	glm::mat4 getProjectionMatrix() {
		// For simplicity, just use static projection matrix. (Actually we have to be more accurate with considering camera's frustum)
		return glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 50.0f);
	}

	void updateLightDir() {

        float lightDistance = 15.0f;
		if(elevation < 15.0f) this -> elevation = 15.0f;
		else if (elevation > 80.0f) this -> elevation = 80.0f;
		float rad_elevation = glm::radians(elevation);
		float rad_azimuth = glm::radians(azimuth);
		this -> lightDir = lightDistance * glm::vec3( sin(rad_azimuth)*cos(rad_elevation), sin(rad_elevation),cos(rad_azimuth)*cos(rad_elevation));
        //std::cout << "azimuth : " << this->azimuth <<std::endl;
        //std::cout << "elevation : " << this->elevation <<std::endl;
	}

	void ProcessKeyboard(Light_Movement direction, float amount)
	{

		// set elevation between 15 to 80 (degree)!
		if (direction == ELEVATION_UP) this -> elevation += amount;
        if (direction == ELEVATION_DOWN) this -> elevation -= amount;
        if (direction == AZIMUTH_UP) this -> azimuth += amount;
        if (direction == AZIMUTH_DOWN) this -> azimuth -= amount;
        updateLightDir();

	}
};

#endif