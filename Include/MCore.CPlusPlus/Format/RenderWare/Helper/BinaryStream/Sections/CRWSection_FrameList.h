#ifndef CRWSection_FrameList_H
#define CRWSection_FrameList_H

#include "mcore.h"
#include "Format/RenderWare/Helper/BinaryStream/CRWSection.h"
#include <string>

class mcore::CRWSection_FrameList : public mcore::CRWSection
{
public:
	mcore::CRWSection_FrameList(void);

	void							unserialize(void);
	void							serialize(void);

	void							setFrameCount(uint32 uiFrameCount) { m_uiFrameCount = uiFrameCount; }
	uint32							getFrameCount(void) { return m_uiFrameCount; }

	void							setFrameData(std::string& strFrameData) { m_strFrameData = strFrameData; }
	std::string&					getFrameData(void) { return m_strFrameData; }

private:
	uint32							m_uiFrameCount;
	std::string						m_strFrameData;
};

#endif