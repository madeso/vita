#pragma once

#include <vector>

#include "vita/anim/shader.h"
#include "vita/anim/attribute.h"
#include "vita/anim/vec3.h"
#include "vita/anim/mat4.h"
#include "vita/anim/pose.h"


enum class DebugDrawMode
{
	Lines,
	Loop,
	Strip,
	Points
};

struct DebugDraw
{
   protected:

	std::vector<vec3> mPoints;
	Attribute<vec3>* mAttribs;
	Shader* mShader;

   private:

	DebugDraw(const DebugDraw&);
	DebugDraw& operator=(const DebugDraw&);

   public:

	DebugDraw();
	DebugDraw(unsigned int size);
	~DebugDraw();

	unsigned int Size();
	void Resize(unsigned int newSize);
	vec3& operator[](unsigned int index);
	void Push(const vec3& v);

	void FromPose(const Pose& pose);

	void LinesFromIKSolver(const std::vector<vec3>& points);
	void PointsFromIKSolver(const std::vector<vec3>& points);

	void UpdateOpenGLBuffers();
	void Draw(DebugDrawMode mode, const vec3& color, const mat4& mvp);
};
