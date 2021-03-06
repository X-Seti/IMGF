#ifndef CIDEEntry_OBJS_H
#define CIDEEntry_OBJS_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RockstarGames/IDE/Entry/CIDEEntry_Data.h"
#include "Format/RockstarGames/IDE/eIDESection.h"
#include "Type/Vector/CVector3D.h"
#include <string>
#include <vector>

class mcore::CIDEEntry_OBJS : public mcore::CIDEEntry_Data
{
public:
	CIDEEntry_OBJS(mcore::CIDEFormat *pIDEFormat) :
		CIDEEntry_Data(pIDEFormat, IDE_SECTION_OBJS),
		m_uiObjectId(0),
		m_uiClumpCount(0),
		m_uiFlags(0),
		m_uiUnknown2(0),
		m_vecBoundingBoxLowerLeftCorner{ 0.0f, 0.0f, 0.0f },
		m_vecBoundingBoxUpperRightCorner{ 0.0f, 0.0f, 0.0f },
		m_vecBoundingSphereCenterPosition{ 0.0f, 0.0f, 0.0f },
		m_fBoundingSphereRadius(0.0f)
	{
	};

	void						unserialize(void);
	void						serialize(void);

	void						setObjectId(uint32 uiObjectId) { m_uiObjectId = uiObjectId; }
	uint32						getObjectId(void) { return m_uiObjectId; }

	void						setModelName(std::string& strModelName) { m_strModelName = strModelName; }
	std::string&				getModelName(void) { return m_strModelName; }

	void						setTXDName(std::string& strTXDName) { m_strTXDName = strTXDName; }
	std::string&				getTXDName(void) { return m_strTXDName; }

	void						setClumpCount(uint32 uiClumpCount) { m_uiClumpCount = uiClumpCount; }
	uint32						getClumpCount(void) { return m_uiClumpCount; }

	void						setDrawDistances(std::vector<float32>& vecDrawDistances) { m_vecDrawDistances = vecDrawDistances; }
	std::vector<float32>&		getDrawDistances(void) { return m_vecDrawDistances; }

	void						setFlags(uint32 uiFlags) { m_uiFlags = uiFlags; }
	uint32						getFlags(void) { return m_uiFlags; }

	void						setUnknown2(uint32 uiUnknown2) { m_uiUnknown2 = uiUnknown2; }
	uint32						getUnknown2(void) { return m_uiUnknown2; }

	void						setBoundingBoxLowerLeftCorner(mcore::CVector3D& vecBoundingBoxLowerLeftCorner) { m_vecBoundingBoxLowerLeftCorner = vecBoundingBoxLowerLeftCorner; }
	mcore::CVector3D&					getBoundingBoxLowerLeftCorner(void) { return m_vecBoundingBoxLowerLeftCorner; }

	void						setBoundingBoxUpperRightCorner(mcore::CVector3D& vecBoundingBoxUpperRightCorner) { m_vecBoundingBoxUpperRightCorner = vecBoundingBoxUpperRightCorner; }
	mcore::CVector3D&					getBoundingBoxUpperRightCorner(void) { return m_vecBoundingBoxUpperRightCorner; }

	void						setBoundingSphereCenterPosition(mcore::CVector3D& vecBoundingSphereCenterPosition) { m_vecBoundingSphereCenterPosition = vecBoundingSphereCenterPosition; }
	mcore::CVector3D&					getBoundingSphereCenterPosition(void) { return m_vecBoundingSphereCenterPosition; }

	void						setBoundingSphereRadius(float32 fBoundingSphereRadius) { m_fBoundingSphereRadius = fBoundingSphereRadius; }
	float32						getBoundingSphereRadius(void) { return m_fBoundingSphereRadius; }

	void						setLODModel(std::string& strLODModel) { m_strLODModel = strLODModel; }
	std::string&				getLODModel(void) { return m_strLODModel; }

private:
	// GTA III era & GTA IV
	std::string					m_strModelName;
	std::string					m_strTXDName;
	std::vector<float32>		m_vecDrawDistances;
	uint32						m_uiFlags;
	uint32						m_uiUnknown2;

	// GTA III era only
	uint32						m_uiObjectId;
	uint32						m_uiClumpCount;

	// GTA IV only
	mcore::CVector3D					m_vecBoundingBoxLowerLeftCorner;
	mcore::CVector3D					m_vecBoundingBoxUpperRightCorner;
	mcore::CVector3D					m_vecBoundingSphereCenterPosition;
	float32						m_fBoundingSphereRadius;
	std::string					m_strLODModel;
};

#endif