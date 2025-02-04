#pragma once
#include "IImporter.h"
#include <imgui.h>
#if _EDITOR_ONLY
namespace PigeonEngine
{
	class ETextureImporter : public IImporter
	{
	public:
		virtual ~ETextureImporter() {};
		virtual void CreateImportEditor(TArray<EString> Paths) override;
		virtual EString GetFileFilterList() { return "PNG;TGA;JPG;JEPG"; }
	protected:
		virtual void UpdateImportEditor() override;
	private:
		TArray<EString> m_Paths;
		TArray<INT32> m_CubeMapIndex;
		BOOL8 InitializeEditor = FALSE;
	};
};
#endif