#pragma once

#include <cmath>
#include <vector>

#include "vita/anim/vec3.h"
#include "vita/anim/mesh.h"

struct Ray
{
	vec3 origin;
	vec3 direction = vec3(0, -1, 0);

	Ray() = default;

	explicit Ray(const vec3& o)
		: origin(o)
	{
	}

	Ray(const vec3& o, const vec3& d)
		: origin(o)
		, direction(d)
	{
	}
};

struct Triangle
{
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;

	Triangle() = default;

	Triangle(const vec3& av0, const vec3& av1, const vec3& av2)
		: v0(av0)
		, v1(av1)
		, v2(av2)
	{
		normal = get_normalized(cross(v1 - v0, v2 - v0));
	}
};

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint);
std::vector<Triangle> MeshToTriangles(const Mesh& mesh);
std::vector<Triangle> MeshesToTriangles(const std::vector<Mesh>& mesh);
