#pragma once

#include "renderer_3d.hpp"

class DefaultRenderer3D : public IRenderer3D
{
public:

	DefaultRenderer3D(GraphicsSystem* graphicsSystem);

	virtual void onSizeChanged(unsigned w, unsigned h);

	virtual void draw(RenderTarget& output, const Transform& cameraTransform, Scene* scene);

};