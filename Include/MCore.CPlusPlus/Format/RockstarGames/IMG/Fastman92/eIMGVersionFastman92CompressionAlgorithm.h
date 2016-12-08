#ifndef eIMGVersionFastman92CompressionAlgorithm_H
#define eIMGVersionFastman92CompressionAlgorithm_H

#include "mcore.h"

enum mcore::eIMGVersionFastman92CompressionAlgorithm // todo - rename other enum with algoruthm in it to not have Id at the end
{
	IMGFASTMAN92COMPRESSIONALGORITHM_UNKNOWN,
	IMGFASTMAN92COMPRESSIONALGORITHM_UNCOMPRESSED,
	IMGFASTMAN92COMPRESSIONALGORITHM_ZLIB,
	IMGFASTMAN92COMPRESSIONALGORITHM_LZ4,
};

#endif