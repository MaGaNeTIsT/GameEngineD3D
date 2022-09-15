#ifndef _ENGINE_SCREEN_POLYGON_2D_VS_HLSL
#define _ENGINE_SCREEN_POLYGON_2D_VS_HLSL

#include "../Common/ShaderCommon.hlsl"

VaryingScreenPolygon2D main(Attribute input)
{
	float4 positionCS = float4(input.position.xy * _CameraViewportMinSizeAndInvBufferSize.zw * 2.0 - 1.0, 0.0, 1.0);
	positionCS.y = -positionCS.y;
	VaryingScreenPolygon2D output;
	output.positionCS = positionCS;
	output.uv0 = input.uv0;
	return output;
}

#endif