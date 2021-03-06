#ifndef CRWEntry_2dEffect_EnterExit_H
#define CRWEntry_2dEffect_EnterExit_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RenderWare/Helper/BinaryStream/Entries/C2dEffect.h"
#include "Type/Vector/CVector2D.h"
#include "Type/Vector/CVector3D.h"
#include <string>

class mcore::CRWEntry_2dEffect_EnterExit : public mcore::C2dEffect
{
public:
	CRWEntry_2dEffect_EnterExit(void);

	void							unserialize(void);
	void							serialize(void);

	void							setEnterMarkerRotationAngle(float32 fEnterMarkerRotationAngle) { m_fEnterMarkerRotationAngle = fEnterMarkerRotationAngle; }
	float32							getEnterMarkerRotationAngle(void) { return m_fEnterMarkerRotationAngle; }

	void							setMarkerApporximationRadius(mcore::CVector2D& vecMarkerApporximationRadius) { m_vecMarkerApporximationRadius = vecMarkerApporximationRadius; }
	mcore::CVector2D&						getMarkerApporximationRadius(void) { return m_vecMarkerApporximationRadius; }

	void							setExitMarkerPosition(mcore::CVector3D& vecExitMarkerPosition) { m_vecExitMarkerPosition = vecExitMarkerPosition; }
	mcore::CVector3D&						getExitMarkerPosition(void) { return m_vecExitMarkerPosition; }

	void							setExitMarkerAngleRotation(float32 fExitMarkerAngleRotation) { m_fExitMarkerAngleRotation = fExitMarkerAngleRotation; }
	float32							getExitMarkerAngleRotation(void) { return m_fExitMarkerAngleRotation; }

	void							setInteriorCount(int32 iInteriorCount) { m_iInteriorCount = iInteriorCount; }
	int32							getInteriorCount(void) { return m_iInteriorCount; }

	void							setFlags(int32 iFlags) { m_iFlags = iFlags; }
	int32							getFlags(void) { return m_iFlags; }
	
	void							setInteriorName(std::string& strInteriorName) { m_strInteriorName = strInteriorName; }
	std::string&					getInteriorName(void) { return m_strInteriorName; }

	void							setSkyColour(int32 iSkyColour) { m_iSkyColour = iSkyColour; }
	int32							getSkyColour(void) { return m_iSkyColour; }

private:
	float32							m_fEnterMarkerRotationAngle; // -marker(relative to the object) (Always 0 ? )
	mcore::CVector2D						m_vecMarkerApporximationRadius; // The radius of the approximation to the marker X and Y
	mcore::CVector3D						m_vecExitMarkerPosition; // The position of exit - marker(offset relative to the object) (Always 0 ? )
	float32							m_fExitMarkerAngleRotation; // Rotation exit - marker(relative to the object) (always 0 ? )
	int32							m_iInteriorCount; // Number of interior
	int32							m_iFlags;
	std::string						m_strInteriorName; // -The name of the interior
	int32							m_iSkyColour; // color of the sky
};

#endif