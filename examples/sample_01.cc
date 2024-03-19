#include "vita/vita.h"
#include "vita/anim/vec3.h"
#include "vita/anim/vec2.h"
#include "vita/anim/shader.h"
#include "vita/anim/attribute.h"
#include "vita/anim/indexbuffer.h"
#include "vita/anim/texture.h"
#include "vita/anim/mat4.h"
#include "vita/anim/quat.h"
#include "vita/anim/uniform.h"
#include "vita/anim/draw.h"
#include "vita/assets.h"

constexpr float DEG2RAD = 0.0174533f;

struct Sample : public App
{
	Shader* mShader;
	Attribute<vec3>* mVertexPositions;
	Attribute<vec3>* mVertexNormals;
	Attribute<vec2>* mVertexTexCoords;
	IndexBuffer* mIndexBuffer;
	Texture* mDisplayTexture;
	float mRotation;

	Sample();
	~Sample();

	void on_frame(float) override;

	void on_gui() override
	{
	}

	void on_render(float) override;
};

Sample::Sample()
{
	mRotation = 0.0f;
	mShader = new Shader(assets::static_shader(), assets::lit_shader());
	mDisplayTexture = new Texture(assets::uv_texture());

	mVertexPositions = new Attribute<vec3>();
	mVertexNormals = new Attribute<vec3>();
	mVertexTexCoords = new Attribute<vec2>();
	mIndexBuffer = new IndexBuffer();

	std::vector<vec3> positions;
	positions.push_back(vec3(-1, -1, 0));
	positions.push_back(vec3(-1, 1, 0));
	positions.push_back(vec3(1, -1, 0));
	positions.push_back(vec3(1, 1, 0));
	mVertexPositions->Set(positions);

	std::vector<vec3> normals;
	normals.resize(4, vec3(0, 0, 1));
	mVertexNormals->Set(normals);

	std::vector<vec2> uvs;
	uvs.push_back(vec2(0, 0));
	uvs.push_back(vec2(0, 1));
	uvs.push_back(vec2(1, 0));
	uvs.push_back(vec2(1, 1));
	mVertexTexCoords->Set(uvs);

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	mIndexBuffer->Set(indices);
}

void Sample::on_frame(float inDeltaTime)
{
	mRotation += inDeltaTime * 45.0f;
	while (mRotation > 360.0f)
	{
		mRotation -= 360.0f;
	}
}

void Sample::on_render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 model = quatToMat4(angleAxis(mRotation * DEG2RAD, vec3(0, 0, 1)));

	mShader->Bind();

	mVertexPositions->BindTo(mShader->GetAttribute("position"));
	mVertexNormals->BindTo(mShader->GetAttribute("normal"));
	mVertexTexCoords->BindTo(mShader->GetAttribute("texCoord"));

	Uniform<mat4>::Set(mShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mShader->GetUniform("projection"), projection);

	Uniform<vec3>::Set(mShader->GetUniform("light"), vec3(0, 0, 1));

	mDisplayTexture->bind(mShader->GetUniform("tex0"), 0);

	Draw(*mIndexBuffer, DrawMode::Triangles);

	mDisplayTexture->unbind(0);

	mVertexPositions->UnBindFrom(mShader->GetAttribute("position"));
	mVertexNormals->UnBindFrom(mShader->GetAttribute("normal"));
	mVertexTexCoords->UnBindFrom(mShader->GetAttribute("texCoord"));

	mShader->UnBind();
}

Sample::~Sample()
{
	delete mShader;
	delete mDisplayTexture;
	delete mVertexPositions;
	delete mVertexNormals;
	delete mVertexTexCoords;
	delete mIndexBuffer;
}


IMPLEMENT_MAIN(Sample)
