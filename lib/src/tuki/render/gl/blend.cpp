#include "blend.hpp"

#include <glad/glad.h>

static const GLenum lookUpFactorGl[(unsigned)BlendFactor::COUNT] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_DST_COLOR,
	GL_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,

	GL_ONE_MINUS_SRC_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_ALPHA
};

static const GLenum lookUpOperatorGl[(unsigned)BlendOperator::COUNT] =
{
	GL_FUNC_ADD,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_FUNC_SUBTRACT,
	GL_MIN,
	GL_MAX
};

void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha)
{
	glBlendFuncSeparate(
		lookUpFactorGl[(int)rgb.src], lookUpFactorGl[(int)rgb.dst],
		lookUpFactorGl[(int)alpha.src], lookUpFactorGl[(int)alpha.dst]);
	glBlendEquationSeparate(lookUpOperatorGl[(int)rgb.op], lookUpOperatorGl[(int)alpha.op]);
}