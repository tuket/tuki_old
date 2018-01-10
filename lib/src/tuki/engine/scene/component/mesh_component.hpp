#pragma once

#include "component.hpp"

class IMeshGpu;

class MeshComponent : public IComponent
{
public:

private:
	IMeshGpu* meshGpu;
};