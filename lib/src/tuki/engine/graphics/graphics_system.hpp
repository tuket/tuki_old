#pragma once

#include <tuki/engine/scene/component/component.hpp>
#include "../engine.hpp"
#include <vector>
#include <array>
#include <string>

class Renderer3D;

class GraphicsSystem : public EngineSystem
{
public:
	std::string getName()const override { return "GraphicsSystem"; }
	void onInit()override;
	void onUpdate(float dt)override;

	unsigned getWindowWidth()const { return w; }
	unsigned getWindowHeight()const { return h; }
	void setWindowDimensions(unsigned w, unsigned h);
	bool isFullScreen()const { return fullScreen; }
	void setFullScreen(bool yes = true);
	void setWindowPosition(int x, int y);
	
private:
	unsigned x, y;
	unsigned w, h;
	bool fullScreen;
	Renderer3D* renderer3d;
};