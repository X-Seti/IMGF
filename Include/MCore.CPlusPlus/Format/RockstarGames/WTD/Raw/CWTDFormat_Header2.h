#ifndef RG_CWTDFormat_Header2_H
#define RG_CWTDFormat_Header2_H

#include "mcore.h"
#include "Type/Types.h"

#pragma pack(push, 1)
struct mcore::RG_CWTDFormat_Header2
{
	uint32					m_uiVTable;
	uint32					m_uiBlockMapOffset;
	uint32					m_uiParentDictionary;
	uint32					m_uiUsageCount;
	uint32					m_uiHashTableOffset;
	uint16					m_usTextureCount;
	uint16					m_usUnknown1;
	uint32					m_uiTextureListOffset;
	uint16					m_usUnknown2;
	uint16					m_usUnknown3;
};
#pragma pack(pop)

#endif