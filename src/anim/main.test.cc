#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "anim/assert.h"

int main(int argc, char* argv[])
{
	enable_exception_on_assert();
	return Catch::Session().run(argc, argv);
}
