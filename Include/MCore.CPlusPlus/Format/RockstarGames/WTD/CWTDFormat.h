#ifndef CWTDFormat_H
#define CWTDFormat_H

#include "mcore.h"
#include "Format/CFormat.h"
#include "Pool/CVectorPool.h"
#include "CWTDEntry.h"
#include "d3d9.h"
#include <string>

class CIntermediateTextureFormat;

class mcore::CWTDFormat : public mcore::CFormat, public mcore::CVectorPool<mcore::CWTDEntry*>
{
public:
	CWTDFormat(void) : CFormat(true, mcore::LITTLE_ENDIAN) {};

	void						unload(void);

	mcore::CIntermediateTextureFormat*	convertToIntermediateFormat(void);

	static uint32				getFileHeaderFlagsFromSystemAndGraphicsStreamSizes(uint32 uiSystemStreamSize, uint32 uiGraphicsStreamSize);

	static std::string			getFourCCFromD3DFormat(D3DFORMAT d3dFormat);
	static D3DFORMAT			getD3DFormatFromFourCC(std::string strFourCC);

private:
	void						unserialize(void);
	void						serialize(void);

	static uint32				getCompactSize(uint32 uiSize);
	uint32						convertULongToOffset(uint32 uiValue);
	uint32						convertULongToDataOffset(uint32 uiValue);

	std::string					decompressWTDFormatData(uint32& uiSystemSegmentSize, uint32& uiGPUSegmentSize);
};

#endif