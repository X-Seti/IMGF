#ifndef CIDEFormat_H
#define CIDEFormat_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RockstarGames/Helper/SectionLines/CSectionLinesFormat.h"
//#include "eDataType.h" // todo - no idea why this include is needed - it makes eIDESection underlined when not here
#include "eIDESection.h"
#include "Format/RockstarGames/e2DFXType.h"
#include "eIDEPathType.h"
#include "Entry/CIDEEntry_Other.h"
#include "Entry/CIDEEntry_Section.h"
#include "Entry/CIDEEntry_Data.h"
#include <string>
#include <vector>

class mcore::CIDEFormat;
class mcore::CIDEEntry;

class mcore::CIDEFormat : public mcore::CSectionLinesFormat<mcore::CIDEFormat, mcore::CIDEEntry, mcore::eIDESection, mcore::CIDEEntry_Other, mcore::CIDEEntry_Section, mcore::CIDEEntry_Data>
{
public:
	CIDEFormat(void);

	mcore::CIDEEntry_Data*								createDataEntry(eIDESection eIDESectionValue, uint32 uiSectionSpecificType = 0);
	
	template<class EntryClass2>
	std::vector<EntryClass2*>					getEntriesBySection(eIDESection eIDESectionValue, mcore::e2DFXType e2DFXTypeValue = mcore::_2DFX_UNKNOWN);

	eIDESection									getSectionFromText(std::string strIDESectionText);
	std::string									getSectionText(eIDESection eIDESectionValue);

	uint32										detectSectionSpecificType(eIDESection eIDESectionValue);

private:
	void										unserialize(void);
	void										serialize(void);

	mcore::CIDEEntry_Data*								unserializeDataEntry(eIDESection eIDESectionValue);

	std::pair<mcore::e2DFXType, uint32>				detect2DFXTypeAndGame(void);
	eIDEPathType								detectPATHType(void);
};

template<class EntryClass2>
std::vector<EntryClass2*>						mcore::CIDEFormat::getEntriesBySection(eIDESection eIDESectionValue, mcore::e2DFXType e2DFXTypeValue)
{
	if (eIDESectionValue != IDE_SECTION_2DFX || e2DFXTypeValue == mcore::_2DFX_UNKNOWN)
	{
		return CSectionLinesFormat::getEntriesBySection<EntryClass2>(eIDESectionValue);
	}
	else
	{
		std::vector<EntryClass2*> vecDerivedEntries;
		for (EntryClass2* pIDEEntry : CSectionLinesFormat::getEntriesBySection<EntryClass2>(eIDESectionValue))
		{
			if (((CIDEEntry_2DFX*)pIDEEntry)->get2DFXType() == e2DFXTypeValue)
			{
				vecDerivedEntries.push_back(pIDEEntry);
			}
		}
		return vecDerivedEntries;
	}
}

#endif