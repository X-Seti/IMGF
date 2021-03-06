#ifndef CIPLFormat_H
#define CIPLFormat_H

#include "mcore.h"
#include "Format/CFormat.h"
#include "Format/RockstarGames/Helper/SectionLines/CSectionLinesFormat.h"
#include "eIPLSection.h"
#include "eIPLPathType.h"
#include "Entry/CIPLEntry_Other.h"
#include "Entry/CIPLEntry_Section.h"
#include "Entry/CIPLEntry_Data.h"
#include <string>
#include <vector>

class mcore::CIPLFormat;
class mcore::CIPLEntry;

class mcore::CIPLFormat : public mcore::CSectionLinesFormat<mcore::CIPLFormat, mcore::CIPLEntry, eIPLSection, mcore::CIPLEntry_Other, mcore::CIPLEntry_Section, mcore::CIPLEntry_Data>
{
public:
	CIPLFormat(void);

	void												setIsBinary(bool bState) { m_bIsBinary = bState; }
	bool												isBinary(void) { return m_bIsBinary; }

	mcore::CIPLEntry_Data*										createDataEntry(eIPLSection eIDESectionValue, uint32 uiSectionSpecificType = 0);

	eIPLSection											getSectionFromText(std::string strIPLSectionText);
	std::string											getSectionText(eIPLSection eIPLSectionValue);

	uint32												detectSectionSpecificType(eIPLSection eIPLSectionValue);

private:
	void												unserialize(void);
	void												serialize(void);

	void												unserializeBinary(void);
	void												serializeBinary(void);

	mcore::CIPLEntry_Data*										unserializeDataEntry(eIPLSection eIPLSectionValue);

	eIPLPathType										detectPATHType(void);

private:
	uint8												m_bIsBinary : 1;
};

#endif