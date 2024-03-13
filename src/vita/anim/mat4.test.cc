#include "catch.hpp"

#include "vita/anim/mat4.h"

TEST_CASE("basic mat4 test")
{
	mat4 test = mat4(
		0.928477f,
		0,
		-0.371391f,
		0,
		-0.0996546f,
		0.963328f,
		-0.249136f,
		0,
		0.357771f,
		0.268328f,
		0.894427f,
		0,
		4,
		3,
		10,
		1
	);
	mat4 inv = inverse(test);

	mat4 identity = test * inv;

	// ERROR, expected inverse * matrix to be identity
	CHECK(identity == mat4());
}
