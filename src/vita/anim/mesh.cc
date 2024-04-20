#include "vita/anim/mesh.h"

#include "vita/anim/draw.h"
#include "vita/anim/transform.h"

Mesh::Mesh()
{
	mPosAttrib = new Attribute<vec3>();
	mNormAttrib = new Attribute<vec3>();
	mUvAttrib = new Attribute<vec2>();
	mWeightAttrib = new Attribute<vec4>();
	mInfluenceAttrib = new Attribute<ivec4>();
	mIndexBuffer = new IndexBuffer();
}

Mesh::Mesh(const Mesh& other)
{
	mPosAttrib = new Attribute<vec3>();
	mNormAttrib = new Attribute<vec3>();
	mUvAttrib = new Attribute<vec2>();
	mWeightAttrib = new Attribute<vec4>();
	mInfluenceAttrib = new Attribute<ivec4>();
	mIndexBuffer = new IndexBuffer();
	*this = other;
}

Mesh& Mesh::operator=(const Mesh& other)
{
	if (this == &other)
	{
		return *this;
	}
	mPosition = other.mPosition;
	mNormal = other.mNormal;
	mTexCoord = other.mTexCoord;
	mWeights = other.mWeights;
	mInfluences = other.mInfluences;
	mIndices = other.mIndices;
	UpdateOpenGLBuffers();
	return *this;
}

Mesh::~Mesh()
{
	delete mPosAttrib;
	delete mNormAttrib;
	delete mUvAttrib;
	delete mWeightAttrib;
	delete mInfluenceAttrib;
	delete mIndexBuffer;
}

std::vector<vec3>& Mesh::GetPosition()
{
	return mPosition;
}

std::vector<vec3>& Mesh::GetNormal()
{
	return mNormal;
}

std::vector<vec2>& Mesh::GetTexCoord()
{
	return mTexCoord;
}

std::vector<vec4>& Mesh::GetWeights()
{
	return mWeights;
}

std::vector<ivec4>& Mesh::GetInfluences()
{
	return mInfluences;
}

std::vector<unsigned int>& Mesh::GetIndices()
{
	return mIndices;
}

void Mesh::UpdateOpenGLBuffers()
{
	if (mPosition.size() > 0)
	{
		mPosAttrib->set(mPosition);
	}
	if (mNormal.size() > 0)
	{
		mNormAttrib->set(mNormal);
	}
	if (mTexCoord.size() > 0)
	{
		mUvAttrib->set(mTexCoord);
	}
	if (mWeights.size() > 0)
	{
		mWeightAttrib->set(mWeights);
	}
	if (mInfluences.size() > 0)
	{
		mInfluenceAttrib->set(mInfluences);
	}
	if (mIndices.size() > 0)
	{
		mIndexBuffer->set(mIndices);
	}
}

void Mesh::Bind(int position, int normal, int texCoord, int weight, int influcence)
{
	if (position >= 0)
	{
		mPosAttrib->bind_to(static_cast<unsigned int>(position));
	}
	if (normal >= 0)
	{
		mNormAttrib->bind_to(static_cast<unsigned int>(normal));
	}
	if (texCoord >= 0)
	{
		mUvAttrib->bind_to(static_cast<unsigned int>(texCoord));
	}
	if (weight >= 0)
	{
		mWeightAttrib->bind_to(static_cast<unsigned int>(weight));
	}
	if (influcence >= 0)
	{
		mInfluenceAttrib->bind_to(static_cast<unsigned int>(influcence));
	}
}

void Mesh::Draw()
{
	if (mIndices.size() > 0)
	{
		::draw(*mIndexBuffer, DrawMode::Triangles);
	}
	else
	{
		::draw(static_cast<unsigned int>(mPosition.size()), DrawMode::Triangles);
	}
}

void Mesh::DrawInstanced(unsigned int numInstances)
{
	if (mIndices.size() > 0)
	{
		::draw_instanced(*mIndexBuffer, DrawMode::Triangles, numInstances);
	}
	else
	{
		::draw_instanced(
			static_cast<unsigned int>(mPosition.size()), DrawMode::Triangles, numInstances
		);
	}
}

void Mesh::UnBind(int position, int normal, int texCoord, int weight, int influcence)
{
	if (position >= 0)
	{
		mPosAttrib->unbind_from(static_cast<unsigned int>(position));
	}
	if (normal >= 0)
	{
		mNormAttrib->unbind_from(static_cast<unsigned int>(normal));
	}
	if (texCoord >= 0)
	{
		mUvAttrib->unbind_from(static_cast<unsigned int>(texCoord));
	}
	if (weight >= 0)
	{
		mWeightAttrib->unbind_from(static_cast<unsigned int>(weight));
	}
	if (influcence >= 0)
	{
		mInfluenceAttrib->unbind_from(static_cast<unsigned int>(influcence));
	}
}

#if 1
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose)
{
	const auto numVerts = mPosition.size();
	if (numVerts == 0)
	{
		return;
	}

	mSkinnedPosition.resize(numVerts);
	mSkinnedNormal.resize(numVerts);

	mPosePalette = calc_matrix_palette(pose);
	std::vector<mat4> invPosePalette = skeleton.inverse_bind_pose;

	for (std::size_t i = 0; i < numVerts; ++i)
	{
		const auto& j = mInfluences[i];
		const auto& w = mWeights[i];

		mat4 m0 = (mPosePalette[static_cast<std::size_t>(j.x)]
				   * invPosePalette[static_cast<std::size_t>(j.x)])
				* w.x;
		mat4 m1 = (mPosePalette[static_cast<std::size_t>(j.y)]
				   * invPosePalette[static_cast<std::size_t>(j.y)])
				* w.y;
		mat4 m2 = (mPosePalette[static_cast<std::size_t>(j.z)]
				   * invPosePalette[static_cast<std::size_t>(j.z)])
				* w.z;
		mat4 m3 = (mPosePalette[static_cast<std::size_t>(j.w)]
				   * invPosePalette[static_cast<std::size_t>(j.w)])
				* w.w;

		mat4 skin = m0 + m1 + m2 + m3;

		mSkinnedPosition[i] = get_transformed_point(skin, mPosition[i]);
		mSkinnedNormal[i] = get_transformed_vector(skin, mNormal[i]);
	}

	mPosAttrib->set(mSkinnedPosition);
	mNormAttrib->set(mSkinnedNormal);
}
#else
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose)
{
	unsigned int numVerts = (unsigned int) mPosition.size();
	if (numVerts == 0)
	{
		return;
	}

	mSkinnedPosition.resize(numVerts);
	mSkinnedNormal.resize(numVerts);
	Pose& bindPose = skeleton.GetBindPose();

	for (unsigned int i = 0; i < numVerts; ++i)
	{
		ivec4& joint = mInfluences[i];
		vec4& weight = mWeights[i];

		Transform skin0 = combine(pose[joint.x], inverse(bindPose[joint.x]));
		vec3 p0 = transformPoint(skin0, mPosition[i]);
		vec3 n0 = transformVector(skin0, mNormal[i]);

		Transform skin1 = combine(pose[joint.y], inverse(bindPose[joint.y]));
		vec3 p1 = transformPoint(skin1, mPosition[i]);
		vec3 n1 = transformVector(skin1, mNormal[i]);

		Transform skin2 = combine(pose[joint.z], inverse(bindPose[joint.z]));
		vec3 p2 = transformPoint(skin2, mPosition[i]);
		vec3 n2 = transformVector(skin2, mNormal[i]);

		Transform skin3 = combine(pose[joint.w], inverse(bindPose[joint.w]));
		vec3 p3 = transformPoint(skin3, mPosition[i]);
		vec3 n3 = transformVector(skin3, mNormal[i]);
		mSkinnedPosition[i] = p0 * weight.x + p1 * weight.y + p2 * weight.z + p3 * weight.w;
		mSkinnedNormal[i] = n0 * weight.x + n1 * weight.y + n2 * weight.z + n3 * weight.w;
	}

	mPosAttrib->Set(mSkinnedPosition);
	mNormAttrib->Set(mSkinnedNormal);
}
#endif
