#include "vita/anim/intersections.h"

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint)
{
	const float EPSILON = 0.0000001f;

	const auto& vertex0 = triangle.v0;
	const auto& vertex1 = triangle.v1;
	const auto& vertex2 = triangle.v2;
	const auto& rayVector = ray.direction;
	const auto& rayOrigin = ray.origin;

	const auto edge1 = vertex1 - vertex0;
	const auto edge2 = vertex2 - vertex0;
	const auto h = cross(rayVector, edge2);
	const auto a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
	{
		return false;
	}

	const auto f = 1.0f / a;
	const auto s = rayOrigin - vertex0;
	const auto u = f * dot(s, h);
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	const auto q = cross(s, edge1);
	const auto v = f * dot(rayVector, q);
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	float t = f * dot(edge2, q);
	if (t > EPSILON)
	{
		hitPoint = rayOrigin + rayVector * t;
		return true;
	}

	return false;
}

std::vector<Triangle> MeshToTriangles(const Mesh& mesh)
{
	std::vector<Triangle> result;

	const auto& vertices = mesh.position;
	const auto& indices = mesh.indices;

	if (indices.empty())
	{
		const auto numVertices = vertices.size();
		for (std::size_t i = 0; i < numVertices; i += 3)
		{
			result.emplace_back(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
		}
	}
	else
	{
		const auto numIndices = indices.size();
		for (std::size_t i = 0; i < numIndices; i += 3)
		{
			result.emplace_back(
				vertices[indices[i + 0]], vertices[indices[i + 1]], vertices[indices[i + 2]]
			);
		}
	}

	return result;
}

std::vector<Triangle> MeshesToTriangles(const std::vector<Mesh>& meshes)
{
	std::vector<Triangle> result;
	const auto numMeshes = meshes.size();
	for (std::size_t j = 0; j < numMeshes; ++j)
	{
		const auto& mesh = meshes[j];

		const auto& vertices = mesh.position;
		const auto& indices = mesh.indices;
		if (indices.empty())
		{
			const auto numVertices = vertices.size();
			for (std::size_t i = 0; i < numVertices; i += 3)
			{
				result.emplace_back(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
			}
		}
		else
		{
			const auto numIndices = indices.size();
			for (std::size_t i = 0; i < numIndices; i += 3)
			{
				result.emplace_back(
					vertices[indices[i + 0]], vertices[indices[i + 1]], vertices[indices[i + 2]]
				);
			}
		}
	}
	return result;
}
