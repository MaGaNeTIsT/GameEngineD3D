/*
#include "../Headers/CComponent.h"
#include "../Headers/CGameObject.h"

CTransform::CTransform()
{
	this->m_GameObject		= nullptr;
	this->m_Parent			= nullptr;
	this->m_LocalPosition	= CustomType::Vector3::Zero();
	this->m_LocalRotation	= CustomType::Quaternion::Identity();
	this->m_LocalScale		= 1.f;
#ifdef _DEVELOPMENT_EDITOR
	this->m_RealTimeChangeValue = TRUE;
	this->m_EditorRotation[0] = 0.f; this->m_EditorRotation[1] = 0.f; this->m_EditorRotation[2] = 0.f;
#endif
}
void CTransform::InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Quaternion& worldRotation, const CustomType::Vector3& worldScale)
{
	this->SetWorldPosition(worldPosition);
	this->SetWorldRotation(worldRotation);
	this->SetWorldScale(worldScale);
}
void CTransform::InitTransform(const CustomType::Vector3& worldPosition)
{
	this->InitTransform(worldPosition, CustomType::Quaternion::Identity(), CustomType::Vector3::One());
}
void CTransform::InitTransform(const CustomType::Quaternion& worldRotation)
{
	this->InitTransform(CustomType::Vector3::Zero(), worldRotation, CustomType::Vector3::One());
}
void CTransform::InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Quaternion& worldRotation)
{
	this->InitTransform(worldPosition, worldRotation, CustomType::Vector3::One());
}
void CTransform::InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Vector3& worldScale)
{
	this->InitTransform(worldPosition, CustomType::Quaternion::Identity(), CustomType::Vector3::One());
}
CGameObjectTransformBase* CTransform::GetGameObject()const
{
	return (this->m_GameObject);
}
CTransform* CTransform::GetParent()const
{
	return (this->m_Parent);
}
UINT32 CTransform::GetChildrenNum()const
{
	return (static_cast<UINT32>(this->m_Children.size()));
}
CTransform* CTransform::GetChildByUniqueID(const ULONGLONG& id)const
{
	if (this->HasChild())
	{
		auto& element = this->m_Children.find(id);
		if (element == this->m_Children.end())
		{
			return nullptr;
		}
		return (element->second);
	}
	return nullptr;
}
std::vector<CTransform*> CTransform::GetChildrenList()const
{
	std::vector<CTransform*> childrenList;
	if (this->HasChild())
	{
		for (const auto& child : this->m_Children)
		{
			if (child.second != nullptr)
			{
				childrenList.push_back(child.second);
			}
		}
	}
	return childrenList;
}
std::map<ULONGLONG, CTransform*> CTransform::GetChildrenMap()const
{
	return (this->m_Children);
}
void CTransform::SetParent(CTransform* parent)
{
	if (parent != nullptr)
	{
		if (this->HasParent())
		{
			this->DisconnectParentAndChild(this->m_Parent, this);
		}
		this->ConnectParentAndChild(parent, this);
	}
}
void CTransform::AddChild(CTransform* child)
{
	if (child != nullptr)
	{
		if (child->HasParent())
		{
			this->DisconnectParentAndChild(child->m_Parent, child);
		}
		this->ConnectParentAndChild(this, child);
	}
}
void CTransform::RemoveParent()
{
	if (this->HasParent())
	{
		this->DisconnectParentAndChild(this->m_Parent, this);
	}
}
void CTransform::RemoveChild(CTransform* child)
{
	if (child != nullptr && this->HasChild())
	{
		this->DisconnectParentAndChild(this, child);
	}
}
void CTransform::RemoveChildByUniqueID(const ULONGLONG& id)
{
	if (this->HasChild())
	{
		CTransform* child = nullptr;
		if (this->BaseModifyChildByUniqueID(id, child))
		{
			this->DisconnectParentAndChild(this, child);
		}
	}
}
void CTransform::RemoveAllChildren()
{
	if (this->HasChild())
	{
		for (auto itChild = this->m_Children.begin(); itChild != this->m_Children.end(); itChild++)
		{
			if (itChild->second != nullptr)
			{
				this->DisconnectParentAndChild(this, itChild->second);
			}
		}
		this->m_Children.clear();
	}
}
BOOL32 CTransform::IsBelongGameObject(const ULONGLONG& gameObjectID)const
{
	if (this->HasGameObject())
	{
		return (this->m_GameObject->GetUniqueID() == gameObjectID);
	}
	return FALSE;
}
BOOL32 CTransform::HasGameObject()const
{
	return (this->m_GameObject != nullptr);
}
BOOL32 CTransform::HasParent()const
{
	return (this->m_Parent != nullptr);
}
BOOL32 CTransform::HasChild()const
{
	return (this->m_Children.size() > 0);
}
BOOL32 CTransform::IsParent(const ULONGLONG& parentID)const
{
	if (this->HasParent())
	{
		return (this->m_Parent->GetUniqueID() == parentID);
	}
	return FALSE;
}
BOOL32 CTransform::IsChild(const ULONGLONG& childID)const
{
	if (this->HasChild())
	{
		auto& element = this->m_Children.find(childID);
		if (element == this->m_Children.end())
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
void CTransform::BaseAddChild(CTransform* child)
{
	if (child != nullptr)
	{
		if (!this->BaseFindChildByUniqueID(child->GetUniqueID()))
		{
			this->m_Children.insert_or_assign(child->GetUniqueID(), child);
		}
	}
}
void CTransform::BaseRemoveChildByUniqueID(const ULONGLONG& id)
{
	if (this->HasChild())
	{
		if (this->BaseFindChildByUniqueID(id))
		{
			this->m_Children.erase(id);
		}
	}
}
BOOL32 CTransform::BaseFindChildByUniqueID(const ULONGLONG& id)
{
	if (this->HasChild())
	{
		auto& element = this->m_Children.find(id);
		if (element == this->m_Children.end())
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL32 CTransform::BaseModifyChildByUniqueID(const ULONGLONG& id, CTransform*& output)
{
	if (this->HasChild())
	{
		auto& element = this->m_Children.find(id);
		if (element == this->m_Children.end())
		{
			return FALSE;
		}
		output = element->second;
		return TRUE;
	}
	return FALSE;
}
void CTransform::ConnectParentAndChild(CTransform* parent, CTransform* child)
{
	if (parent == nullptr || child == nullptr)
	{
		return;
	}
	if (child->HasParent())
	{
		CTransform* oldParent = child->m_Parent;
		if ((parent->GetUniqueID() == oldParent->GetUniqueID()) && parent->BaseFindChildByUniqueID(child->GetUniqueID()))
		{
			return;
		}
		child->CalculateCurrentLocalTransform(nullptr);
		oldParent->BaseRemoveChildByUniqueID(child->GetUniqueID());
		child->m_Parent = nullptr;
	}
	child->CalculateCurrentLocalTransform(parent);
	child->m_Parent = parent;
	parent->BaseAddChild(child);
}
void CTransform::DisconnectParentAndChild(CTransform* parent, CTransform* child)
{
	if (parent == nullptr || child == nullptr)
	{
		return;
	}
	if (child->HasParent())
	{
		if (parent->GetUniqueID() == child->m_Parent->GetUniqueID())
		{
			child->CalculateCurrentLocalTransform(nullptr);
			child->m_Parent = nullptr;
		}
	}
	parent->BaseRemoveChildByUniqueID(child->GetUniqueID());
}
void CTransform::CalculateCurrentLocalTransform(CTransform* newParent)
{
	if (newParent != nullptr)
	{
		this->m_LocalPosition = this->GetWorldPosition() - newParent->GetWorldPosition();
		this->m_LocalRotation = CustomType::Quaternion::MultiplyQuaternion(newParent->GetWorldRotation().Inverse(), this->GetWorldRotation());
		this->m_LocalScale = CustomType::Vector3::Reciprocal(newParent->GetWorldScale()) * this->GetWorldScale();
	}
	else
	{
		this->m_LocalPosition = this->GetWorldPosition();
		this->m_LocalRotation = this->GetWorldRotation();
		this->m_LocalScale = this->GetWorldScale();
	}
}
const CustomType::Vector3& CTransform::GetLocalPosition()const
{
	return (this->m_LocalPosition);
}
const CustomType::Quaternion& CTransform::GetLocalRotation()const
{
	return (this->m_LocalRotation);
}
const CustomType::Vector3& CTransform::GetLocalScale()const
{
	return (this->m_LocalScale);
}
void CTransform::SetLocalPosition(const CustomType::Vector3& localPosition)
{
	this->m_LocalPosition = localPosition;
}
void CTransform::SetLocalRotation(const CustomType::Quaternion& localRotation)
{
	this->m_LocalRotation = localRotation;
}
void CTransform::SetLocalScale(const CustomType::Vector3& localScale)
{
	this->m_LocalScale = localScale;
}
void CTransform::SetWorldPosition(const CustomType::Vector3& worldPosition)
{
	if (this->HasParent())
	{
		CustomType::Vector3 parentWorldPosition(-(this->m_Parent->GetWorldPosition()));
		this->m_LocalPosition = parentWorldPosition + worldPosition;
	}
	else
	{
		this->m_LocalPosition = worldPosition;
	}
}
void CTransform::SetWorldRotation(const CustomType::Quaternion& worldRotation)
{
	if (this->HasParent())
	{
		this->m_LocalRotation = CustomType::Quaternion::MultiplyQuaternion(this->m_Parent->GetWorldRotation().Inverse(), worldRotation);
	}
	else
	{
		this->m_LocalRotation = worldRotation;
	}
}
void CTransform::SetWorldScale(const CustomType::Vector3& worldScale)
{
	if (this->HasParent())
	{
		this->m_LocalScale = CustomType::Vector3::Reciprocal(this->m_Parent->GetWorldScale()) * worldScale;
	}
	else
	{
		this->m_LocalScale = worldScale;
	}
}
CustomType::Vector3 CTransform::GetWorldPosition()const
{
	CustomType::Vector3 result(this->m_LocalPosition);
	this->RecursionWorldPosition(this->m_Parent, result);
	return result;
}
CustomType::Quaternion CTransform::GetWorldRotation()const
{
	CustomType::Quaternion result(this->m_LocalRotation);
	this->RecursionWorldRotation(this->m_Parent, result);
	return result;
}
CustomType::Vector3 CTransform::GetWorldScale()const
{
	CustomType::Vector3 result(this->m_LocalScale);
	this->RecursionWorldScale(this->m_Parent, result);
	return result;
}
void CTransform::RecursionWorldPosition(const CTransform* parent, CustomType::Vector3& position)const
{
	if (parent == nullptr)
	{
		return;
	}
	position += parent->m_LocalPosition;
	this->RecursionWorldPosition(parent->m_Parent, position);
}
void CTransform::RecursionWorldRotation(const CTransform* parent, CustomType::Quaternion& rotation)const
{
	if (parent == nullptr)
	{
		return;
	}
	rotation = CustomType::Quaternion::MultiplyQuaternion(parent->m_LocalRotation, rotation);
	this->RecursionWorldRotation(parent->m_Parent, rotation);
}
void CTransform::RecursionWorldScale(const CTransform* parent, CustomType::Vector3& scale)const
{
	if (parent == nullptr)
	{
		return;
	}
	scale *= parent->m_LocalScale;
	this->RecursionWorldScale(parent->m_Parent, scale);
}
CustomType::Vector3 CTransform::GetForwardVector()const
{
	return (this->GetWorldRotation().MultiplyVector(CustomType::Vector3::ZVector()));
}
CustomType::Vector3 CTransform::GetUpVector()const
{
	return (this->GetWorldRotation().MultiplyVector(CustomType::Vector3::YVector()));
}
CustomType::Vector3 CTransform::GetRightVector()const
{
	return (this->GetWorldRotation().MultiplyVector(CustomType::Vector3::XVector()));
}
CustomType::Matrix4x4 CTransform::GetLocalToWorldMatrix()const
{
	return (CustomType::Matrix4x4(this->GetWorldPosition(), this->GetWorldRotation(), this->GetWorldScale()));
}
CustomType::Matrix4x4 CTransform::GetWorldToLocalMatrix()const
{
	return (this->GetLocalToWorldMatrix().Inverse());
}
#ifdef _DEVELOPMENT_EDITOR
void CTransform::SetEditorRotation(const CustomType::Vector3& v)
{
	this->m_EditorRotation[0] = v.X();
	this->m_EditorRotation[1] = v.Y();
	this->m_EditorRotation[2] = v.Z();
}
#endif
#ifdef _DEVELOPMENT_EDITOR
void CTransform::SelectedEditorUpdate()
{
	bool realTimeChange = this->m_RealTimeChangeValue;
	BOOL32 needChangeTransform = FALSE;
	FLOAT localPosition[3], localScale[3];
	{
		CustomType::Vector3 localPositionVector(this->GetLocalPosition());
		CustomType::Vector3 localScaleVector(this->GetLocalScale());
		localPosition[0] = localPositionVector.X(); localPosition[1] = localPositionVector.Y(); localPosition[2] = localPositionVector.Z();
		localScale[0] = localScaleVector.X(); localScale[1] = localScaleVector.Y(); localScale[2] = localScaleVector.Z();
	}

	{
		ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
		if (ImGui::TreeNode("Transform"))
		{
			ImGui::Checkbox("IsRealTimeChange", &realTimeChange);
			ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(400, 135), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
			ImGui::InputFloat3("LocalPosition", localPosition, "%.2f");
			ImGui::DragFloat3("LocalRotation", this->m_EditorRotation, 0.01f, -360.f, 360.f, "%.2f");
			ImGui::InputFloat3("LocalScale", localScale, "%.2f");
			{
				FLOAT worldPosition[3], worldRotation[4], worldScale[3];
				CustomType::Vector3 worldPositionVector(this->GetWorldPosition());
				CustomType::Quaternion worldRotationVector(this->GetWorldRotation());
				CustomType::Vector3 worldScaleVector(this->GetWorldScale());
				worldPosition[0] = worldPositionVector.X(); worldPosition[1] = worldPositionVector.Y(); worldPosition[2] = worldPositionVector.Z();
				worldRotation[0] = worldRotationVector.X(); worldRotation[1] = worldRotationVector.Y(); worldRotation[2] = worldRotationVector.Z(); worldRotation[3] = worldRotationVector.W();
				worldScale[0] = worldScaleVector.X(); worldScale[1] = worldScaleVector.Y(); worldScale[2] = worldScaleVector.Z();
				ImGui::Text("World position : %.2f, %.2f, %.2f", worldPosition[0], worldPosition[1], worldPosition[2]);
				ImGui::Text("World rotation : %.2f, %.2f, %.2f, %.2f", worldRotation[0], worldRotation[1], worldRotation[2], worldRotation[3]);
				ImGui::Text("World scale    : %.2f, %.2f, %.2f", worldScale[0], worldScale[1], worldScale[2]);
			}
			ImGui::EndChild();
			if (ImGui::Button("ApplyTransform") || realTimeChange)
			{
				needChangeTransform = TRUE;
			}
			if (ImGui::Button("ResetRotation"))
			{
				needChangeTransform = TRUE;
				{
					CustomType::Vector3 localPositionVector(this->GetLocalPosition());
					CustomType::Vector3 localScaleVector(this->GetLocalScale());
					localPosition[0] = localPositionVector.X(); localPosition[1] = localPositionVector.Y(); localPosition[2] = localPositionVector.Z();
					localScale[0] = localScaleVector.X(); localScale[1] = localScaleVector.Y(); localScale[2] = localScaleVector.Z();
				}
				this->m_EditorRotation[0] = 0.f; this->m_EditorRotation[1] = 0.f; this->m_EditorRotation[2] = 0.f;
			}
			ImGui::TreePop();
		}
	}

	this->m_RealTimeChangeValue = realTimeChange;
	if (needChangeTransform)
	{
		this->SetLocalPosition(CustomType::Vector3(localPosition[0], localPosition[1], localPosition[2]));
		this->SetLocalScale(CustomType::Vector3(localScale[0], localScale[1], localScale[2]));
		{
			CustomType::Quaternion rotation(CustomType::Vector3::XVector(), this->m_EditorRotation[0] * CustomType::CMath::GetDegToRad());
			{
				CustomType::Quaternion rY(CustomType::Vector3::YVector(), this->m_EditorRotation[1] * CustomType::CMath::GetDegToRad());
				rotation = CustomType::Quaternion::MultiplyQuaternion(rotation, rY);
				CustomType::Quaternion rZ(CustomType::Vector3::ZVector(), this->m_EditorRotation[2] * CustomType::CMath::GetDegToRad());
				rotation = CustomType::Quaternion::MultiplyQuaternion(rotation, rZ);
			}
			this->SetLocalRotation(rotation);
		}
	}
}
#endif
*/