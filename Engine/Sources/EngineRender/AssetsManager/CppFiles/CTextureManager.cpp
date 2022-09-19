#include "../../../../../Entry/EngineMain.h"
#include "../Headers/CTextureManager.h"
#include "../Headers/CTexture2D.h"
#include "../../RenderBase/Headers/CStructCommon.h"
#include "../../RenderBase/Headers/CRenderDevice.h"

CTextureManager* CTextureManager::m_TextureManager = new CTextureManager();
void CTextureManager::ShutDown()
{
	delete m_TextureManager;
}
void CTextureManager::ClearTexture2DData()
{
	if (m_TextureManager->m_Texture2DData.size() > 0)
	{
		for (const auto& data : (m_TextureManager->m_Texture2DData))
		{
			if ((data.second) != NULL)
			{
				delete (data.second);
			}
		}
		m_TextureManager->m_Texture2DData.clear();
	}
}
CTexture2D* CTextureManager::LoadTexture2D(const std::string& name, const BOOL& isSRGB)
{
	const static std::string _tgaName = "tga";

	CTexture2D* resultTexture = CTextureManager::FindTexture2DData(name);
	if (resultTexture != NULL)
		return resultTexture;
	{
		size_t pos = name.find_last_of('.');
		if (pos == std::string::npos)
			return NULL;
		std::string typeName = name.substr(pos + 1, name.length());

		if (typeName == _tgaName)
			resultTexture = (CTextureManager::LoadTGATexture2D(name, isSRGB));
		else
			return NULL;

		if (resultTexture == NULL)
			return NULL;
		CTextureManager::AddTexture2DData(name, resultTexture);
	}
	return resultTexture;
}
CTexture2D* CTextureManager::LoadTGATexture2D(const std::string& name, const BOOL& isSRGB)
{
	TBYTE	header[18];
	TBYTE*	rawData;
	TBYTE*	image;
	TBYTE	depth;
	UINT	width, height, bpp, size;
	FILE*	file;
	fopen_s(&file, name.c_str(), "rb");
	if (file == NULL)
	{
		return NULL;
	}
	fread_s(header, sizeof(header), sizeof(header), 1u, file);
	width	= header[13] * 256 + header[12];
	height	= header[15] * 256 + header[14];
	depth	= header[16];
	if (depth == 32)
	{
		bpp = 4u;
	}
	else if (depth == 24)
	{
		bpp = 3u;
	}
	else
	{
		bpp = 0u;
	}
	if (!(bpp == 4u || bpp == 3u))
	{
		return NULL;
	}
	size	= width * height * bpp;
	rawData	= new TBYTE[size];
	image = rawData;
	fread_s(rawData, size, size, 1u, file);
	if (bpp == 3u)
	{
		image = new TBYTE[width * height * 4u];
		// Copy to new buffer
		// R<->B
		for (UINT y = 0u; y < height; y++)
		{
			for (UINT x = 0u; x < width; x++)
			{
				image[(y * width + x) * 4u + 0u] = rawData[(y * width + x) * 3u + 2u];
				image[(y * width + x) * 4u + 1u] = rawData[(y * width + x) * 3u + 1u];
				image[(y * width + x) * 4u + 2u] = rawData[(y * width + x) * 3u + 0u];
				image[(y * width + x) * 4u + 3u] = 0xff;
			}
		}
	}
	else if (bpp == 4u)
	{
		// R<->B
		for (UINT y = 0u; y < height; y++)
		{
			for (UINT x = 0u; x < width; x++)
			{
				TBYTE c = rawData[(y * width + x) * 4u + 0u];
				rawData[(y * width + x) * 4u + 0u] = rawData[(y * width + x) * 4u + 2u];
				rawData[(y * width + x) * 4u + 2u] = c;
			}
		}
	}
	fclose(file);
	CRenderDevice::Texture2DViewInfo tempTexture2D;
	{
		CustomStruct::CRenderFormat sourceFormat = CustomStruct::CRenderFormat::FORMAT_R8G8B8A8_UNORM;
		if (isSRGB)
		{
			CustomStruct::CRenderFormat sourceFormat = CustomStruct::CRenderFormat::FORMAT_R8G8B8A8_UNORM_SRGB;
		}
		if (CRenderDevice::CreateTexture2D(tempTexture2D, CustomStruct::CRenderTextureDesc(width, height, CustomStruct::CRenderBindFlag::BIND_SHADER_RESOURCE, sourceFormat, &sourceFormat), &CustomStruct::CRenderSubresourceData(static_cast<const void*>(image), width * 4u, width * height * 4u)) == FALSE)
		{
			if (bpp == 3u)
			{
				delete[]image;
			}
			delete[]rawData;
			return NULL;
		}
	}
	if (bpp == 3u)
	{
		delete[]image;
	}
	delete[]rawData;
	CTexture2D* resultTexture2D = new CTexture2D(name, tempTexture2D);
	return resultTexture2D;
}
void CTextureManager::AddTexture2DData(const std::string& name, CTexture2D* ptrData)
{
	m_TextureManager->m_Texture2DData[name] = ptrData;
}
CTexture2D* CTextureManager::FindTexture2DData(const std::string& name)
{
	std::map<std::string, CTexture2D*>::iterator element = m_TextureManager->m_Texture2DData.find(name);
	if (element == m_TextureManager->m_Texture2DData.end())
		return NULL;
	return (element->second);
}