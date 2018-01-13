#pragma once

class RenderTarget;
class Scene;
class GraphicsSystem;
class Transform;

class IRenderer3D
{
	
public:
	IRenderer3D(GraphicsSystem* graphicsSystem)
		:graphicsSystem(graphicsSystem) {}

	virtual void onSizeChanged(unsigned w, unsigned h) = 0;
		
	virtual void draw(RenderTarget& output, const Transform& cameraTransform, Scene* scene) = 0;
	
private:
	GraphicsSystem* graphicsSystem;
};