#pragma once

#include "../../../../../Entry/EngineMain.h"
#include "../../RenderBase/Headers/CRenderStructCommon.h"
#include "../../../EngineGame/Headers/CComponent.h"

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
	const void*		GetVertexData() const { return (this->m_VertexData); }
	UINT			GetVertexStride()const { return (this->m_VertexStride); }
	UINT			GetVertexCount()const { return (this->m_VertexCount); }
	UINT			GetIndexCount()const { return (this->m_IndexCount); }
	BOOL			HasVertexData()const { return (this->m_VertexData != nullptr); }
public:
	CBaseMesh(const std::string& name, const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT& inputLayoutNum, void* vdata, const UINT& vNum, const std::vector<IndexType>& idata, const std::vector<CustomStruct::CSubMeshInfo>& submeshInfo, Microsoft::WRL::ComPtr<ID3D11Buffer> vbuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> ibuffer, const CustomType::Vector3& boundMin, const CustomType::Vector3& boundMax)
	{
		this->m_MeshPath		= name;
		this->m_SubMeshInfo		= submeshInfo;
		this->m_VertexData		= vdata;
		this->m_IndexData		= idata;
		this->m_VertexBuffer	= vbuffer;
		this->m_IndexBuffer		= ibuffer;
		this->m_VertexCount		= vNum;
		this->m_IndexCount		= static_cast<UINT>(idata.size());
		this->m_VertexStride	= 0u;
		this->m_InputLayoutDesc.resize(inputLayoutNum);
		for (UINT i = 0u; i < inputLayoutNum; i++)
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
	UINT												m_VertexStride;
	UINT												m_VertexCount;
	UINT												m_IndexCount;
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
		return NULL;
	}
	UINT GetVertexStride()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexStride());
		}
		return 0u;
	}
	UINT GetVertexCount()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetVertexCount());
		}
		return 0u;
	}
	UINT GetIndexCount()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->GetIndexCount());
		}
		return 0u;
	}
	BOOL HasVertexData()const
	{
		if (this->HasMesh())
		{
			return (this->m_Mesh->HasVertexData());
		}
		return FALSE;
	}
public:
	void SetMesh(const CBaseMesh<UINT>* mesh)
	{
		if (mesh != NULL)
		{
			this->m_Mesh = mesh;
		}
	}
	BOOL HasMesh()const
	{
		return (this->m_Mesh != NULL);
	}
	virtual void Init()override {}
	virtual void Uninit()override {}
protected:
	const CBaseMesh<UINT>* m_Mesh;
public:
	CMeshComponent() : CBaseComponent(TRUE, FALSE, FALSE)
	{
		m_Mesh = NULL;
	}
	virtual ~CMeshComponent() {}
};