#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <stdio.h>
#include <ctime>
#include <assert.h>
#include <io.h>
#include <tchar.h>
#include <dwmapi.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <functional>
using namespace std;

#include <d3d11.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "xaudio2.lib")
using namespace DirectX;

//#include <fbxsdk.h>
//#pragma comment (lib, "libfbxsdk-mt.lib")
//#pragma comment (lib, "libxml2-mt.lib")
//#pragma comment (lib, "zlib-mt.lib")

#include "../ThirdParty/imgui/imgui.h"

#define ENGINE_FLOAT32_MAX									(3.402823466e+38f)
#define	ENGINE_DEFAULT_STENCIL_READ_MASK					(0xff)
#define	ENGINE_DEFAULT_STENCIL_WRITE_MASK					(0xff)

#define ENGINE_SCREEN_WIDTH									(1600)
#define ENGINE_SCREEN_HEIGHT								(900)

#define ENGINE_FIXED_UPDATE_FRAME							(60)
#define ENGINE_UPDATE_FRAME									(90)
#define ENGINE_WINDOWED										(1)

#define ENGINE_CONSTANT_BUFFER_PER_FRAME_START_SLOT			(0u)
#define ENGINE_CONSTANT_BUFFER_PER_DRAW_START_SLOT			(1u)

#define ENGINE_SAMPLER_ALL_START_SLOT						(0u)
#define ENGINE_SAMPLER_POINT_CLAMP_START_SLOT				(ENGINE_SAMPLER_ALL_START_SLOT + 0u)
#define ENGINE_SAMPLER_POINT_WRAP_START_SLOT				(ENGINE_SAMPLER_ALL_START_SLOT + 1u)
#define ENGINE_SAMPLER_LINEAR_CLAMP_START_SLOT				(ENGINE_SAMPLER_ALL_START_SLOT + 2u)
#define ENGINE_SAMPLER_LINEAR_WRAP_START_SLOT				(ENGINE_SAMPLER_ALL_START_SLOT + 3u)
#define ENGINE_GBUFFER_ALL_START_SLOT						(0u)
#define ENGINE_GBUFFER_COLOR_START_SLOT						(ENGINE_GBUFFER_ALL_START_SLOT + 0u)
#define ENGINE_GBUFFER_NORMAL_START_SLOT					(ENGINE_GBUFFER_ALL_START_SLOT + 1u)
#define ENGINE_GBUFFER_ALBEDO_START_SLOT					(ENGINE_GBUFFER_ALL_START_SLOT + 2u)
#define ENGINE_GBUFFER_PROPERTY_START_SLOT					(ENGINE_GBUFFER_ALL_START_SLOT + 3u)

#define ENGINE_SRV_CAMERA_COLOR								(0u)
#define ENGINE_SRV_CAMERA_DEPTH								(1u)
#define ENGINE_SRV_LIGHT_SHADOW_MAP_START_SLOT				(2u)
#define ENGINE_TEXTURE2D_ALBEDO_START_SLOT					(3u)
#define ENGINE_TEXTURE2D_NORMAL_START_SLOT					(4u)
#define ENGINE_TEXTURE2D_PROPERTY_START_SLOT				(5u)

#define ENGINE_TEXTURE2D_DEFAULT_WHITE						("./Engine/Assets/EngineTextures/Default/EngineDefaultWhite.tga")
#define ENGINE_TEXTURE2D_DEFAULT_BLACK						("./Engine/Assets/EngineTextures/Default/EngineDefaultBlack.tga")
#define ENGINE_TEXTURE2D_DEFAULT_GRAY						("./Engine/Assets/EngineTextures/Default/EngineDefaultGray.tga")
#define ENGINE_TEXTURE2D_DEFAULT_RED						("./Engine/Assets/EngineTextures/Default/EngineDefaultRed.tga")
#define ENGINE_TEXTURE2D_DEFAULT_GREEN						("./Engine/Assets/EngineTextures/Default/EngineDefaultGreen.tga")
#define ENGINE_TEXTURE2D_DEFAULT_BLUE						("./Engine/Assets/EngineTextures/Default/EngineDefaultBlue.tga")
#define ENGINE_TEXTURE2D_DEFAULT_BUMP						("./Engine/Assets/EngineTextures/Default/EngineDefaultBump.tga")
#define ENGINE_TEXTURE2D_DEFAULT_PROPERTY					("./Engine/Assets/EngineTextures/Default/EngineDefaultProperty.tga")

#define ENGINE_SHADER_NONE									("EngineBase/Shader/None")
#define ENGINE_SHADER_DEFAULT_VS							("./Engine/Assets/EngineShaders/EngineDefaultVS.cso")
#define ENGINE_SHADER_DEFAULT_PS							("./Engine/Assets/EngineShaders/EngineDefaultPS.cso")
#define ENGINE_SHADER_EMPTY_PS								("./Engine/Assets/EngineShaders/EngineEmptyPS.cso")
#define ENGINE_SHADER_GBUFFER_WRITE_PS						("./Engine/Assets/EngineShaders/EngineGBufferWritePS.cso")
#define ENGINE_SHADER_DIRECT_LIGHT_PS						("./Engine/Assets/EngineShaders/EngineDirectLightPS.cso")
#define ENGINE_SHADER_POST_EFFECT_EXPOSURE_PS				("./Engine/Assets/EngineShaders/EnginePostEffectExposurePS.cso")
#define ENGINE_SHADER_SCREEN_POLYGON_2D_VS					("./Engine/Assets/EngineShaders/EngineScreenPolygon2DVS.cso")
#define ENGINE_SHADER_SCREEN_POLYGON_2D_PS					("./Engine/Assets/EngineShaders/EngineScreenPolygon2DPS.cso")

#define ENGINE_MESH_CUBE_NAME								("EngineBase/Mesh/CubeMesh")
#define ENGINE_MESH_PLANE_NAME								("EngineBase/Mesh/PlaneMesh")
#define ENGINE_MESH_POLYGON_NAME							("EngineBase/Mesh/PolygonMesh")
#define ENGINE_MESH_POLYGON_2D_NAME							("EngineBase/Mesh/Polygon2DMesh")

#define ENGINE_GPU_PROFILER_NONE							("EngineBase/GPUProfiler/None")
#define ENGINE_GPU_PROFILER_RECORD_FRAME_COUNT				(5)
#define ENGINE_GPU_QUERY_NONE								("EngineBase/GPUQuery/None")

#define ENGINE_DEFAULT_CULLING_DISTANCE						(1500.f)
#define ENGINE_DEFAULT_CULLING_OFFSET						(-5.f)

#define ENGINE_CAMERA_FOV									(60.f)
#define ENGINE_CAMERA_NEAR									(5.f)
#define ENGINE_CAMERA_FAR									(2000.f)
#define ENGINE_CAMERA_MOVE_SPEED							(20.f)
#define ENGINE_CAMERA_LOOK_SPEED							(270.f)

#define ENGINE_SHADOW_MAP_SIZE								(4096)