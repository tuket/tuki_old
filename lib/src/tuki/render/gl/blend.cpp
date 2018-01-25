#include "blend.hpp"

#include <glad/glad.h>

using namespace glm;

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

BlendFactor operator-(BlendFactor one, BlendFactor sub) {
	if (one != BlendFactor::ONE) throw std::logic_error("invalid one (ONE_MINUS)");
	switch (sub)
	{
	case BlendFactor::ZERO: return BlendFactor::ONE;
	case BlendFactor::ONE: return BlendFactor::ZERO;
	case  BlendFactor::SRC_RGB: return BlendFactor::ONE_MINUS_SRC_RGB;
	case  BlendFactor::DST_RGB: return BlendFactor::ONE_MINUS_DST_RGB;
	case BlendFactor::SRC_ALPHA: return BlendFactor::ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DST_ALPHA: return BlendFactor::ONE_MINUS_DST_ALPHA;
	case BlendFactor::CONSTANT_RGB: return BlendFactor::ONE_MINUS_CONSTANT_RGB;
	case BlendFactor::CONSTANT_ALPHA: return BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
	default: throw std::logic_error("invalid sub (ONE_MINUS)");
	}
}

void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha)
{
	glBlendFuncSeparate(
		lookUpFactorGl[(int)rgb.src], lookUpFactorGl[(int)rgb.dst],
		lookUpFactorGl[(int)alpha.src], lookUpFactorGl[(int)alpha.dst]);
	glBlendEquationSeparate(lookUpOperatorGl[(int)rgb.op], lookUpOperatorGl[(int)alpha.op]);
}

void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha,
	vec4 c)
{
	glBlendColor(c.r, c.g, c.b, c.a);
	setGraphicsApiBlendEquations(rgb, alpha);
}

void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha,
	vec3 c, float a)
{
	glBlendColor(c.r, c.g, c.b, a);
	setGraphicsApiBlendEquations(rgb, alpha);
}