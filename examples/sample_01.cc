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
	mVertexPositions->set(positions);

	std::vector<vec3> normals;
	normals.resize(4, vec3(0, 0, 1));
	mVertexNormals->set(normals);

	std::vector<vec2> uvs;
	uvs.push_back(vec2(0, 0));
	uvs.push_back(vec2(0, 1));
	uvs.push_back(vec2(1, 0));
	uvs.push_back(vec2(1, 1));
	mVertexTexCoords->set(uvs);

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	mIndexBuffer->set(indices);
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
	mat4 projection = mat4_from_perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = mat4_from_look_at(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 model = mat4_from_quat(quat_from_angle_axis(mRotation * DEG2RAD, vec3(0, 0, 1)));

	mShader->bind();

	mVertexPositions->bind_to(mShader->get_attribute("position"));
	mVertexNormals->bind_to(mShader->get_attribute("normal"));
	mVertexTexCoords->bind_to(mShader->get_attribute("texCoord"));

	Uniform<mat4>::Set(mShader->get_uniform("model"), model);
	Uniform<mat4>::Set(mShader->get_uniform("view"), view);
	Uniform<mat4>::Set(mShader->get_uniform("projection"), projection);

	Uniform<vec3>::Set(mShader->get_uniform("light"), vec3(0, 0, 1));

	mDisplayTexture->bind(mShader->get_uniform("tex0"), 0);

	draw(*mIndexBuffer, DrawMode::Triangles);

	mDisplayTexture->unbind(0);

	mVertexPositions->unbind_from(mShader->get_attribute("position"));
	mVertexNormals->unbind_from(mShader->get_attribute("normal"));
	mVertexTexCoords->unbind_from(mShader->get_attribute("texCoord"));

	mShader->unbind();
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
