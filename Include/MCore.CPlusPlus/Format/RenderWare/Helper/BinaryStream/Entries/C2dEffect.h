#ifndef C2dEffect_H
#define C2dEffect_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RockstarGames/e2DFXType.h"
#include "Type/Vector/CVector3D.h"

class mcore::C2dEffect;

class mcore::C2dEffect
{
public:
	C2dEffect(e2DFXType e2DFXTypeValue) :
		m_e2DFXType(e2DFXTypeValue),
		m_uiDataSize(0),
		m_vecPosition{ 0.0f, 0.0f, 0.0f }
	{};

	void							unload(void) {}
	
	virtual void					unserialize(void)	= 0;
	virtual void					serialize(void)		= 0;
	
	static mcore::C2dEffect*				createRW2dEffect(e2DFXType e2DFXTypeValue);
	
	void							setDataSize(uint32 uiDataSize) { m_uiDataSize = uiDataSize; }
	uint32							getDataSize(void) { return m_uiDataSize; }

	void							setPosition(mcore::CVector3D& vecPosition) { m_vecPosition = vecPosition; }
	mcore::CVector3D&						getPosition(void) { return m_vecPosition; }

	void							set2DFXType(e2DFXType e2DFXTypeValue) { m_e2DFXType = e2DFXTypeValue; }
	e2DFXType						get2DFXType(void) { return m_e2DFXType; }

protected:
	uint32							m_uiDataSize;

private:
	mcore::CVector3D						m_vecPosition;
	e2DFXType						m_e2DFXType;
};

#endif