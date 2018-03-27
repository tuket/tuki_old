#pragma once

class RenderTarget;
class SceneNode;

namespace RenderPass
{
	
void forwardRender(RenderTarget& rt, SceneNode* node);

}

/* Call this function on each render pass that you want
   its shaders to be cached
   That will prevent compiling the shaders the first time
   you call the render pass */
template<RenderPassFunction>
void compileRenderPassShaders();

void compileAllRenderPassShaders();