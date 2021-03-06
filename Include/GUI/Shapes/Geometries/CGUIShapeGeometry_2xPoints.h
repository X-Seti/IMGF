#ifndef CGUIShapeGeometry_2xPoints_H
#define CGUIShapeGeometry_2xPoints_H

#include "Type/Types.h"
#include "Type/Vector/CVector2i32.h"
#include "Type/Vector/CPoint2D.h"
#include "Shape/eGUIShape.h"
#include "Shape/CGUIShape.h"

class CGUIShapeGeometry_2xPoints : public CGUIShape
{
public:
	CGUIShapeGeometry_2xPoints(eGUIShape eShapeType);

	void							unserialize(bool bSkipShapeId = false);
	void							serialize(void);

	void							moveItem(mcore::CVector2i32& vecPositionChange);
	void							resizeItemViaOffsets(mcore::CVector2i32& vecItemSizeChange) {}

	void							setPoint1(mcore::CPoint2D& vecPoint1) { m_vecPoint1 = vecPoint1; }
	mcore::CPoint2D&						getPoint1(void) { return m_vecPoint1; }

	void							setPoint2(mcore::CPoint2D& vecPoint2) { m_vecPoint2 = vecPoint2; }
	mcore::CPoint2D&						getPoint2(void) { return m_vecPoint2; }
	
private:
	mcore::CPoint2D						m_vecPoint1;
	mcore::CPoint2D						m_vecPoint2;
};

#endif