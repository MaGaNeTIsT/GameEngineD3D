#include "SceneRenderer.h"

namespace PigeonEngine
{

	static void RegisterClassTypes()
	{
		RegisterClassType<RSceneRenderer, EManagerBase>();
	}

	PE_REGISTER_CLASS_TYPE(&RegisterClassTypes);

	RSceneRenderer::RSceneRenderer()
	{
	}
	RSceneRenderer::~RSceneRenderer()
	{
	}
	void RSceneRenderer::Initialize()
	{
	}
	void RSceneRenderer::ShutDown()
	{
	}

};