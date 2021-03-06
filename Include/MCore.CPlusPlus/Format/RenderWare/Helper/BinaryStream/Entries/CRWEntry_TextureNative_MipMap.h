#ifndef CRWEntry_TextureNative_MipMap_H
#define CRWEntry_TextureNative_MipMap_H

#include "mcore.h"
#include "Type/Types.h"
#include "Type/Vector/CSize2D.h"
#include "Image/eRasterDataFormat.h"
#include <string>

class mcore::CRWSection_TextureNative;

class mcore::CRWEntry_TextureNative_MipMap
{
public:
	CRWEntry_TextureNative_MipMap(CRWSection_TextureNative *pTextureNative) :
		m_pTextureNative(pTextureNative)
	{
		m_vecImageSize.m_x = 0;
		m_vecImageSize.m_y = 0;
		m_vecSwizzledImageSize.m_x = 0;
		m_vecSwizzledImageSize.m_y = 0;
	};

	void						unload(void) {}

	bool						canRasterDataBeConverted(void);
	mcore::eRasterDataFormat			getRasterDataFormat(void);
	std::string					getRasterDataBGRA32(void);
	std::string					getRasterDataRGBA32(void);

	void						convertToRasterDataFormat(mcore::eRasterDataFormat eRasterDataFormatValue);

	void						setRasterData(std::string& strData) { m_strRasterData = strData; }
	std::string&				getRasterData(void) { return m_strRasterData; }

	void						setImageSize(mcore::CSize2D& vecImageSize) { m_vecImageSize = vecImageSize; }
	mcore::CSize2D&					getImageSize(void) { return m_vecImageSize; }

	void						setSwizzledImageSize(mcore::CSize2D& vecSwizzledImageSize) { m_vecSwizzledImageSize = vecSwizzledImageSize; }
	mcore::CSize2D&					getSwizzledImageSize(void) { return m_vecSwizzledImageSize; }

	void						setTexture(CRWSection_TextureNative *pTextureNative) { m_pTextureNative = pTextureNative; }
	CRWSection_TextureNative*	getTexture(void) { return m_pTextureNative; }

private:
	CRWSection_TextureNative*	m_pTextureNative;
	mcore::CSize2D						m_vecImageSize;
	mcore::CSize2D						m_vecSwizzledImageSize;
	std::string					m_strRasterData;
};

#endif