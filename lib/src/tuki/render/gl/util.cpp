#include "util.hpp"

#include <iostream>
#include <string>
#include <glad/glad.h>

using namespace std;

bool checkGlErrors()
{
	GLenum err = glGetError();
	bool ret = false;
	while (err != GL_NO_ERROR) {
		ret = true;
		string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		cerr << "GL_" << error.c_str() << endl;
		err = glGetError();
	}
	return ret;
}