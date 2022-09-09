#ifndef _ENGINE_GBUFFER_RESOLVE_PS_HLSL
#define _ENGINE_GBUFFER_RESOLVE_PS_HLSL

#include "../Common/ShaderCommon.hlsl"

DeferredOutput main(Varying input)
{
	float3 normalModelWS	= SafeNormalize(input.normal.xyz);
	float3 normalWS			= TransformTangentToSpaceDir(SafeNormalize(_NormalTexture.Sample(_LinearWrapSampler, input.uv0).rgb * 2 - 1), CreateTangentMatrix(normalModelWS, SafeNormalize(input.tangent.xyz)));
	float3 albedo			= _AlbedoTexture.Sample(_LinearWrapSampler, input.uv0).rgb;
	float4 property			= _PropertyTexture.Sample(_LinearWrapSampler, input.uv0).rgba;

	DeferredOutput output;
	output.normalWS	= float4(normalWS, 0);
	output.albedo	= float4(albedo, 1);
	output.property = property;
	output.id		= 0;
	return output;
}

#endif