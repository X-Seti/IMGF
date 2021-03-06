#ifndef mcore_H
#define mcore_H

#include <string>

namespace mcore
{
	class CGameManager;
	class CGame;
	enum eGame;
	enum eGameFlag;
	enum ePlatformedGame;

	enum eCompressionAlgorithm;

	class CDebug;

	class CEventBinder;

	class CEventBoundFunction;
	class CEventManager;
	class CEventType;
	class CInputEventCallbacks;
	enum eEvent;
	enum eEventFunctionType;
	enum eEventType;

	enum eExceptionCode;



	template<class T> class CFormatManager;
	class CFormat;

	class CBMPFormat;
	struct CBMPFormat_Header1;
	struct CBMPFormat_Header2_Version2;
	struct CBMPFormat_Header2_Version3;
	struct CBMPFormat_Header2_Version4;
	class CBMPManager;

	class CCURFormat;
	class CCURManager;

	class CDDSFormat;
	struct CDDSFormat_Header_DDS_HEADER;
	struct CDDSFormat_Header_DDS_HEADER_DXT10;
	struct CDDSFormat_Header_DDS_PIXELFORMAT_Part1;
	struct CDDSFormat_Header_DDS_PIXELFORMAT_Part2;
	class CDDSManager;

	class CICOFormat;
	class CICOManager;

	class CIntermediateBoundingObject;
	class CIntermediateGeometry;
	class CIntermediateIndexData;
	class CIntermediateVertexData;
	enum eIntermediateBoundingObjectType;
	class CIntermediateModel;
	class CIntermediateModelFormat;

	class CIntermediateTextureMipmap;
	class CIntermediateTexture;
	class CIntermediateTextureFormat;

	enum eJsonTypes;
	class CJSONString;
	class CJSONValue;
	class CJSONNumber;
	class CJSONBoolean;
	class CJSONNull;
	class CJSONContainer;
	class CJSONArray;
	class CJSONObject;
	class CJSONManager;

	class CRageManager;
	class CRageResourceType;
	class CRageResourceTypeManager;
	enum eRageResourceType;

	class CRWFormat;
	class CRWManager;
	class CRWSection;
	class CRWSectionContainer;
	class C2dEffect;
	class CRWSectionDepthData;
	class CRWVersion;
	class CRWVersionManager;
	class CTextureEntry;
	enum eRWSection;
	enum eRWVersion;
	enum eRWVersionType;
	class CRWEntry_BinMeshPLG_Mesh;
	class CRWEntry_TextureNative_MipMap;
	class CRWEntry_2dEffect_CoverPoint;
	class CRWEntry_2dEffect_EnterExit;
	class CRWEntry_2dEffect_Escalator;
	class CRWEntry_2dEffect_Light;
	class CRWEntry_2dEffect_ParticleEffect;
	class CRWEntry_2dEffect_PedAttractor;
	class CRWEntry_2dEffect_SlotmachineWheel;
	class CRWEntry_2dEffect_StreetSign;
	class CRWEntry_2dEffect_SunGlare;
	class C2dEffectsPool;
	class CRWGeometryBoundingInfo;
	class CRWSection_2dEffect;
	class CRWSection_Atomic;
	class CRWSection_BinMeshPLG;
	class CRWSection_Clump;
	class CRWSection_Extension;
	class CRWSection_ExtraVertColour;
	class CRWSection_Frame;
	class CRWSection_FrameList;
	class CRWSection_Geometry;
	class CRWSection_GeometryList;
	class CRWSection_Light;
	class CRWSection_Material;
	class CRWSection_MaterialList;
	class CRWSection_String;
	class CRWSection_Texture;
	class CRWSection_TextureDictionary;
	class CRWSection_TextureNative;
	class CRWSection_UnknownSection;

	class CURL;



	enum e2DFXType;
	class CCOLEntry;
	class CCOLFormat;
	class CCOLManager;
	class CCOLVersion;
	class CCOLVersionManager;
	enum eCOLVersion;
	struct TBounds;
	struct TSurface;
	struct TSphere;
	struct TBox;
	struct TFaceGroup;
	struct TVertex;
	struct TFace;

	class CDATAnimgrpSection;
	class CDATAnimgrpEntry;
	class CDATAnimgrpFormat;

	class CDATCAPSFormat;

	class CDATChaseEntry;
	class CDATChaseFormat;
	class CDATChaseEntry_Raw;

	struct CDATEntry_Paths_Default_Header;
	struct CDATEntry_Paths_Default_PathNode;
	struct CDATEntry_Paths_Default_NaviNode;
	struct CDATEntry_Paths_Default_Link;
	struct CDATEntry_Paths_Default_NaviLink;
	struct CDATEntry_Paths_Default_LinkLength;
	struct CDATEntry_Paths_Default_Link_Wrapper;
	class CompressedVector;
	class CompressedVector_extended;
	struct CNodeAddress;
	struct CDATEntry_Paths_Fastman92_Header;
	struct CDATEntry_Paths_Fastman92_PathNode;
	struct CDATEntry_Paths_Fastman92_PathNode_extended;
	struct CDATEntry_Paths_Fastman92_NaviNode;
	struct CDATEntry_Paths_Fastman92_NaviNode_extended;
	struct CDATEntry_Paths_Fastman92_Link;
	struct CDATEntry_Paths_Fastman92_NaviLink;
	struct CDATEntry_Paths_Fastman92_LinkLength;
	struct CDATEntry_Paths_Fastman92_PathIntersectionFlags;
	struct CDATEntry_Paths_General_Header;
	struct CDATEntry_Paths_General_PathNode;
	struct CDATEntry_Paths_General_NaviNode;
	struct CDATEntry_Paths_General_Link;
	class CDATPathFormat;
	class CDATPathManager;
	enum eDATPathFormat;

	class CDATLoaderEntry;
	class CDATLoaderFormat;
	enum eDATLoaderEntryType;
	class CDATLoaderManager;

	class CDATScriptPathEntry;
	class CDATScriptPathFormat;

	class CDATWaterproEntry;
	class CDATWaterproFormat;

	class CDFFFormat;
	class CDFFManager;

	class CIDEEntry;
	class CIDEFormat;
	class CIDEManager;
	enum eIDEPathType;
	enum eIDESection;
	class CIDEEntry_Data;
	class CIDEEntry_Other;
	class CIDEEntry_Section;
	class CIDEEntry_2DFX;
	class CIDEEntry_AGRPS;
	class CIDEEntry_AMAT;
	class CIDEEntry_ANIM;
	class CIDEEntry_CARS;
	class CIDEEntry_HAND;
	class CIDEEntry_HIER;
	class CIDEEntry_LINK;
	class CIDEEntry_LODM;
	class CIDEEntry_MLO;
	class CIDEEntry_OBJS;
	class CIDEEntry_PATH;
	class CIDEEntry_PEDS;
	class CIDEEntry_TANM;
	class CIDEEntry_TOBJ;
	class CIDEEntry_TREE;
	class CIDEEntry_TXDP;
	class CIDEEntry_UnknownSection;
	class CIDEEntry_VNOD;
	class CIDEEntry_WEAP;
	class CIDEEntry_2DFX_Audio;
	class CIDEEntry_2DFX_BouyancyAttr;
	class CIDEEntry_2DFX_Escalator;
	class CIDEEntry_2DFX_Explosion;
	class CIDEEntry_2DFX_Ladder;
	class CIDEEntry_2DFX_Light;
	class CIDEEntry_2DFX_LightShaft;
	class CIDEEntry_2DFX_Particle;
	class CIDEEntry_2DFX_Ped;
	class CIDEEntry_2DFX_ProcObject;
	class CIDEEntry_2DFX_ScrollBar;
	class CIDEEntry_2DFX_SpawnPoint;
	class CIDEEntry_2DFX_SunReflection;
	class CIDEEntry_2DFX_SwayableAttr;
	class CIDEEntry_2DFX_Unknown1;
	class CIDEEntry_2DFX_WalkDontWalkAttr;
	class CIDEEntry_2DFX_WorldPoint;
	class CIDEEntry_PATH_Group;
	class CIDEEntry_PATH_Node;

	class CIMGEntry;
	class CIMGFormat;
	class CIMGManager;
	class CIMGPeekData;
	enum eIMGEntryProperty;
	enum eIMGVersion;
	struct CIMGEntry_Fastman92;
	struct CIMGFormat_VersionFastman92_Header1;
	struct CIMGFormat_VersionFastman92_Header2;
	enum eIMGVersionFastman92CompressionAlgorithm;
	struct RG_CIMGEntry_Version1Or2;
	struct RG_CIMGEntry_Version3;
	struct RG_CIMGFormat_Version2_Header1;
	struct RG_CIMGFormat_Version3_Header1;

	class CIPLEntry_PATH_Group;
	class CIPLEntry_PATH_Node;
	class CIPLEntry_2DFX;
	class CIPLEntry_AUZO;
	class CIPLEntry_BLOK;
	class CIPLEntry_CARS;
	class CIPLEntry_CULL;
	class CIPLEntry_ENEX;
	class CIPLEntry_GRGE;
	class CIPLEntry_INST;
	class CIPLEntry_JUMP;
	class CIPLEntry_LINK;
	class CIPLEntry_LODM;
	class CIPLEntry_MLOPLUS;
	class CIPLEntry_MULT;
	class CIPLEntry_MZON;
	class CIPLEntry_OCCL;
	class CIPLEntry_PATH;
	class CIPLEntry_PICK;
	class CIPLEntry_RTFX;
	class CIPLEntry_SLOW;
	class CIPLEntry_TCYC;
	class CIPLEntry_UnknownSection;
	class CIPLEntry_VNOD;
	class CIPLEntry_ZONE;
	class CIPLEntry_Data;
	class CIPLEntry_Other;
	class CIPLEntry_Section;
	class CIPLEntry;
	class CIPLFormat;
	class CIPLManager;
	enum eIPLPathType;
	enum eIPLSection;

	template<class FormatClass, typename SectionEnum> class CSectionLinesEntry;
	template<class FormatClass, class EntryClass, typename SectionEnum, class OtherEntryClass, class SectionEntryClass, class DataEntryClass> class CSectionLinesFormat;
	enum eSectionLinesEntryType;
	class CSectionLinesEntry_Data;
	class CSectionLinesEntry_Other;

	class CTXDFormat;
	class CTXDManager;
	enum eTXDRasterDataFormat;

	class CWDRBlock_Geometry;
	class CWDRBlock_IndexData;
	class CWDRBlock_Model;
	class CWDRBlock_VertexData;
	class CWDRBoundingObject;
	class CWDRFormat;
	class CWDRManager;

	class CWTDEntry;
	class CWTDFormat;
	class CWTDManager;
	class CWTDMipmap;
	struct RG_CWTDEntry;
	struct RG_CWTDFormat_Header1;
	struct RG_CWTDFormat_Header2;

	class CINIManager;



	class CImageFile;
	class CImageManager;
	class CRasterDataFormat;
	class CRasterDataFormatManager;

	class CLocalizationFormat;
	class CLocalizationManager;
	enum eLanguage;

	template<class Class> class CIndexedInstance;
	template<class SingletonClass> class CSingleton;
	class CManager;

	class CPlatform;
	class CPlatformManager;
	enum ePlatform;

	template <class T> class CVectorPool;
	template <class Key, class Value> class CUMapContainer;
	template <class KeyClass> class CMultipleTypeValuesUMapContainer;

	class CDataReader;
	class CDataStream;
	enum eDataStreamType;
	class CDataWriter;

	class CTiming;

	enum eEndian;

	class CCompressionManager;
	class CFileManager;
	class CGUIManager;
	class CImageManager;
	enum eDXTCompressionType;
	enum eRasterDataFormat;
	class CMath;
	class CMemoryManager;
	class CNumber;
	class CPathManager;
	class CGUIManager;
	class CProcessManager;
	class CRegistryManager;
	class CString2;
	class CStdVector;

	class CColour;
	class CPoint2D;
	class CSize2D;
	class CVector2D;
	class CVector2i32;
	class CVector2ui8;
	class CVector2ui16;
	class CVector2ui32;
	class CVector3D;
	class CVector3i8;
	class CVector3i16;
	class CVector3ui8;
	class CVector3ui32;
	class CVector4D;
	class CVector4ui8;
	class CVector4ui16;
	class CVector4ui32;
	
	extern std::string g_strBlankString;
	extern bool g_bDefaultBool;

	void init(void);
};

#endif