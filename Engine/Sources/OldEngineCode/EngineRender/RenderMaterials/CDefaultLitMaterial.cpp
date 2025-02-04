/*
#include "../Headers/CDefaultLitMaterial.h"
#include "../../RenderBase/Headers/CRenderStructCommon.h"
#include "../../AssetsManager/Headers/CShader.h"
#include "../../AssetsManager/Headers/CShaderManager.h"
#include "../../AssetsManager/Headers/CTextureType.h"
#include "../../AssetsManager/Headers/CTextureManager.h"
#include "../../RenderBase/Headers/CRenderDevice.h"
#include "../../RenderBase/Headers/CRenderPipeline.h"

const static CustomStruct::CRenderInputLayoutDesc _GDefaultMeshInputLayout[4u] = {
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_NORMAL),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TANGENT),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD) };
const static CustomStruct::CRenderInputLayoutDesc _GSkeletonMeshInputLayout[6u] = {
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_NORMAL),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TANGENT),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDINDICES),
	CustomStruct::CRenderInputLayoutDesc(CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDWEIGHT) };

CDefaultLitMaterial::CDefaultLitMaterial() : CMaterialBase(typeid(CDefaultLitMaterial).name(), MaterialType::MATERIAL_TYPE_OPAQUE_FORWARD, sizeof(CDefaultLitMaterial::RenderParams), _GDefaultMeshInputLayout, 4u, ENGINE_DEFAULT_LIT_SHADER_FORWARD_VS, ENGINE_DEFAULT_LIT_SHADER_FORWARD_PS)
{
	this->m_VertexShader	= CShaderManager::LoadVertexShader(ENGINE_DEFAULT_LIT_SHADER_FORWARD_VS, _GDefaultMeshInputLayout, 4u);
	this->m_PixelShader		= CShaderManager::LoadPixelShader(ENGINE_DEFAULT_LIT_SHADER_FORWARD_PS);
#ifdef _DEVELOPMENT_EDITOR
	this->m_IsGlossy				= FALSE;
	this->m_NormalSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP;
	this->m_AlbedoSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_EmissiveSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK;
	this->m_RoughnessSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_MetallicnessSelect		= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_AmbientOcclusionSelect	= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_ReflectanceSelect		= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	{
		auto initPathChar = [](CHAR* path, const std::string& str) {
			strcpy_s(path, 512, str.c_str()); };
		initPathChar(this->m_NormalPath, "Default Bump");
		initPathChar(this->m_AlbedoPath, "Default White");
		initPathChar(this->m_EmissivePath, "Default Black");
		initPathChar(this->m_RoughnessPath, "Default White");
		initPathChar(this->m_MetallicnessPath, "Default Black");
		initPathChar(this->m_AmbientOcclusionPath, "Default White");
		initPathChar(this->m_ReflectancePath, "Default White");
	}
#endif
	this->m_NormalTexture			= NULL;
	this->m_AlbedoTexture			= NULL;
	this->m_EmissiveTexture			= NULL;
	this->m_RoughnessTexture		= NULL;
	this->m_MetallicnessTexture		= NULL;
	this->m_AmbientOcclusionTexture	= NULL;
	this->m_ReflectanceTexture		= NULL;

	this->m_RenderParams = new CDefaultLitMaterial::RenderParams();
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness		= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->EmissiveAmbientOcclusion	= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->MetallicnessReflectanceIsGlossy = DirectX::XMFLOAT4(0.f, 1.f, 0.f, 0.f);
}
CDefaultLitMaterial::CDefaultLitMaterial(const std::string& name, MaterialType materialType, const UINT32& constantSize, const CustomStruct::CRenderInputLayoutDesc* inputLayout, const UINT32& inputLayoutNum, const std::string& vertexShaderName, const std::string& pixelShaderName) : CMaterialBase(name, materialType, constantSize, inputLayout, inputLayoutNum, vertexShaderName, pixelShaderName)
{
	this->m_VertexShader			= CShaderManager::LoadVertexShader(vertexShaderName, inputLayout, inputLayoutNum);
	this->m_PixelShader				= CShaderManager::LoadPixelShader(pixelShaderName);
#ifdef _DEVELOPMENT_EDITOR
	this->m_IsGlossy				= FALSE;
	this->m_NormalSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP;
	this->m_AlbedoSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_EmissiveSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK;
	this->m_RoughnessSelect			= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_MetallicnessSelect		= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_AmbientOcclusionSelect	= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_ReflectanceSelect		= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	{
		auto initPathChar = [](CHAR* path, const std::string& str) {
			strcpy_s(path, 512, str.c_str()); };
		initPathChar(this->m_NormalPath, "Default Bump");
		initPathChar(this->m_AlbedoPath, "Default White");
		initPathChar(this->m_EmissivePath, "Default Black");
		initPathChar(this->m_RoughnessPath, "Default White");
		initPathChar(this->m_MetallicnessPath, "Default White");
		initPathChar(this->m_AmbientOcclusionPath, "Default White");
		initPathChar(this->m_ReflectancePath, "Default White");
	}
#endif
	this->m_NormalTexture			= NULL;
	this->m_AlbedoTexture			= NULL;
	this->m_EmissiveTexture			= NULL;
	this->m_RoughnessTexture		= NULL;
	this->m_MetallicnessTexture		= NULL;
	this->m_AmbientOcclusionTexture	= NULL;
	this->m_ReflectanceTexture		= NULL;

	this->m_RenderParams			= nullptr;
}
CDefaultLitMaterial::~CDefaultLitMaterial()
{
	if (this->m_RenderParams != nullptr)
	{
		delete (this->m_RenderParams);
		this->m_RenderParams = nullptr;
	}
}
void CDefaultLitMaterial::SetNormalTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_NormalTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_NormalSelect = -2;
		strcpy_s(this->m_NormalPath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetAlbedoTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_AlbedoTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_AlbedoSelect = -2;
		strcpy_s(this->m_AlbedoPath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetEmissiveTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_EmissiveTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_EmissiveSelect = -2;
		strcpy_s(this->m_EmissivePath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetRoughnessTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_RoughnessTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_RoughnessSelect = -2;
		strcpy_s(this->m_RoughnessPath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetMetallicnessTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_MetallicnessTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_MetallicnessSelect = -2;
		strcpy_s(this->m_MetallicnessPath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetAmbientOcclusionTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_AmbientOcclusionTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_AmbientOcclusionSelect = -2;
		strcpy_s(this->m_AmbientOcclusionPath, 512, tex->GetName().c_str());
#endif
	}
}
void CDefaultLitMaterial::SetReflectanceTexture(class CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_ReflectanceTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_ReflectanceSelect = -2;
		strcpy_s(this->m_ReflectancePath, 512, tex->GetName().c_str());
#endif
	}
}
CTexture2D* CDefaultLitMaterial::GetNormalTexture()const
{
	return (this->m_NormalTexture);
}
CTexture2D* CDefaultLitMaterial::GetAlbedoTexture()const
{
	return (this->m_AlbedoTexture);
}
CTexture2D* CDefaultLitMaterial::GetEmissiveTexture()const
{
	return (this->m_EmissiveTexture);
}
CTexture2D* CDefaultLitMaterial::GetRoughnessTexture()const
{
	return (this->m_RoughnessTexture);
}
CTexture2D* CDefaultLitMaterial::GetMetallicnessTexture()const
{
	return (this->m_MetallicnessTexture);
}
CTexture2D* CDefaultLitMaterial::GetAmbientOcclusionTexture()const
{
	return (this->m_AmbientOcclusionTexture);
}
CTexture2D* CDefaultLitMaterial::GetReflectanceTexture()const
{
	return (this->m_ReflectanceTexture);
}
void CDefaultLitMaterial::SetIsGlossyRoughness(const BOOL32& v)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.z = v ? 1.f : 0.f;
}
void CDefaultLitMaterial::SetRoughness(const FLOAT& v)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness.w = v;
}
void CDefaultLitMaterial::SetMetallicness(const FLOAT& v)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.x = v;
}
void CDefaultLitMaterial::SetReflectance(const FLOAT& v)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.y = v;
}
void CDefaultLitMaterial::SetAmbientOcclusion(const FLOAT& v)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->EmissiveAmbientOcclusion.w = v;
}
void CDefaultLitMaterial::SetBaseColor(const CustomStruct::CColor& clr)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness.x = clr.r;
	renderParams->BaseColorRoughness.y = clr.g;
	renderParams->BaseColorRoughness.z = clr.b;
}
void CDefaultLitMaterial::SetEmissiveColor(const CustomStruct::CColor& clr)
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->EmissiveAmbientOcclusion.x = clr.r;
	renderParams->EmissiveAmbientOcclusion.y = clr.g;
	renderParams->EmissiveAmbientOcclusion.z = clr.b;
}
BOOL32 CDefaultLitMaterial::GetIsGlossyRoughness()const
{
	return (this->m_IsGlossy);
}
FLOAT CDefaultLitMaterial::GetRoughness()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->BaseColorRoughness.w;
	return v;
}
FLOAT CDefaultLitMaterial::GetMetallicness()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->MetallicnessReflectanceIsGlossy.x;
	return v;
}
FLOAT CDefaultLitMaterial::GetReflectance()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->MetallicnessReflectanceIsGlossy.y;
	return v;
}
FLOAT CDefaultLitMaterial::GetAmbientOcclusion()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->EmissiveAmbientOcclusion.w;
	return v;
}
CustomStruct::CColor CDefaultLitMaterial::GetBaseColor()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	 return (CustomStruct::CColor(renderParams->BaseColorRoughness.x, renderParams->BaseColorRoughness.y, renderParams->BaseColorRoughness.z));
}
CustomStruct::CColor CDefaultLitMaterial::GetEmissiveColor()const
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	return (CustomStruct::CColor(renderParams->EmissiveAmbientOcclusion.x, renderParams->EmissiveAmbientOcclusion.y, renderParams->EmissiveAmbientOcclusion.z));
}
void CDefaultLitMaterial::Init()
{
#ifdef _DEVELOPMENT_EDITOR
	{
		std::map<INT32, std::string> baseEngineTextureItems = {
			{ -1, "None" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE, "Default White" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK, "Default Black" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GRAY, "Default Gray" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED, "Default Red" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GREEN, "Default Green" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLUE, "Default Blue" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP, "Default Bump" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_PROPERTY, "Default Property" },
			{ -2, "Custom Path" } };
		auto pathSelect = [&baseEngineTextureItems](const INT32& select, CHAR* name, CTexture2D*& texture, const BOOL32& sRGB = TRUE) {
			if (select != -2)
			{
				strcpy_s(name, 512, baseEngineTextureItems[select].c_str());
				if (select != -1)
				{
					texture = CRenderPipeline::GetDefaultTexture(static_cast<CustomStruct::CEngineDefaultTexture2DType>(select));
				}
			}
			else
			{
				std::string tempName(name);
				texture = CTextureManager::LoadTexture2D(tempName, sRGB);
			}};
		pathSelect(this->m_AlbedoSelect, this->m_AlbedoPath, this->m_AlbedoTexture);
		pathSelect(this->m_EmissiveSelect, this->m_EmissivePath, this->m_EmissiveTexture);
		pathSelect(this->m_NormalSelect, this->m_NormalPath, this->m_NormalTexture, FALSE);
		pathSelect(this->m_RoughnessSelect, this->m_RoughnessPath, this->m_RoughnessTexture, FALSE);
		pathSelect(this->m_MetallicnessSelect, this->m_MetallicnessPath, this->m_MetallicnessTexture, FALSE);
		pathSelect(this->m_AmbientOcclusionSelect, this->m_AmbientOcclusionPath, this->m_AmbientOcclusionTexture, FALSE);
		pathSelect(this->m_ReflectanceSelect, this->m_ReflectancePath, this->m_ReflectanceTexture, FALSE);
	}
#endif

	{
		auto nullTextureSelect = [](CTexture2D*& texture, CustomStruct::CEngineDefaultTexture2DType type) {
			if (texture == NULL)
			{
				texture = CRenderPipeline::GetDefaultTexture(type);
			}};
		nullTextureSelect(this->m_NormalTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP);
		nullTextureSelect(this->m_AlbedoTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
		nullTextureSelect(this->m_EmissiveTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK);
		nullTextureSelect(this->m_RoughnessTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
		nullTextureSelect(this->m_MetallicnessTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
		nullTextureSelect(this->m_AmbientOcclusionTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
		nullTextureSelect(this->m_ReflectanceTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	}
}
void CDefaultLitMaterial::Bind()const
{
	auto bindTexture = [](const CTexture2D* tex, const UINT32& slot, CustomStruct::CEngineDefaultTexture2DType type) {
		if (tex != NULL)
		{
			CRenderDevice::BindPSShaderResourceView(tex->GetShaderResourceView(), slot);
		}
		else
		{
			CRenderDevice::BindPSShaderResourceView(CRenderPipeline::GetDefaultTexture(type)->GetShaderResourceView(), slot);
		}};
	bindTexture(this->m_NormalTexture, ENGINE_TEXTURE2D_CUSTOM_A_START_SLOT, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP);
	bindTexture(this->m_AlbedoTexture, ENGINE_TEXTURE2D_CUSTOM_B_START_SLOT, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	bindTexture(this->m_EmissiveTexture, ENGINE_TEXTURE2D_CUSTOM_C_START_SLOT, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK);
	bindTexture(this->m_AmbientOcclusionTexture, ENGINE_TEXTURE2D_CUSTOM_D_START_SLOT, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	bindTexture(this->m_RoughnessTexture, 9u, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	bindTexture(this->m_MetallicnessTexture, 10u, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	bindTexture(this->m_ReflectanceTexture, 11u, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
}
const void* CDefaultLitMaterial::GetConstantData()const
{
	return (this->m_RenderParams);
}
#ifdef _DEVELOPMENT_EDITOR
void CDefaultLitMaterial::HookSelectedTextureUpdate()
{

}
void CDefaultLitMaterial::HookApplyTextureUpdate()
{

}
void CDefaultLitMaterial::SelectedDefaultLitUpdate()
{
	std::map<INT32, std::string> baseEngineTextureItems = {
		{ -1, "None" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE, "Default White" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK, "Default Black" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GRAY, "Default Gray" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED, "Default Red" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GREEN, "Default Green" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLUE, "Default Blue" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP, "Default Bump" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_PROPERTY, "Default Property" },
		{ -2, "Custom Path" } };

	{
		auto textureCombo = [&baseEngineTextureItems](const std::string& name, INT32& select) {
			if (ImGui::BeginCombo(name.c_str(), baseEngineTextureItems[select].c_str()))
			{
				for (const auto& textureItem : baseEngineTextureItems)
				{
					BOOL32 selected = (select == textureItem.first);
					if (ImGui::Selectable(textureItem.second.c_str(), &selected))
					{
						select = textureItem.first;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}};
		auto texturePathText = [](const std::string& name, const INT32& select, CHAR* path) {
			if (select == -2)
			{
				ImGui::InputText(name.c_str(), path, 512, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
			}};
		textureCombo("BaseTexture", this->m_AlbedoSelect);
		texturePathText("BaseTexturePath", this->m_AlbedoSelect, this->m_AlbedoPath);
		textureCombo("EmissiveTexture", this->m_EmissiveSelect);
		texturePathText("EmissiveTexturePath", this->m_EmissiveSelect, this->m_EmissivePath);
		textureCombo("NormalTexture", this->m_NormalSelect);
		texturePathText("NormalTexturePath", this->m_NormalSelect, this->m_NormalPath);
		textureCombo("RoughnessTexture", this->m_RoughnessSelect);
		texturePathText("RoughnessTexturePath", this->m_RoughnessSelect, this->m_RoughnessPath);
		textureCombo("MetallicnessTexture", this->m_MetallicnessSelect);
		texturePathText("MetallicnessTexturePath", this->m_MetallicnessSelect, this->m_MetallicnessPath);
		textureCombo("AmbientOcclusionTexture", this->m_AmbientOcclusionSelect);
		texturePathText("AmbientOcclusionTexturePath", this->m_AmbientOcclusionSelect, this->m_AmbientOcclusionPath);
		textureCombo("ReflectanceTexture", this->m_ReflectanceSelect);
		texturePathText("ReflectanceTexturePath", this->m_ReflectanceSelect, this->m_ReflectancePath);
		this->HookSelectedTextureUpdate();
	}

	{
		auto textureUpdate = [&baseEngineTextureItems](const INT32& select, CHAR* path, CTexture2D*& tex, const BOOL32& sRGB = TRUE) {
			if (select != -2)
			{
				strcpy_s(path, 512, baseEngineTextureItems[select].c_str());
				if (select == -1)
				{
					tex = CRenderPipeline::GetDefaultTexture(CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED);
				}
				else
				{
					tex = CRenderPipeline::GetDefaultTexture(static_cast<CustomStruct::CEngineDefaultTexture2DType>(select));
				}
			}
			else
			{
				std::string tempName(path);
				tex = CTextureManager::LoadTexture2D(tempName, sRGB);
			}};
		if (ImGui::Button("ApplyTextures"))
		{
			textureUpdate(this->m_AlbedoSelect, this->m_AlbedoPath, this->m_AlbedoTexture);
			textureUpdate(this->m_EmissiveSelect, this->m_EmissivePath, this->m_EmissiveTexture);
			textureUpdate(this->m_NormalSelect, this->m_NormalPath, this->m_NormalTexture, FALSE);
			textureUpdate(this->m_RoughnessSelect, this->m_RoughnessPath, this->m_RoughnessTexture, FALSE);
			textureUpdate(this->m_MetallicnessSelect, this->m_MetallicnessPath, this->m_MetallicnessTexture, FALSE);
			textureUpdate(this->m_AmbientOcclusionSelect, this->m_AmbientOcclusionPath, this->m_AmbientOcclusionTexture, FALSE);
			textureUpdate(this->m_ReflectanceSelect, this->m_ReflectancePath, this->m_ReflectanceTexture, FALSE);
			this->HookApplyTextureUpdate();
		}
	}
}
void CDefaultLitMaterial::SelectedEditorUpdate()
{
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT baseColor[3]		= { renderParams->BaseColorRoughness.x, renderParams->BaseColorRoughness.y, renderParams->BaseColorRoughness.z };
	FLOAT EmissiveColor[3]	= { renderParams->EmissiveAmbientOcclusion.x, renderParams->EmissiveAmbientOcclusion.y, renderParams->EmissiveAmbientOcclusion.z };

	bool isGlossy			= this->m_IsGlossy;
	FLOAT glossy			= renderParams->MetallicnessReflectanceIsGlossy.z;
	FLOAT roughness			= renderParams->BaseColorRoughness.w;
	FLOAT ambientOcclusion	= renderParams->EmissiveAmbientOcclusion.w;
	FLOAT metallicness		= renderParams->MetallicnessReflectanceIsGlossy.x;
	FLOAT reflectance		= renderParams->MetallicnessReflectanceIsGlossy.y;

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(400, 270), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

	{
		ImGui::Text("BaseColor");
		ImGui::ColorEdit3("BaseColor", baseColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);

		ImGui::Text("EmissiveColor");
		ImGui::ColorEdit3("EmissiveColor", EmissiveColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);

		ImGui::Checkbox("IsGlossy", &isGlossy);
		ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);
		ImGui::SliderFloat("Metallicness", &metallicness, 0.f, 1.f);
		ImGui::SliderFloat("Reflectance", &reflectance, 0.f, 1.f);
		ImGui::SliderFloat("AmbientOcclusion", &ambientOcclusion, 0.f, 1.f);
	}
	this->SelectedDefaultLitUpdate();

	ImGui::EndChild();

	this->m_IsGlossy = isGlossy;
	glossy = isGlossy ? 1.f : 0.f;
	renderParams->BaseColorRoughness		= DirectX::XMFLOAT4(baseColor[0], baseColor[1], baseColor[2], roughness);
	renderParams->EmissiveAmbientOcclusion	= DirectX::XMFLOAT4(EmissiveColor[0], EmissiveColor[1], EmissiveColor[2], ambientOcclusion);
	renderParams->MetallicnessReflectanceIsGlossy = DirectX::XMFLOAT4(metallicness, reflectance, glossy, 0.f);
}
#endif



CDefaultLitSkeletonMeshMaterial::CDefaultLitSkeletonMeshMaterial() : CDefaultLitMaterial(typeid(CDefaultLitSkeletonMeshMaterial).name(), MaterialType::MATERIAL_TYPE_OPAQUE_FORWARD, sizeof(CDefaultLitMaterial::RenderParams), _GSkeletonMeshInputLayout, 6u, ENGINE_DEFAULT_LIT_SHADER_FORWARD_SKELETON_MESH_VS, ENGINE_DEFAULT_LIT_SHADER_FORWARD_PS)
{
	this->m_RenderParams = new CDefaultLitMaterial::RenderParams();
	CDefaultLitMaterial::RenderParams* renderParams = static_cast<CDefaultLitMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness				= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->EmissiveAmbientOcclusion			= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->MetallicnessReflectanceIsGlossy	= DirectX::XMFLOAT4(0.f, 1.f, 0.f, 0.f);
}
CDefaultLitSkeletonMeshMaterial::~CDefaultLitSkeletonMeshMaterial()
{
	if (this->m_RenderParams != nullptr)
	{
		delete (this->m_RenderParams);
		this->m_RenderParams = nullptr;
	}
}



CAnisotropicMaterial::CAnisotropicMaterial() : CDefaultLitMaterial(typeid(CAnisotropicMaterial).name(), MaterialType::MATERIAL_TYPE_OPAQUE_FORWARD, sizeof(CAnisotropicMaterial::RenderParams), _GDefaultMeshInputLayout, 4u, ENGINE_DEFAULT_ANISOTROPIC_SHADER_FORWARD_VS, ENGINE_DEFAULT_ANISOTROPIC_SHADER_FORWARD_PS)
{
#ifdef _DEVELOPMENT_EDITOR
	this->m_AnisotropyStrengthSelect	= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	this->m_AnisotropyDirectionSelect	= CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE;
	{
		auto initPathChar = [](CHAR* path, const std::string& str) {
			strcpy_s(path, 512, str.c_str()); };
		initPathChar(this->m_AnisotropyStrengthPath, "Default White");
		initPathChar(this->m_AnisotropyDirectionPath, "Default White");
	}
#endif
	this->m_AnisotropyStrengthTexture	= NULL;
	this->m_AnisotropyDirectionTexture	= NULL;

	if (this->m_RenderParams != nullptr)
	{
		delete (this->m_RenderParams);
		this->m_RenderParams = nullptr;
	}
	this->m_RenderParams = new CAnisotropicMaterial::RenderParams();
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness						= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->EmissiveAmbientOcclusion					= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	renderParams->MetallicnessReflectanceIsGlossy			= DirectX::XMFLOAT4(0.f, 1.f, 0.f, 0.f);
	renderParams->AnisotropyStrengthDirection				= DirectX::XMFLOAT4(0.5f, 1.f, 0.f, 0.f);
}
CAnisotropicMaterial::~CAnisotropicMaterial()
{
	if (this->m_RenderParams != nullptr)
	{
		delete (this->m_RenderParams);
		this->m_RenderParams = nullptr;
	}
}
void CAnisotropicMaterial::SetAnisotropyStrengthTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_AnisotropyStrengthTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_AnisotropyStrengthSelect = -2;
		strcpy_s(this->m_AnisotropyStrengthPath, 512, tex->GetName().c_str());
#endif
	}
}
void CAnisotropicMaterial::SetAnisotropyDirectionTexture(CTexture2D* tex)
{
	if (tex != NULL)
	{
		this->m_AnisotropyDirectionTexture = tex;
#ifdef _DEVELOPMENT_EDITOR
		this->m_AnisotropyDirectionSelect = -2;
		strcpy_s(this->m_AnisotropyDirectionPath, 512, tex->GetName().c_str());
#endif
	}
}
CTexture2D* CAnisotropicMaterial::GetAnisotropyStrengthTexture()const
{
	return (this->m_AnisotropyStrengthTexture);
}
CTexture2D* CAnisotropicMaterial::GetAnisotropyDirectionTexture()const
{
	return (this->m_AnisotropyDirectionTexture);
}
void CAnisotropicMaterial::SetAnisotropyStrength(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->AnisotropyStrengthDirection.x = v;
}
void CAnisotropicMaterial::SetAnisotropyDirection(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->AnisotropyStrengthDirection.y = v;
}
void CAnisotropicMaterial::SetIsGlossyRoughness(const BOOL32& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.z = v ? 1.f : 0.f;
}
void CAnisotropicMaterial::SetRoughness(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness.w = v;
}
void CAnisotropicMaterial::SetMetallicness(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.x = v;
}
void CAnisotropicMaterial::SetReflectance(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->MetallicnessReflectanceIsGlossy.y = v;
}
void CAnisotropicMaterial::SetAmbientOcclusion(const FLOAT& v)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->EmissiveAmbientOcclusion.w = v;
}
void CAnisotropicMaterial::SetBaseColor(const CustomStruct::CColor& clr)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->BaseColorRoughness.x = clr.r;
	renderParams->BaseColorRoughness.y = clr.g;
	renderParams->BaseColorRoughness.z = clr.b;
}
void CAnisotropicMaterial::SetEmissiveColor(const CustomStruct::CColor& clr)
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	renderParams->EmissiveAmbientOcclusion.x = clr.r;
	renderParams->EmissiveAmbientOcclusion.y = clr.g;
	renderParams->EmissiveAmbientOcclusion.z = clr.b;
}
FLOAT CAnisotropicMaterial::GetAnisotropyStrength()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->AnisotropyStrengthDirection.x;
	return v;
}
FLOAT CAnisotropicMaterial::GetAnisotropyDirection()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->AnisotropyStrengthDirection.y;
	return v;
}
BOOL32 CAnisotropicMaterial::GetIsGlossyRoughness()const
{
	return (this->m_IsGlossy);
}
FLOAT CAnisotropicMaterial::GetRoughness()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->BaseColorRoughness.w;
	return v;
}
FLOAT CAnisotropicMaterial::GetMetallicness()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->MetallicnessReflectanceIsGlossy.x;
	return v;
}
FLOAT CAnisotropicMaterial::GetReflectance()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->MetallicnessReflectanceIsGlossy.y;
	return v;
}
FLOAT CAnisotropicMaterial::GetAmbientOcclusion()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT v = renderParams->EmissiveAmbientOcclusion.w;
	return v;
}
CustomStruct::CColor CAnisotropicMaterial::GetBaseColor()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	return (CustomStruct::CColor(renderParams->BaseColorRoughness.x, renderParams->BaseColorRoughness.y, renderParams->BaseColorRoughness.z));
}
CustomStruct::CColor CAnisotropicMaterial::GetEmissiveColor()const
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	return (CustomStruct::CColor(renderParams->EmissiveAmbientOcclusion.x, renderParams->EmissiveAmbientOcclusion.y, renderParams->EmissiveAmbientOcclusion.z));
}
void CAnisotropicMaterial::Init()
{
	CDefaultLitMaterial::Init();
#ifdef _DEVELOPMENT_EDITOR
	{
		std::map<INT32, std::string> baseEngineTextureItems = {
			{ -1, "None" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE, "Default White" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK, "Default Black" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GRAY, "Default Gray" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED, "Default Red" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GREEN, "Default Green" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLUE, "Default Blue" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP, "Default Bump" },
			{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_PROPERTY, "Default Property" },
			{ -2, "Custom Path" } };
		auto pathSelect = [&baseEngineTextureItems](const INT32& select, CHAR* name, CTexture2D*& texture, const BOOL32& sRGB = TRUE) {
			if (select != -2)
			{
				strcpy_s(name, 512, baseEngineTextureItems[select].c_str());
				if (select != -1)
				{
					texture = CRenderPipeline::GetDefaultTexture(static_cast<CustomStruct::CEngineDefaultTexture2DType>(select));
				}
			}
			else
			{
				std::string tempName(name);
				texture = CTextureManager::LoadTexture2D(tempName, sRGB);
			}};
		pathSelect(this->m_AnisotropyStrengthSelect, this->m_AnisotropyStrengthPath, this->m_AnisotropyStrengthTexture, FALSE);
		pathSelect(this->m_AnisotropyDirectionSelect, this->m_AnisotropyDirectionPath, this->m_AnisotropyDirectionTexture, FALSE);
	}
#endif

	{
		auto nullTextureSelect = [](CTexture2D*& texture, CustomStruct::CEngineDefaultTexture2DType type) {
			if (texture == NULL)
			{
				texture = CRenderPipeline::GetDefaultTexture(type);
			}};
		nullTextureSelect(this->m_AnisotropyStrengthTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
		nullTextureSelect(this->m_AnisotropyDirectionTexture, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	}
}
void CAnisotropicMaterial::Bind()const
{
	CDefaultLitMaterial::Bind();
	auto bindTexture = [](const CTexture2D* tex, const UINT32& slot, CustomStruct::CEngineDefaultTexture2DType type) {
		if (tex != NULL)
		{
			CRenderDevice::BindPSShaderResourceView(tex->GetShaderResourceView(), slot);
		}
		else
		{
			CRenderDevice::BindPSShaderResourceView(CRenderPipeline::GetDefaultTexture(type)->GetShaderResourceView(), slot);
		}};
	bindTexture(this->m_AnisotropyStrengthTexture, 12u, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
	bindTexture(this->m_AnisotropyDirectionTexture, 13u, CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE);
}
#ifdef _DEVELOPMENT_EDITOR
void CAnisotropicMaterial::HookSelectedTextureUpdate()
{
	std::map<INT32, std::string> baseEngineTextureItems = {
		{ -1, "None" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE, "Default White" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK, "Default Black" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GRAY, "Default Gray" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED, "Default Red" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GREEN, "Default Green" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLUE, "Default Blue" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP, "Default Bump" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_PROPERTY, "Default Property" },
		{ -2, "Custom Path" } };
	auto textureCombo = [&baseEngineTextureItems](const std::string& name, INT32& select) {
		if (ImGui::BeginCombo(name.c_str(), baseEngineTextureItems[select].c_str()))
		{
			for (const auto& textureItem : baseEngineTextureItems)
			{
				BOOL32 selected = (select == textureItem.first);
				if (ImGui::Selectable(textureItem.second.c_str(), &selected))
				{
					select = textureItem.first;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}};
	auto texturePathText = [](const std::string& name, const INT32& select, CHAR* path) {
		if (select == -2)
		{
			ImGui::InputText(name.c_str(), path, 512, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
		}};
	textureCombo("AnisotropyTexture", this->m_AnisotropyStrengthSelect);
	texturePathText("AnisotropyTexturePath", this->m_AnisotropyStrengthSelect, this->m_AnisotropyStrengthPath);
	textureCombo("AnisotropyDirectionTexture", this->m_AnisotropyDirectionSelect);
	texturePathText("AnisotropyDirectionTexturePath", this->m_AnisotropyDirectionSelect, this->m_AnisotropyDirectionPath);
}
void CAnisotropicMaterial::HookApplyTextureUpdate()
{
	std::map<INT32, std::string> baseEngineTextureItems = {
		{ -1, "None" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_WHITE, "Default White" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLACK, "Default Black" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GRAY, "Default Gray" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED, "Default Red" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_GREEN, "Default Green" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BLUE, "Default Blue" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_BUMP, "Default Bump" },
		{ CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_PROPERTY, "Default Property" },
		{ -2, "Custom Path" } };
	auto textureUpdate = [&baseEngineTextureItems](const INT32& select, CHAR* path, CTexture2D*& tex, const BOOL32& sRGB = TRUE) {
		if (select != -2)
		{
			strcpy_s(path, 512, baseEngineTextureItems[select].c_str());
			if (select == -1)
			{
				tex = CRenderPipeline::GetDefaultTexture(CustomStruct::CEngineDefaultTexture2DType::ENGINE_DEFAULT_TEXTURE2D_TYPE_RED);
			}
			else
			{
				tex = CRenderPipeline::GetDefaultTexture(static_cast<CustomStruct::CEngineDefaultTexture2DType>(select));
			}
		}
		else
		{
			std::string tempName(path);
			tex = CTextureManager::LoadTexture2D(tempName, sRGB);
		}};
	textureUpdate(this->m_AnisotropyStrengthSelect, this->m_AnisotropyStrengthPath, this->m_AnisotropyStrengthTexture, FALSE);
	textureUpdate(this->m_AnisotropyDirectionSelect, this->m_AnisotropyDirectionPath, this->m_AnisotropyDirectionTexture, FALSE);
}
void CAnisotropicMaterial::SelectedEditorUpdate()
{
	CAnisotropicMaterial::RenderParams* renderParams = static_cast<CAnisotropicMaterial::RenderParams*>(this->m_RenderParams);
	FLOAT baseColor[3]		= { renderParams->BaseColorRoughness.x, renderParams->BaseColorRoughness.y, renderParams->BaseColorRoughness.z };
	FLOAT EmissiveColor[3]	= { renderParams->EmissiveAmbientOcclusion.x, renderParams->EmissiveAmbientOcclusion.y, renderParams->EmissiveAmbientOcclusion.z };

	bool isGlossy				= this->m_IsGlossy;
	FLOAT glossy				= renderParams->MetallicnessReflectanceIsGlossy.z;
	FLOAT roughness				= renderParams->BaseColorRoughness.w;
	FLOAT ambientOcclusion		= renderParams->EmissiveAmbientOcclusion.w;
	FLOAT metallicness			= renderParams->MetallicnessReflectanceIsGlossy.x;
	FLOAT reflectance			= renderParams->MetallicnessReflectanceIsGlossy.y;
	FLOAT anisotropyStrength	= renderParams->AnisotropyStrengthDirection.x;
	FLOAT anisotropyDirection	= renderParams->AnisotropyStrengthDirection.y;

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(400, 270), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

	{
		ImGui::Text("BaseColor");
		ImGui::ColorEdit3("BaseColor", baseColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);

		ImGui::Text("EmissiveColor");
		ImGui::ColorEdit3("EmissiveColor", EmissiveColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);

		ImGui::Checkbox("IsGlossy", &isGlossy);
		ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);
		ImGui::SliderFloat("Metallicness", &metallicness, 0.f, 1.f);
		ImGui::SliderFloat("Reflectance", &reflectance, 0.f, 1.f);
		ImGui::SliderFloat("AmbientOcclusion", &ambientOcclusion, 0.f, 1.f);
		ImGui::SliderFloat("AnisotropyStrength", &anisotropyStrength, 0.f, 1.f);
		ImGui::SliderFloat("AnisotropyDirection", &anisotropyDirection, 0.f, 1.f);
	}
	this->SelectedDefaultLitUpdate();

	ImGui::EndChild();

	this->m_IsGlossy = isGlossy;
	glossy = isGlossy ? 1.f : 0.f;
	renderParams->BaseColorRoughness						= DirectX::XMFLOAT4(baseColor[0], baseColor[1], baseColor[2], roughness);
	renderParams->EmissiveAmbientOcclusion					= DirectX::XMFLOAT4(EmissiveColor[0], EmissiveColor[1], EmissiveColor[2], ambientOcclusion);
	renderParams->MetallicnessReflectanceIsGlossy			= DirectX::XMFLOAT4(metallicness, reflectance, glossy, 0.f);
	renderParams->AnisotropyStrengthDirection				= DirectX::XMFLOAT4(anisotropyStrength, anisotropyDirection, 0.f, 0.f);
}
#endif
*/