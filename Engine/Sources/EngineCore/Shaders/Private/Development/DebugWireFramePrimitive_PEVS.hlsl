#ifndef _DEBUG_WIREFRAME_PRIMITIVE_PE_VS_HLSL
#define _DEBUG_WIREFRAME_PRIMITIVE_PE_VS_HLSL

#define SHADER_USE_VIEW_INPUT		1
#define SHADER_VIEW_INPUT_SLOT		b0

#include "../../Public/ShaderVariables.hlsl"

StructuredBuffer<float4x4>	_DebugWireframePrimitiveTransformInput	: register(t0);
StructuredBuffer<float4>	_DebugWireframePrimitiveColorInput		: register(t1);

void main(in float3 InPosition : POSITION0, in uint InInstanceID : SV_InstanceID, out float4 OutPosition : SV_Position, out float4 OutColor : COLOR0)
{
	OutPosition	= mul(float4(mul(float4(InPosition.xyz, 1.0), _DebugWireframePrimitiveTransformInput[InInstanceID]).xyz, 1.0), _ViewProjectionMatrix).xyzw;
	OutColor	= float4(_DebugWireframePrimitiveColorInput[InInstanceID].xyz, 1.0);
}

#endif