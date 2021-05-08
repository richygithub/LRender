/*
 * @Author: libl
 * @Date: 2021-04-26 15:19:10
 * @LastEditors: libl
 * @Description: 
 */
#pragma once
#include "glm\glm.hpp"

#define REAL_ZERO 0.01f


enum class Rel{
	LEFT,
	RIGHT,
	ON,
	INSIDE,
	OUTSIDE,
};

Rel inCircle2D(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p);
bool toLeft2D(glm::vec3 a, glm::vec3 b, glm::vec3 c);
Rel relation_point_seg(glm::vec3 a, glm::vec3 b, glm::vec3 p);

glm::vec3 triCenter2D(glm::vec3 a, glm::vec3 b, glm::vec3 c);
