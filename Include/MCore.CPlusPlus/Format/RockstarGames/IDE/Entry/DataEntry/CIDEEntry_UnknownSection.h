#ifndef CIDEEntry_UnknownSection_H
#define CIDEEntry_UnknownSection_H

#include "mcore.h"
#include "Type/Types.h"
#include "Format/RockstarGames/IDE/Entry/CIDEEntry_Data.h"
#include <string>

class mcore::CIDEEntry_UnknownSection : public mcore::CIDEEntry_Data
{
public:
	CIDEEntry_UnknownSection(mcore::CIDEFormat *pIDEFormat) :
		CIDEEntry_Data(pIDEFormat, IDE_SECTION_UNKNOWN)
	{
	};

	void						unserialize(void);
	void						serialize(void);

	void						setLine(std::string& strLine) { m_strLine = strLine; }
	std::string&				getLine(void) { return m_strLine; }

private:
	std::string					m_strLine;
};

#endif