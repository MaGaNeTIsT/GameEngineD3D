/*
#pragma once

#include "../../../../Entry/EngineMain.h"
#include "../../EngineRender/RenderBase/Headers/CRenderStructCommon.h"
#include "../../EngineRender/RenderBase/Headers/CRenderDevice.h"
#include "./CComponent.h"
#include "./CSkeletonComponent.h"

template<typename IndexType>
class CBaseMesh
{
public:
	void GetMinMaxBounding(CustomType::Vector3& boundMin, CustomType::Vector3& boundMax)const
	{
		boundMin = this->m_BoundMin;
		boundMax = this->m_BoundMax;
	}
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer>						GetVertexBuffer()const { return (this->m_VertexBuffer); }
	Microsoft::WRL::ComPtr<ID3D11Buffer>						GetIndexBuffer()const { return (this->m_IndexBuffer); }
	const std::vector<CustomStruct::CRenderInputLayoutDesc>&	GetInputLayoutDesc()const { return (this->m_InputLayoutDesc); }
	const std::vector<CustomStruct::CSubMeshInfo>&				GetSubMeshInfo() const { return (this->m_SubMeshInfo); }
	const std::string&											GetName()const { return (this->m_MeshPath); }
	const void*		GetVertexData() const { return (this->m_VertexData); }
	UINT32			GetVertexStride()const { return (this->m_VertexStride); }
	UINT32			GetVertexCount()const { return (this->m_VertexCount); }
	UINT32			GetIndexCount()const { return (this->m_IndexCount); }
	BOOL32			HasVertexData()const { return (this->m_VertexData != nullptr); }
public:
	CBaseMesh(const std::string& name, const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT32& inputLayoutNum, void* vdata, const UINT32& vNum, const std::vector<IndexType>& idata, const std::vector<CustomStruct::CSubMeshInfo>& submeshInfo, Microsoft::WRL::ComPtr<ID3D11Buffer> vbuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> ibuffer, const CustomType::Vector3& boundMin, const CustomType::Vector3& boundMax)
	{
		this->m_MeshPath		= name;
		this->m_SubMeshInfo		= submeshInfo;
		this->m_VertexData		= vdata;
		this->m_IndexData		= idata;
		this->m_VertexBuffer	= vbuffer;
		this->m_IndexBuffer		= ibuffer;
		this->m_VertexCount		= vNum;
		this->m_IndexCount		= static_cast<UINT32>(idata.size());
		this->m_VertexStride	= 0u;
		this->m_InputLayoutDesc.resize(inputLayoutNum);
		for (UINT32 i = 0u; i < inputLayoutNum; i++)
		{
			this->m_InputLayoutDesc[i] = inputLayoutDesc[i];
			this->m_VertexStride += inputLayoutDesc[i].GetSemanticSizeByByte();
		}
		this->m_BoundMin = boundMin;
		this->m_BoundMax = boundMax;
	}
	virtual ~CBaseMesh()
	{
		if (this->m_VertexData != nullptr)
		{
			delete[](this->m_VertexData);
			this->m_VertexData = nullptr;
		}
	}
private:
	std::string											m_MeshPath;
	std::vector<CustomStruct::CRenderInputLayoutDesc>	m_InputLayoutDesc;
	std::vector<CustomStruct::CSubMeshInfo>				m_SubMeshInfo;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_IndexBuffer;
	const void*											m_VertexData;
	std::vector<IndexType>								m_IndexData;
	UINT32												m_VertexStride;
	UINT32												m_VertexCount;
	UINT32												m_IndexCount;
	CustomType::Vector3									m_BoundMin;
	CustomType::Vector3									m_BoundMax;
};

class CMeshComponent : public CBaseComponent
{
public:
	void GetMinMaxBounding(CustomType::Vector3& boundMin, CustomType::Vector3& boundMax)const
	{
		if (this->HasMesh())
		{
			this->m_Mesh->GetMinMaxBounding(boundMin, boundMax);
		}
		else
		{
			boundMin = 0.f;
			boundMax = 0.f;
		}
	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexBuffer());
		}
		return nullptr;
	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetIndexBuffer());
		}
		return nullptr;
	}
	std::vector<CustomStruct::CRenderInputLayoutDesc> GetInputLayoutDesc()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetInputLayoutDesc());
		}
		std::vector<CustomStruct::CRenderInputLayoutDesc> layout;
		return layout;
	}
	std::vector<CustomStruct::CSubMeshInfo> GetSubMeshInfo()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetSubMeshInfo());
		}
		std::vector<CustomStruct::CSubMeshInfo> subMesh;
		return subMesh;
	}
	const void* GetVertexData()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexData());
		}
		return nullptr;
	}
	UINT32 GetVertexStride()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexStride());
		}
		return 0u;
	}
	UINT32 GetVertexCount()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexCount());
		}
		return 0u;
	}
	UINT32 GetIndexCount()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetIndexCount());
		}
		return 0u;
	}
	BOOL32 HasVertexData()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->HasVertexData());
		}
		return FALSE;
	}
public:
	void SetMesh(const CBaseMesh<UINT32>* mesh)
	{
		if (mesh != nullptr)
		{
			this->m_Mesh = mesh;
		}
	}
	BOOL32 HasMesh()const
	{
		return (this->m_Mesh != nullptr);
	}
	virtual void Init()override {}
	virtual void Uninit()override {}
	virtual void Bind(const BOOL32& needVertex, const BOOL32& needIndex)const
	{
		if (needVertex == TRUE)
		{
			CRenderDevice::SetVertexBuffer(this->m_Mesh->GetVertexBuffer(), this->m_Mesh->GetVertexStride());
		}
		if (needIndex == TRUE)
		{
			CRenderDevice::SetIndexBuffer(this->m_Mesh->GetIndexBuffer());
		}
		this->BindExtra();
	}
protected:
	virtual void BindExtra()const {}
#ifdef _DEVELOPMENT_EDITOR
public:
	virtual void SelectedEditorUpdate()override
	{
		if (ImGui::TreeNode("MeshComponent"))
		{
			std::string meshComponentUniqueID = std::to_string((this->GetUniqueID()));
			ImGui::Text("MeshComponent uniqueID : %s", meshComponentUniqueID.c_str());
			std::string meshName = this->m_Mesh == nullptr ? "NULL" : this->m_Mesh->GetName();
			ImGui::Text("Mesh name : %s", meshName.c_str());
			std::vector<CustomStruct::CSubMeshInfo> submesh = this->GetSubMeshInfo();
			ImGui::Text("Has submesh : %s", (submesh.size() > 1) ? "true" : "false");
			if (submesh.size() > 1)
			{
				if (ImGui::TreeNode("SubMeshInfo"))
				{
					ImGui::Text("Submesh number : %d", submesh.size());
					for (size_t i = 0; i < submesh.size(); i++)
					{
						ImGui::Text("Submesh index : %d\nVertex start : %d, Vertex count : %d\nIndex start : %d, Index count : %d", i, submesh[i].VertexStart, submesh[i].VertexCount, submesh[i].IndexStart, submesh[i].IndexCount);
					}
					ImGui::TreePop();
				}
			}
			std::vector<CustomStruct::CRenderInputLayoutDesc> layoutDesc = this->GetInputLayoutDesc();
			if (layoutDesc.size() > 0)
			{
				static std::map<CustomStruct::CRenderShaderSemantic, std::string> layoutSemanticMap = {
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION, "POSITION" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD, "TEXCOORD" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_NORMAL, "NORMAL" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TANGENT, "TANGENT" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_COLOR, "COLOR" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDINDICES, "BLENDINDICES" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDWEIGHT, "BLENDWEIGHT" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITIONT, "POSITIONT" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_PSIZE, "PSIZE" },
					{ CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BINORMAL, "BINORMAL" } };
				if (ImGui::TreeNode("InputLayoutInfo"))
				{
					ImGui::Text("Input layout number : %d", layoutDesc.size());
					for (size_t i = 0; i < layoutDesc.size(); i++)
					{
						ImGui::Text("Layout index : %d, %s", i, layoutSemanticMap[layoutDesc[i].SemanticName].c_str());
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
#endif
protected:
	const CBaseMesh<UINT32>* m_Mesh;
public:
	CMeshComponent() : CBaseComponent(TRUE, FALSE, FALSE)
	{
		m_Mesh = nullptr;
	}
	CMeshComponent(const BOOL32& active, const BOOL32& needUpdate, const BOOL32& needFixedUpdate) : CBaseComponent(active, needUpdate, needFixedUpdate)
	{
		m_Mesh = nullptr;
	}
	virtual ~CMeshComponent() {}
};

class CSkeletonMeshComponent : public CMeshComponent
{
public:
	BOOL32	HasSkeleton()const { return (this->m_SkeletonComponent != nullptr); }
public:
	void						SetSkeletonComponent(const CSkeletonComponent* skeletonComponent) { this->m_SkeletonComponent = skeletonComponent; };
	const CSkeletonComponent*	GetSkeletonComponent() { return (this->m_SkeletonComponent); };
protected:
	const CSkeletonComponent*	m_SkeletonComponent;
protected:
	virtual void BindExtra()const override
	{
		m_SkeletonComponent->UploadSkeletonGPUConstantBuffer();
		m_SkeletonComponent->BindVSSkeletonGPUConstantBuffer(ENGINE_CBUFFER_VS_SKELETON_DATA_START_SLOT);
	}
public:
	CSkeletonMeshComponent() : CMeshComponent(TRUE, FALSE, FALSE)
	{
		this->m_SkeletonComponent	= nullptr;
	}
	virtual ~CSkeletonMeshComponent() {}
};
*/