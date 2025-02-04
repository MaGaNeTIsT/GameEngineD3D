#pragma once
#include <CoreMinimal.h>
#include <Base/DataStructure/Container/Map.h>
#include <PigeonBase/Object/Component/Primitive/PrimitiveComponent.h>
#include "PhysicsUtility.h"
#include "JoltIncludes.h"
#include "JoltPhysicsLayer.h"
#include "JoltPhysicsListener.h"
#include "PhysicsManagerInterface.h"
#include "Shapes.h"
#include "PhysicsData.h"

PIGEONENGINE_NAMESPACE_BEGIN
// All Jolt symbols are in the JPH namespace
using namespace JPH;
using namespace PhysicsUtility;

struct FPhysicsBodyId
{
	BodyID ID;

	/// Equals check
	inline BOOL32				operator == (const FPhysicsBodyId& inRHS) const
	{
		return ID == inRHS.ID;
	}

	/// Not equals check
	inline BOOL32				operator != (const FPhysicsBodyId& inRHS) const
	{
		return ID != inRHS.ID;
	}

	/// Smaller than operator, can be used for sorting bodies
	inline BOOL32				operator < (const FPhysicsBodyId& inRHS) const
	{
		return ID < inRHS.ID;
	}

	/// Greater than operator, can be used for sorting bodies
	inline BOOL32				operator > (const FPhysicsBodyId& inRHS) const
	{
		return ID > inRHS.ID;
	}

	/// Get index in body array
	inline UINT32			GetIndex() const
	{
		return ID.GetIndex();;
	}

	/// Get sequence number of body.
	/// The sequence number can be used to check if a body ID with the same body index has been reused by another body.
	/// It is mainly used in multi threaded situations where a body is removed and its body index is immediately reused by a body created from another thread.
	/// Functions querying the broadphase can (after aquiring a body lock) detect that the body has been removed (we assume that this won't happen more than 128 times in a row).
	inline UINT8			GetSequenceNumber() const
	{
		return ID.GetSequenceNumber();
	}

	/// Returns the index and sequence number combined in an uint32
	inline UINT32			GetIndexAndSequenceNumber() const
	{
		return ID.GetIndexAndSequenceNumber();
	}

	/// Check if the ID is valid
	inline BOOL32				IsInvalid() const
	{
		return ID.IsInvalid();
	}
};

static void TraceImpl(const CHAR* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	CHAR buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);

	// Print to the TTY
}

//template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Alloc = std::allocator<std::pair<const Key, T>>> using THashMap = std::unordered_map<Key, T, Hash, KeyEqual, Alloc>;

class FPhysics_Jolt : public IPhysicsManagerInterface
{
public:
	FPhysics_Jolt() : PhysicsData(nullptr) {}
	virtual ~FPhysics_Jolt() {}
public:
	virtual void InitPhysics();
	virtual void UninitPhysics();
	virtual void PrePhysicsUpdate();
	virtual void PhysicsUpdate(FLOAT DeltaTime);
	virtual void PostPhysicsUpdate();

	FPhysicsData* GetPhysicsData() { return PhysicsData; }

	void AddCharacter(class FCharacter* Character);
	void RemoveCharacter(class FCharacter* Character);

public:
	BOOL32 TryCreateBody(FShape* inShape, BOOL32 CreateNew, Vector3 inPosition, Quaternion inRotation, PhysicsUtility::EMotionType inMotionType, UINT16 inLayer, FPhysicsBodyId& outBodyID);
	void AddBody(const ObjectIdentityType& GameObjectId, const FPhysicsBodyId& inBodyID, EActivate inActivationMode = EActivate::DontActivate);
	/// <summary>
	/// remove a rigid body by a GameObjectId.
	/// </summary>
	/// <param name="GameObjectId">Id</param>
	/// <param name="bDeleteShape">TRUE to delete shape at sametime,if your shape is shared to other bodys, set FALSE.</param>
	void RemoveBody(const ObjectIdentityType& GameObjectId, BOOL32 bDeleteShape = TRUE);

	Vector3 GetPosition(const FPhysicsBodyId& FPhysicsBodyId);
	Quaternion GetRotation(const FPhysicsBodyId& FPhysicsBodyId);
	void SetPosition(const FPhysicsBodyId& inPhysicsBodyId, Vector3 inPosition, EActivate inActivationMode = EActivate::DontActivate);
	void SetRoation(const FPhysicsBodyId& inPhysicsBodyId, Quaternion inRotation, EActivate inActivationMode = EActivate::DontActivate);
	void AddForce(const FPhysicsBodyId& inPhysicsBodyId, Vector3 inForce);
	void AddForce(const FPhysicsBodyId& inPhysicsBodyId, Vector3 inForce, Vector3 inPoint);
	void AddImpulse(const FPhysicsBodyId& inPhysicsBodyId, Vector3 inImpulse); ///< Applied at center of mass
	void AddImpulse(const FPhysicsBodyId& inPhysicsBodyId, Vector3 inImpulse, Vector3 inPoint); ///< Applied at inPoint

	void SetGravity(Vector3 inGravity);
private:
	TMap<ObjectIdentityType, FPhysicsBodyId>					m_Bodys;
	TMap<FPhysicsBodyId, FShape*>							m_Shapes;
	TSet<class FCharacter*>									m_Characters;
private:
	FPhysicsData* PhysicsData;
};
PIGEONENGINE_NAMESPACE_END