#pragma once

#include <stdexcept>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

enum class BlendOperator
{
	PLUS,
	DST_MINUS_SRC,
	SRC_MINUS_DST,
	MIN,
	MAX,

	COUNT
};

enum class BlendFactor
{
	ZERO,
	ONE,
	SRC_RGB,
	DST_RGB,
	SRC_ALPHA,
	DST_ALPHA,
	CONSTANT_RGB,
	CONSTANT_ALPHA,

	ONE_MINUS_SRC_RGB,
	ONE_MINUS_DST_RGB,
	ONE_MINUS_SRC_ALPHA,
	ONE_MINUS_DST_ALPHA,
	ONE_MINUS_CONSTANT_RGB,
	ONE_MINUS_CONSTANT_ALPHA,

	COUNT
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

 BlendFactor operator-(int one, BlendFactor sub) {
	if(one != 1) throw std::logic_error("invalid one (ONE_MINUS)");
	return operator-(BlendFactor::ONE, sub);
}
 BlendFactor operator-(float one, BlendFactor sub) {
	 if (one != 1.f) throw std::logic_error("invalid one (ONE_MINUS)");
	 return operator-(BlendFactor::ONE, sub);
}
 BlendFactor operator-(double one, BlendFactor sub) {
	 if (one != 1.0) throw std::logic_error("invalid one (ONE_MINUS)");
	 return operator-(BlendFactor::ONE, sub);
}

// RGB

struct BlendSrcRGB
{
	BlendFactor blendFactor;
	BlendSrcRGB(BlendFactor blendFactor = BlendFactor::ONE) : blendFactor(blendFactor) {}
};

struct BlendDstRGB
{
	BlendFactor blendFactor;
	BlendDstRGB(BlendFactor blendFactor = BlendFactor::ONE) : blendFactor(blendFactor) {}
};

struct BlendEquationRGB
{
	BlendFactor src;
	BlendFactor dst;
	BlendOperator op;
	BlendEquationRGB(BlendFactor src, BlendFactor dst, BlendOperator op) : src(src), dst(dst), op(op) {}
	BlendEquationRGB(BlendSrcRGB blendSrc) : src(blendSrc.blendFactor), dst(BlendFactor::ZERO), op(BlendOperator::PLUS) {}
	BlendEquationRGB(BlendDstRGB blendDst) : src(BlendFactor::ZERO), dst(blendDst.blendFactor), op(BlendOperator::PLUS) {}
};

 BlendEquationRGB operator+(BlendSrcRGB src, BlendDstRGB dst)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::PLUS);
}
 BlendEquationRGB operator+(BlendDstRGB dst, BlendSrcRGB src)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::PLUS);
}

 BlendEquationRGB operator-(BlendSrcRGB src, BlendDstRGB dst)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::SRC_MINUS_DST);
}
 BlendEquationRGB operator-(BlendDstRGB dst, BlendSrcRGB src)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::DST_MINUS_SRC);
}

 BlendEquationRGB MIN(BlendSrcRGB src, BlendDstRGB dst)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::MIN);
}
 BlendEquationRGB MIN(BlendDstRGB dst, BlendSrcRGB src)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::MIN);
}

 BlendEquationRGB MAX(BlendSrcRGB src, BlendDstRGB dst)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::MAX);
}
 BlendEquationRGB MAX(BlendDstRGB dst, BlendSrcRGB src)
{
	return BlendEquationRGB(src.blendFactor, dst.blendFactor, BlendOperator::MAX);
}

// Alpha

struct BlendSrcAlpha
{
	BlendFactor blendFactor;
	BlendSrcAlpha(BlendFactor blendFactor = BlendFactor::ONE) : blendFactor(blendFactor) {}
};

struct BlendDstAlpha
{
	BlendFactor blendFactor;
	BlendDstAlpha(BlendFactor blendFactor = BlendFactor::ONE) : blendFactor(blendFactor) {}
};

struct BlendEquationAlpha
{
	BlendFactor src;
	BlendFactor dst;
	BlendOperator op;
	BlendEquationAlpha(BlendFactor src, BlendFactor dst, BlendOperator op) : src(src), dst(dst), op(op) {}
	BlendEquationAlpha(BlendSrcAlpha& srcAlpha) : src(srcAlpha.blendFactor), dst(BlendFactor::ZERO), op(BlendOperator::PLUS) {}
	BlendEquationAlpha(BlendDstAlpha& dstAlpha) : src(BlendFactor::ZERO), dst(dstAlpha.blendFactor), op(BlendOperator::PLUS) {}
};

BlendEquationAlpha operator+(BlendSrcAlpha src, BlendDstAlpha dst)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::PLUS);
}
BlendEquationAlpha operator+(BlendDstAlpha dst, BlendSrcAlpha src)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::PLUS);
}

BlendEquationAlpha operator-(BlendSrcAlpha src, BlendDstAlpha dst)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::SRC_MINUS_DST);
}
BlendEquationAlpha operator-(BlendDstAlpha dst, BlendSrcAlpha src)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::DST_MINUS_SRC);
}

BlendEquationAlpha MIN(BlendSrcAlpha src, BlendDstAlpha dst)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::MIN);
}
BlendEquationAlpha MIN(BlendDstAlpha dst, BlendSrcAlpha src)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::MIN);
}

BlendEquationAlpha MAX(BlendSrcAlpha src, BlendDstAlpha dst)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::MAX);
}
BlendEquationAlpha MAX(BlendDstAlpha dst, BlendSrcAlpha src)
{
	return BlendEquationAlpha(src.blendFactor, dst.blendFactor, BlendOperator::MAX);
}

void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha);
void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha, glm::vec4 constant);
void setGraphicsApiBlendEquations(BlendEquationRGB rgb, BlendEquationAlpha alpha, glm::vec3 constantColor, float constantAlpha);