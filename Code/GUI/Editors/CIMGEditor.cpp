#include "CIMGEditor.h"
#include "Globals.h"
#include "CIMGF.h"
#include "Format/RockstarGames/IMG/CIMGManager.h"
#include "Type/String/CString2.h"
#include "Path/CPathManager.h"
#include "File/CFileManager.h"
#include "Localization/CLocalizationManager.h"
#include "CSettingsManager.h"
#include "Format/RockstarGames/COL/CCOLManager.h"
#include "Format/RockstarGames/IMG/CIMGEntry.h"
#include "Format/RenderWare/Helper/BinaryStream/CRWVersion.h"
#include "Task/CTaskManager.h"
#include "Task/CTaskDispatchManager.h"
#include "GUI/Window/CIMGFWindow.h"
#include "Styles/CGUIStyles.h"
#include "Controls/CDropControl.h"
#include "Controls/CTabControl.h"
#include "Controls/CListControl.h"
#include "Controls/CButtonControl.h"
#include "Type/Vector/CColour.h"
#include "CGUIManager.h"

// for menu start - todo
#include "Format/RenderWare/Helper/BinaryStream/CRWManager.h"
#include "Format/RenderWare/Helper/BinaryStream/CRWVersionManager.h"
#include "RecentlyOpen/CRecentlyOpenManager.h"
#include "Sort/CSortManager.h"
#include "Sort/CSortType.h"
#include "Sort/CSortPriority.h"
#include "Sort/CSortTypes.h"
#include "Sort/CSortPriorities.h"
#include "Sort/CSortMenuItems.h"
#include "Image/CImageManager.h"
#include "Image/CRasterDataFormatManager.h"
#include "Format/RockstarGames/COL/CCOLManager.h"
#include "Format/RockstarGames/COL/CCOLVersionManager.h"
#include "Format/RockstarGames/COL/CCOLVersion.h"
// for menu end

using namespace std;
using namespace mcore;

CIMGEditorTab*		CIMGEditor::addBlankTab(string strIMGPath, eIMGVersion eIMGVersionValue)
{
	getIMGF()->getTaskManager()->setTaskMaxProgressTickCount(1);

	CIMGFormat *pIMGFormat = new CIMGFormat;
	pIMGFormat->setFilePath(strIMGPath);
	pIMGFormat->setIMGVersion(eIMGVersionValue);

	CIMGEditorTab *pEditorTab = _addTab(pIMGFormat);
	if (!pEditorTab)
	{
		getIMGF()->getTaskManager()->onTaskProgressTick();
		return nullptr;
	}

	pEditorTab->log(CLocalizationManager::getInstance()->getTranslatedFormattedText("Log_26", CPathManager::getFileName(strIMGPath).c_str()));
	pEditorTab->log(CLocalizationManager::getInstance()->getTranslatedFormattedText("Log_27", CIMGManager::getIMGVersionNameWithGames(eIMGVersionValue, pIMGFormat->isEncrypted()).c_str()), true);
	getIMGF()->getTaskManager()->onTaskProgressTick();

	return pEditorTab;
}

CIMGEditorTab*		CIMGEditor::addTab(string strIMGPath, eIMGVersion eIMGVersionValue)
{
	uint32 uiMultiplier = eIMGVersionValue == IMG_3 ? 4 : 3; // x3 for: 1 for reading entry data (parsing), 1 for reading RW versions (parsing), and 1 for adding all entries to main list view. and x4 (+1) for version 3 entry names.
	getIMGF()->getTaskManager()->setTaskMaxProgressTickCount(CIMGManager::getIMGEntryCount(strIMGPath, eIMGVersionValue) * uiMultiplier);

	CIMGFormat *pIMGFormat = CIMGManager::getInstance()->parseViaFile(strIMGPath/*, eIMGVersionValue - todo*/);

	CIMGEditorTab *pEditorTab = _addTab(pIMGFormat);
	if (!pEditorTab)
	{
		getIMGF()->getTaskManager()->onTaskProgressComplete();
		return nullptr;
	}

	pEditorTab->log(CLocalizationManager::getInstance()->getTranslatedFormattedText("Log_28", CPathManager::getFileName(strIMGPath).c_str()));
	pEditorTab->log(CLocalizationManager::getInstance()->getTranslatedFormattedText("Log_27", CIMGManager::getIMGVersionNameWithGames(eIMGVersionValue, pIMGFormat->isEncrypted()).c_str()), true);
	pEditorTab->log(CLocalizationManager::getInstance()->getTranslatedFormattedText("Log_29", CFileManager::getFileSize(strIMGPath)), true);
	
	return pEditorTab;
}

CIMGEditorTab*		CIMGEditor::_addTab(CIMGFormat *pIMGFormat)
{
	uint32 uiTabIndex = getTabs().getNextEntryIndex();

	CIMGEditorTab *pEditorTab = new CIMGEditorTab;
	pEditorTab->setWindow(this);
	pEditorTab->setIndex(uiTabIndex);
	pEditorTab->setIMGFile(pIMGFormat);

	if (pEditorTab->onTabFormatReady())
	{
		getTabs().addEntry(pEditorTab);
		setActiveTab(pEditorTab);
		return pEditorTab;
	}
	else
	{
		pEditorTab->unload();
		delete pEditorTab;

		getIMGF()->getTaskManager()->setTaskMaxProgressTickCount(1); // todo - have like getIMGF()->getTaskManager()->setProgressComplete(void) instead of these 2 lines
		getIMGF()->getTaskManager()->onTaskProgressTick();

		return nullptr;
	}
}

void				CIMGEditor::removeTab(CEditorTab *pEditorTab)
{
	// fetch current tab index
	uint32 uiTabIndex = pEditorTab->getIndex();

	// remove from pool and unload
	getTabs().removeEntry(pEditorTab);

	// shift down higher indices by 1
	//((CTabCtrl*)getIMGF()->getDialog()->GetDlgItem(1))->DeleteItem(uiTabIndex);
	for (CEditorTab *pEditorTab2 : getTabs().getEntries())
	{
		if (pEditorTab2->getIndex() > uiTabIndex)
		{
			pEditorTab2->setIndex(pEditorTab2->getIndex() - 1);
		}
	}

	// set active tab
	if (getTabs().getEntryCount() == 0)
	{
		// blank
		setActiveTab(nullptr);
	}
	else if (getTabs().getEntryByIndex(uiTabIndex) != nullptr)
	{
		// tab to right
		setActiveTab((CIMGEditorTab*)(getTabs().getEntryByIndex(uiTabIndex)));
	}
	else if (getTabs().getEntryByIndex(uiTabIndex - 1) != nullptr)
	{
		// tab to left
		setActiveTab((CIMGEditorTab*)(getTabs().getEntryByIndex(uiTabIndex - 1)));
	}
	else
	{
		// first tab
		setActiveTab((CIMGEditorTab*)(getTabs().getEntryByIndex(0)));
	}
}

void				CIMGEditor::setActiveTab(CIMGEditorTab *pEditorTab)
{
	CEditor::setActiveTab(pEditorTab);
	
	pEditorTab->readdAllEntriesToMainListView();
	
	// todo
	return;

	vector<uint16> vecButtonIds = {16,17,18,25,26,27,28,29,31,32,33,34,35,36,40,41,42,43,45,47};

	if (pEditorTab == nullptr)
	{
		/*
		todo
		getIMGF()->getTaskManager()->setTaskMaxProgressTickCount(((CListCtrl*)getIMGF()->getDialog()->GetDlgItem(37))->GetItemCount());

		// disable buttons
		for (uint16 usButtonId : vecButtonIds)
		{
			((CButton*)getIMGF()->getDialog()->GetDlgItem(usButtonId))->EnableWindow(0);
		}

		// IMG path
		((CEdit*)getIMGF()->getDialog()->GetDlgItem(38))->SetWindowTextW(_T(""));

		// IMG entry count
		((CStatic*)getIMGF()->getDialog()->GetDlgItem(20))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Window_Main_Text_EntryCount", 0).c_str());

		// selected entry count
		((CStatic*)getIMGF()->getDialog()->GetDlgItem(51))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Window_Main_Text_SelectedEntryCount", 0).c_str());

		// IMG version
		((CStatic*)getIMGF()->getDialog()->GetDlgItem(19))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedTextW("Window_Main_Text_IMGVersion").c_str());
		*/

		// 6th column name in main list view
		readdColumnsToMainListView(IMG_UNKNOWN);

		/*
		// search text
		((CEdit*)getIMGF()->getDialog()->GetDlgItem(24))->SetWindowTextW(_T(""));
		((CStatic*)getIMGF()->getDialog()->GetDlgItem(0))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Window_Main_Text_SearchResult_ActiveTab", 0).c_str());
		((CListCtrl*)getIMGF()->getDialog()->GetDlgItem(22))->DeleteAllItems();
		for (auto pSearchEntry : m_vecSearchEntries)
		{
			delete pSearchEntry;
		}
		m_vecSearchEntries.clear();

		// filter
		pEditorTab->unloadFilter_Type();
		pEditorTab->unloadFilter_Version();

		// log
		((CEdit*)getIMGF()->getDialog()->GetDlgItem(14))->SetWindowTextW(_T(""));

		// IMG entries
		//((CListCtrl*)m_pDialog->GetDlgItem(37))->DeleteAllItems();
		CListCtrl *pMainListCtrl = ((CListCtrl*)getIMGF()->getDialog()->GetDlgItem(37));
		for (uint32 i = 0, j = pMainListCtrl->GetItemCount(); i < j; i++)
		{
			pMainListCtrl->DeleteItem((j - 1) - i);
			getIMGF()->getTaskManager()->onTaskProgressTick();
		}
		*/
	}
	else
	{
		/*
		todo
		// enable buttons
		for (uint16 usButtonId : vecButtonIds)
		{
			((CButton*)getIMGF()->getDialog()->GetDlgItem(usButtonId))->EnableWindow(1);
		}

		// tab focus
		((CTabCtrl*)getIMGF()->getDialog()->GetDlgItem(1))->SetCurFocus(pEditorTab->getIndex());

		// IMG path
		((CEdit*)getIMGF()->getDialog()->GetDlgItem(38))->SetWindowTextW(CString2::convertStdStringToStdWString(pEditorTab->getIMGFile()->getFilePath()).c_str());

		// IMG entry count
		pEditorTab->updateEntryCountText();

		// selected entry count
		m_uiSelectedEntryCount = 0;
		((CStatic*)getIMGF()->getDialog()->GetDlgItem(51))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Window_Main_Text_SelectedEntryCount", 0).c_str());

		// IMG version
		pEditorTab->updateIMGText();
		*/

		// 6th column name in main list view
		readdColumnsToMainListView(pEditorTab->getIMGFile()->getIMGVersion());

		/*
		// search text
		if (((CButton*)getIMGF()->getDialog()->GetDlgItem(46))->GetCheck() == BST_UNCHECKED)
		{
			((CEdit*)getIMGF()->getDialog()->GetDlgItem(24))->SetWindowTextW(CString2::convertStdStringToStdWString(pEditorTab->getSearchText()).c_str());
			pEditorTab->searchText();
		}

		// filter
		pEditorTab->loadFilter_Type();
		pEditorTab->loadFilter_Version();

		// log
		CEdit *pEdit = ((CEdit*)getIMGF()->getDialog()->GetDlgItem(14));
		pEdit->SetWindowTextW(CString2::convertStdStringToStdWString(CString2::join(pEditorTab->getLogLinesGUI(), "\r\n")).c_str());
		pEdit->LineScroll(pEdit->GetLineCount());

		// IMG entries
		pEditorTab->readdAllEntriesToMainListView();
		*/
	}
}

void					CIMGEditor::readdColumnsToMainListView(eIMGVersion eIMGVersionValue)
{
	removeColumnsFromMainListView();
	addColumnsToMainListView(eIMGVersionValue);
}
void					CIMGEditor::addColumnsToMainListView(eIMGVersion eIMGVersionValue)
{
	CListControl *pList = (CListControl*) getControlById(37);

	pList->addHeader("ID" /* todo CLocalizationManager::getInstance()->getTranslatedText("Window_Main_ListView_ColumnTitle_ID")*/, 45);
	pList->addHeader("Type" /* CLocalizationManager::getInstance()->getTranslatedText("Window_Main_ListView_ColumnTitle_Type")*/, 40);
	pList->addHeader("Name" /* CLocalizationManager::getInstance()->getTranslatedText("Window_Main_ListView_ColumnTitle_Name")*/, 150);
	pList->addHeader("Offset" /* CLocalizationManager::getInstance()->getTranslatedText("Offset")*/, 85);
	pList->addHeader("Size" /* CLocalizationManager::getInstance()->getTranslatedText("Size")*/, 70);

	switch (eIMGVersionValue)
	{
	case IMG_UNKNOWN:
	case IMG_1:
	case IMG_2:
		pList->addHeader("Version" /* CLocalizationManager::getInstance()->getTranslatedText("Version")*/, 125);
		break;
	case IMG_3:
		pList->addHeader("Resource Type" /* CLocalizationManager::getInstance()->getTranslatedText("ResourceType")*/, 125);
		break;
	case IMG_FASTMAN92:
		pList->addHeader("Version" /* CLocalizationManager::getInstance()->getTranslatedText("Version")*/, 125);
		pList->addHeader("Compression" /* CLocalizationManager::getInstance()->getTranslatedText("Compression")*/, 125);
		pList->addHeader("Encryption" /* CLocalizationManager::getInstance()->getTranslatedText("Encryption")*/, 125);
		break;
	}
}
void					CIMGEditor::removeColumnsFromMainListView(void)
{
	/*
	todo
	CListCtrl *pListCtrl = (CListCtrl*)getIMGF()->getDialog()->GetDlgItem(37);
	while (pListCtrl->DeleteColumn(0) != 0)
	{
	}
	*/
}
int						CIMGEditor::getMainListControlItemByEntry(CIMGEntry *pIMGEntry)
{
	/*
	todo
	CListCtrl *pListControlMain = (CListCtrl*)getIMGF()->getDialog()->GetDlgItem(37);
	for (uint32 i = 0, j = pListControlMain->GetItemCount(); i < j; i++)
	{
		if (pListControlMain->GetItemData(i) == (DWORD)pIMGEntry)
		{
			return i;
		}
	}
	*/
	return 0;
}

void					CIMGEditor::onSelectIMGEntry(bool bEntryIsSelected)
{
	// todo - have onSelectIMGEntry and onUnselectIMGEntry
	if (bEntryIsSelected)
	{
		m_uiSelectedEntryCount++;
	}
	else
	{
		m_uiSelectedEntryCount--;
	}

	// selected entry count
	updateSelectedEntryCountText();
}

void					CIMGEditor::updateSelectedEntryCountText(void)
{
	// todo ((CStatic*)getIMGF()->getDialog()->GetDlgItem(51))->SetWindowTextW(CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Window_Main_Text_SelectedEntryCount", m_uiSelectedEntryCount).c_str());
}

void					CIMGEditor::logAllTabs(string strText, bool bExtendedModeOnly)
{
	for (auto pEditorTab : getTabs().getEntries())
	{
		((CIMGEditorTab*)pEditorTab)->log(strText, bExtendedModeOnly);
	}
}

void					CIMGEditor::logWithNoTabsOpen(string strText, bool bExtendedModeOnly)
{
	string strLogEntryWithTimestampAndIMG = "[" + CString2::getTimestampText() + "] [" + CLocalizationManager::getInstance()->getTranslatedText("NoTabsOpen") + "] " + strText;

	if (bExtendedModeOnly)
	{
		//m_vecLogLinesExtended.push_back(strLogEntryWithTimestampAndIMG);

		// automatic logging to file
		if (getIMGF()->getSettingsManager()->getSettingString("AutomaticLoggingPath") != "")
		{
			// extended file
			if (getIMGF()->getSettingsManager()->getSettingBool("AutomaticLoggingExtended"))
			{
				string strExtendedLogPath = CPathManager::addSlashToEnd(getIMGF()->getSettingsManager()->getSettingString("AutomaticLoggingPath"));
				strExtendedLogPath += CString2::getDateTextForFolder() + "/" + CLocalizationManager::getInstance()->getTranslatedText("LogFilename_Extended");
				CFileManager::storeFile(strExtendedLogPath, strLogEntryWithTimestampAndIMG + "\n", true, false);
			}
		}
	}
	else
	{
		//m_vecLogLinesGUI.push_back(strText);
		//m_vecLogLinesBasic.push_back(strLogEntryWithTimestampAndIMG);
		//m_vecLogLinesExtended.push_back(strLogEntryWithTimestampAndIMG);

		// automatic logging to file
		if (getIMGF()->getSettingsManager()->getSettingString("AutomaticLoggingPath") != "")
		{
			// basic file
			if (getIMGF()->getSettingsManager()->getSettingBool("AutomaticLoggingBasic"))
			{
				string strExtendedLogPath = CPathManager::addSlashToEnd(getIMGF()->getSettingsManager()->getSettingString("AutomaticLoggingPath"));
				strExtendedLogPath += CString2::getDateTextForFolder() + "/" + CLocalizationManager::getInstance()->getTranslatedText("LogFilename_Basic");
				CFileManager::storeFile(strExtendedLogPath, strLogEntryWithTimestampAndIMG + "\n", true, false);
			}

			// extended file
			if (getIMGF()->getSettingsManager()->getSettingBool("AutomaticLoggingExtended"))
			{
				string strExtendedLogPath = CPathManager::addSlashToEnd(getIMGF()->getSettingsManager()->getSettingString("AutomaticLoggingPath"));
				strExtendedLogPath += CString2::getDateTextForFolder() + "/" + CLocalizationManager::getInstance()->getTranslatedText("LogFilename_Extended");
				CFileManager::storeFile(strExtendedLogPath, strLogEntryWithTimestampAndIMG + "\n", true, false);
			}
		}
	}
}

uint32					CIMGEditor::getEntryCountForAllTabs(void)
{
	uint32 uiTotalEntryCount = 0;
	for (auto pEditorTab : getTabs().getEntries())
	{
		uiTotalEntryCount += ((CIMGEditorTab*)pEditorTab)->getIMGFile()->getEntryCount();
	}
	return uiTotalEntryCount;
}

vector<CIMGFormat*>		CIMGEditor::getAllMainWindowTabsIMGFiles(void)
{
	vector<CIMGFormat*> veCIMGFormats;
	for (auto pEditorTab : getTabs().getEntries())
	{
		veCIMGFormats.push_back(((CIMGEditorTab*)pEditorTab)->getIMGFile());
	}
	return veCIMGFormats;
}

void					CIMGEditor::refreshActiveTab(void)
{
	setActiveTab(getActiveTab());
}

void					CIMGEditor::initMenu(void) // todo - move menu stuff to like CMenuManager
{
	/*
	HMENU hMenubar = CreateMenu();

	getIMGF()->m_hMenu_File = CreateMenu();
	HMENU
		hMenu_File = getIMGF()->m_hMenu_File,
		hMenu_Edit = CreateMenu(),
		hMenu_DAT = CreateMenu(),
		hMenu_IMG = CreateMenu(),
		hMenu_Model = CreateMenu(),
		hMenu_Texture = CreateMenu(),
		hMenu_Collision = CreateMenu(),
		hMenu_ItemDefinition = CreateMenu(),
		hMenu_ItemPlacement = CreateMenu(),
		hMenu_Entry = CreateMenu(),
		hMenu_Settings = CreateMenu(),
		hMenu_Help = CreateMenu()
		;

	HMENU hMenu_File_New = CreatePopupMenu();
	HMENU hMenu_File_New_Version3 = CreatePopupMenu();
	HMENU hMenu_File_SaveLog = CreatePopupMenu();
	HMENU hMenu_File_SaveLog_ActiveTab = CreatePopupMenu();
	HMENU hMenu_File_SaveLog_AllTabs = CreatePopupMenu();
	getIMGF()->m_hSubMenu_File_OpenRecent = CreatePopupMenu();
	getIMGF()->m_hSubMenu_File_Sessions = CreatePopupMenu();
	HMENU hMenu_File_Sessions = CreateMenu();

	HMENU hMenu_Edit_Select = CreatePopupMenu();
	HMENU hMenu_Edit_SelectViaRWVersion = CreatePopupMenu();

	HMENU hMenu_IMG_ConvertTo = CreatePopupMenu();
	HMENU hMenu_IMG_ConvertTo_Version3 = CreatePopupMenu();
	HMENU hMenu_IMG_Split = CreatePopupMenu();
	HMENU hMenu_IMG_Validation = CreatePopupMenu();

	getIMGF()->m_hMenu_Entry_Sort = CreatePopupMenu();
	HMENU hMenu_Entry_Import = CreatePopupMenu();
	HMENU hMenu_Entry_Export = CreatePopupMenu();
	HMENU hMenu_Entry_Remove = CreatePopupMenu();
	HMENU hMenu_Entry_Replace = CreatePopupMenu();
	HMENU hMenu_Entry_Case = CreatePopupMenu();
	HMENU hMenu_Entry_Copy = CreatePopupMenu();
	HMENU hMenu_Entry_LowerCase = CreatePopupMenu();
	HMENU hMenu_Entry_UpperCase = CreatePopupMenu();
	HMENU hMenu_Entry_TitleCase = CreatePopupMenu();
	HMENU hMenu_Entry_Shift = CreatePopupMenu();

	HMENU hMenu_Collision_Convert = CreatePopupMenu();

	HMENU hMenu_Model_Convert = CreatePopupMenu();
	HMENU hMenu_Model_Convert_DFFRWVersion = CreatePopupMenu();
	HMENU hMenu_Model_Convert_DFFFileFormat = CreatePopupMenu();

	HMENU hMenu_Texture_Convert = CreatePopupMenu();
	HMENU hMenu_Texture_Convert_TXDGame = CreatePopupMenu();
	HMENU hMenu_Texture_Convert_TXDRWVersion = CreatePopupMenu();
	HMENU hMenu_Texture_Convert_TXDTextureFormat = CreatePopupMenu();
	HMENU hMenu_Texture_Convert_WTDFileFormat = CreatePopupMenu();

	HMENU hMenu_ItemDefinition_FindOrphanEntries = CreatePopupMenu();
	HMENU hMenu_ItemDefinition_FindMissing = CreatePopupMenu();

	HMENU hMenu_ItemPlacement_FindMissing = CreatePopupMenu();

	HMENU hMenu_Entry_Sort_Priorities[10];
	for (uint32 i = 0; i < 10; i++)
	{
		hMenu_Entry_Sort_Priorities[i] = CreatePopupMenu();
	}

	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_File, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_File").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Edit, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Edit").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_DAT, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_DAT").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_IMG, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_IMG").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Model, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Model").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Texture, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Texture").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Collision, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Collision").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_ItemDefinition, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ItemDefinition").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_ItemPlacement, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ItemPlacement").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Entry, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Entry").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Settings, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Settings").c_str());
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_Help, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Help").c_str());

	//AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu_File, _T("&File"));
	AppendMenu(hMenu_File, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_New, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NewIMG").c_str());
	AppendMenu(hMenu_File_New, MF_STRING, 1108, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_V1").c_str());
	AppendMenu(hMenu_File_New, MF_STRING, 1109, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_V2").c_str());
	AppendMenu(hMenu_File_New, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_New_Version3, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_V3").c_str());
	AppendMenu(hMenu_File_New_Version3, MF_STRING, 1112, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_V3_Encrypted").c_str());
	AppendMenu(hMenu_File_New_Version3, MF_STRING, 1113, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_V3_Unencrypted").c_str());
	AppendMenu(hMenu_File_New, MF_SEPARATOR, 1942, _T(""));
	AppendMenu(hMenu_File_New, MF_STRING, 1943, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_New_VFastman92").c_str());
	AppendMenu(hMenu_File, MF_STRING, 2084, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NewWindow").c_str());
	AppendMenu(hMenu_File, MF_SEPARATOR, 1760, _T(""));
	AppendMenu(hMenu_File, MF_STRING, 1100, CLocalizationManager::getInstance()->getTranslatedTextW("Open").c_str());
	AppendMenu(hMenu_File, MF_STRING | MF_POPUP, (UINT_PTR)getIMGF()->m_hSubMenu_File_OpenRecent, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OpenRecent").c_str());
	AppendMenu(hMenu_File, MF_STRING, 1117, CString2::convertStdStringToStdWString(CLocalizationManager::getInstance()->getTranslatedText("Menu_OpenLast") + (getIMGF()->getRecentlyOpenManager()->getLastOpenEntry() == "" ? "" : " (" + CPathManager::getFileName(getIMGF()->getRecentlyOpenManager()->getLastOpenEntry()) + ")")).c_str());
	AppendMenu(hMenu_File, MF_STRING, 1114, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Reopen").c_str());
	//AppendMenu(hMenu_File, MF_STRING, 1109, _T("Associate IMG extension"));
	AppendMenu(hMenu_File, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_Sessions, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Sessions").c_str());
	AppendMenu(hMenu_File_Sessions, MF_STRING | MF_POPUP, (UINT_PTR)getIMGF()->m_hSubMenu_File_Sessions, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_LoadSession").c_str());
	AppendMenu(hMenu_File_Sessions, MF_STRING, 1111, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_SaveSession").c_str());
	AppendMenu(hMenu_File_Sessions, MF_STRING, 1118, CLocalizationManager::getInstance()->getTranslatedTextW("SessionManager").c_str());
	AppendMenu(hMenu_File, MF_SEPARATOR, 1761, _T(""));
	AppendMenu(hMenu_File, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_SaveLog, CLocalizationManager::getInstance()->getTranslatedTextW("Log").c_str());
	AppendMenu(hMenu_File_SaveLog, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_SaveLog_ActiveTab, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab").c_str());
	AppendMenu(hMenu_File_SaveLog, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_File_SaveLog_AllTabs, CLocalizationManager::getInstance()->getTranslatedTextW("AllTabs").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1101, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_SaveBasic").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1102, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_SaveExtended").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1121, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_Clear").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1103, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_OpenFileBasic").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1104, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_OpenFileExtended").c_str());
	AppendMenu(hMenu_File_SaveLog_ActiveTab, MF_STRING, 1120, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_ActiveTab_OpenFolder").c_str());
	AppendMenu(hMenu_File_SaveLog_AllTabs, MF_STRING, 1115, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_AllTabs_SaveBasic").c_str());
	AppendMenu(hMenu_File_SaveLog_AllTabs, MF_STRING, 1116, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_AllTabs_SaveExtended").c_str());
	AppendMenu(hMenu_File_SaveLog_AllTabs, MF_STRING, 1122, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Log_AllTabs_Clear").c_str());
	AppendMenu(hMenu_File, MF_SEPARATOR, 1762, _T(""));
	AppendMenu(hMenu_File, MF_STRING, 1926, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_RepeatLastAction").c_str());
	AppendMenu(hMenu_File, MF_STRING, 1179, CLocalizationManager::getInstance()->getTranslatedTextW("RenameIMG").c_str());
	AppendMenu(hMenu_File, MF_STRING, 1752, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OpenIMGFolder").c_str());
	AppendMenu(hMenu_File, MF_SEPARATOR, 1927, _T(""));
	AppendMenu(hMenu_File, MF_STRING, 1105, CLocalizationManager::getInstance()->getTranslatedTextW("Close").c_str());
	AppendMenu(hMenu_File, MF_STRING, 1106, CLocalizationManager::getInstance()->getTranslatedTextW("CloseAll").c_str());
	AppendMenu(hMenu_File, MF_STRING, 1107, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ExitTool").c_str());

	AppendMenu(hMenu_Edit, MF_STRING, 1180, CLocalizationManager::getInstance()->getTranslatedTextW("Find").c_str());
	AppendMenu(hMenu_Edit, MF_STRING, 1181, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Find_AllTabs").c_str());
	AppendMenu(hMenu_Edit, MF_SEPARATOR, 1720, _T(""));
	AppendMenu(hMenu_Edit, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Edit_Select, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Select").c_str());
	AppendMenu(hMenu_Edit_Select, MF_STRING, 1167, CLocalizationManager::getInstance()->getTranslatedTextW("Select_All").c_str());
	AppendMenu(hMenu_Edit_Select, MF_STRING, 1168, CLocalizationManager::getInstance()->getTranslatedTextW("Select_Inverse").c_str());
	AppendMenu(hMenu_Edit_Select, MF_SEPARATOR, 1730, _T(""));
	AppendMenu(hMenu_Edit_Select, MF_STRING, 1560, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Select_Extensions").c_str());
	AppendMenu(hMenu_Edit_Select, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Edit_SelectViaRWVersion, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Select_RWVersion").c_str());
	AppendMenu(hMenu_Edit_Select, MF_STRING, 1900, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Select_IDE").c_str());
	uint32 i = 0;
	for (auto pRWVersion : CRWManager::getInstance()->getVersionManager()->getEntries())
	{
		getIMGF()->m_umapMenuItemMapping_SelectRWVersion[1561 + i] = pRWVersion;
		string strMenuText = CLocalizationManager::getInstance()->getTranslatedText("Menu_Select_RWVersion_Prefix") + " " + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(0, 2)) + "&" + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(2));
		AppendMenu(hMenu_Edit_SelectViaRWVersion, MF_STRING, 1561 + i, CString2::convertStdStringToStdWString(strMenuText).c_str());
		i++;
	}
	AppendMenu(hMenu_Edit_SelectViaRWVersion, MF_SEPARATOR, 1731, _T(""));
	AppendMenu(hMenu_Edit_SelectViaRWVersion, MF_STRING, 1568, CString2::convertStdStringToStdWString(CLocalizationManager::getInstance()->getTranslatedText("Menu_Select_RWVersion_Prefix") + " " + CLocalizationManager::getInstance()->getTranslatedText("Window_Main_Combo_RWVersion_Unknown")).c_str());
	AppendMenu(hMenu_Edit, MF_SEPARATOR, 1185, _T(""));
	AppendMenu(hMenu_Edit, MF_STRING, 1182, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import").c_str());
	AppendMenu(hMenu_Edit, MF_STRING, 1183, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Replace").c_str());
	AppendMenu(hMenu_Edit, MF_STRING, 1184, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove").c_str());

	AppendMenu(hMenu_DAT, MF_STRING, 2056, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_DATPathsMover").c_str());
	AppendMenu(hMenu_DAT, MF_STRING, 2057, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_DATModelList").c_str());

	AppendMenu(hMenu_IMG, MF_STRING, 1140, CLocalizationManager::getInstance()->getTranslatedTextW("Rebuild").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 1141, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_RebuildAs").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 1142, CLocalizationManager::getInstance()->getTranslatedTextW("RebuildAll").c_str());
	AppendMenu(hMenu_IMG, MF_SEPARATOR, 1740, _T(""));
	AppendMenu(hMenu_IMG, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_IMG_ConvertTo, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion").c_str());
	AppendMenu(hMenu_IMG_ConvertTo, MF_STRING, 1143, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion1").c_str());
	AppendMenu(hMenu_IMG_ConvertTo, MF_STRING, 1144, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion2").c_str());
	AppendMenu(hMenu_IMG_ConvertTo, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_IMG_ConvertTo_Version3, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion3").c_str());
	AppendMenu(hMenu_IMG_ConvertTo_Version3, MF_STRING, 1145, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion3Encrypted").c_str());
	AppendMenu(hMenu_IMG_ConvertTo_Version3, MF_STRING, 1146, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersion3Unencrypted").c_str());
	AppendMenu(hMenu_IMG_ConvertTo, MF_SEPARATOR, 2200, _T(""));
	AppendMenu(hMenu_IMG_ConvertTo, MF_STRING, 2201, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ConvertToVersionFastman92").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 1147, CLocalizationManager::getInstance()->getTranslatedTextW("Merge").c_str());
	AppendMenu(hMenu_IMG, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_IMG_Split, CLocalizationManager::getInstance()->getTranslatedTextW("Split").c_str());
	AppendMenu(hMenu_IMG_Split, MF_STRING, 1156, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_SplitSelected").c_str());
	AppendMenu(hMenu_IMG_Split, MF_STRING, 1157, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_SplitIDE").c_str());
	AppendMenu(hMenu_IMG_Split, MF_STRING, 1158, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_SplitTextLines").c_str());
	AppendMenu(hMenu_IMG, MF_SEPARATOR, 1741, _T(""));

	AppendMenu(hMenu_IMG, MF_STRING | MF_POPUP, (UINT_PTR)getIMGF()->m_hMenu_Entry_Sort, CLocalizationManager::getInstance()->getTranslatedTextW("Sort").c_str());
	for (uint32 i = 0; i < 10; i++)
	{
		AppendMenu(getIMGF()->m_hMenu_Entry_Sort, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Sort_Priorities[i], CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Sort_Priority_N", (i + 1)).c_str());
		uint32 i2 = 0;
		for (auto pSortType : getIMGF()->getSortManager()->getSortTypes()->getEntries())
		{
			CSortPriority *pSortPriority = getIMGF()->getSortManager()->getSortPriorities()->getEntryByIndex((uint16)i);

			uint32 uiMenuHandle = 1200 + (20 * i) + i2;
			AppendMenu(hMenu_Entry_Sort_Priorities[i], MF_STRING, uiMenuHandle, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Sort_ByText", pSortType->getTextForMenu().c_str()).c_str());
			getIMGF()->getSortManager()->getSortMenuItems()->addEntry(new CSortMenuItem(uiMenuHandle, pSortPriority, pSortType));
			i2++;
		}
	}

	AppendMenu(hMenu_IMG, MF_STRING, 2050, CLocalizationManager::getInstance()->getTranslatedTextW("Renamer").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 1155, CLocalizationManager::getInstance()->getTranslatedTextW("UpdateLST").c_str());
	AppendMenu(hMenu_IMG, MF_SEPARATOR, 1743, _T(""));
	AppendMenu(hMenu_IMG, MF_STRING, 2090, CLocalizationManager::getInstance()->getTranslatedTextW("CloneIMG").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 1153, CLocalizationManager::getInstance()->getTranslatedTextW("DuplicateEntries").c_str());
	AppendMenu(hMenu_IMG, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_IMG_Validation, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Validation").c_str());
	AppendMenu(hMenu_IMG_Validation, MF_STRING, 2002, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Validation_SaveIMGSignature").c_str());
	AppendMenu(hMenu_IMG_Validation, MF_STRING, 2003, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Validation_VerifyIMGSignature").c_str());
	AppendMenu(hMenu_IMG_Validation, MF_STRING, 2005, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Validation_DFF").c_str());
	AppendMenu(hMenu_IMG_Validation, MF_STRING, 2006, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Validation_TXD").c_str());
	AppendMenu(hMenu_IMG, MF_STRING, 2004, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Compare").c_str());
	AppendMenu(hMenu_IMG, MF_SEPARATOR, 1742, _T(""));
	AppendMenu(hMenu_IMG, MF_STRING, 1149, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Stats").c_str());
	AppendMenu(hMenu_IMG, MF_SEPARATOR, 2052, _T(""));
	AppendMenu(hMenu_IMG, MF_STRING, 2053, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_IMGVersionSettings").c_str());

	//AppendMenu(hMenu_Model, MF_STRING, 2105, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ExtractDVCAndNVCColoursIntoDFFs").c_str());
	//AppendMenu(hMenu_Model, MF_STRING, 2106, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Extract2dEffectsIntoDFFs").c_str());
	AppendMenu(hMenu_Model, MF_SEPARATOR, 2052, _T(""));
	AppendMenu(hMenu_Model, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Model_Convert, CLocalizationManager::getInstance()->getTranslatedTextW("Convert").c_str());
	AppendMenu(hMenu_Model_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Model_Convert_DFFRWVersion, CLocalizationManager::getInstance()->getTranslatedTextW("DFFRWVersion").c_str());
	AppendMenu(hMenu_Model, MF_STRING, 2081, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Model_RemoveOrphanTextures").c_str());

	i = 0;
	for (auto pRWVersion : CRWManager::getInstance()->getVersionManager()->getEntries())
	{
		if (pRWVersion->getVersionType() != TYPE_MODEL)
		{
			continue;
		}

		getIMGF()->m_umapMenuItemMapping_ConvertDFFtoRWVersion[1590 + i] = pRWVersion;
		string strMenuText = "Convert DFF to " + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(0, 2)) + "&" + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(2));
		AppendMenu(hMenu_Model_Convert_DFFRWVersion, MF_STRING, 1590 + i, CString2::convertStdStringToStdWString(strMenuText).c_str());
		i++;
	}

	//AppendMenu(hMenu_Model_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Model_Convert_FileFormat, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_FileFormat").c_str());
	//AppendMenu(hMenu_Model_Convert_FileFormat, MF_STRING, 1840, _T("Convert DFF to WDR"));

	AppendMenu(hMenu_Texture, MF_STRING, 1154, CLocalizationManager::getInstance()->getTranslatedTextW("Dump").c_str());
	AppendMenu(hMenu_Texture, MF_STRING, 2051, CLocalizationManager::getInstance()->getTranslatedTextW("BuildTXD").c_str());
	AppendMenu(hMenu_Texture, MF_STRING, 2055, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_TXDOrganizer").c_str());
	AppendMenu(hMenu_Texture, MF_SEPARATOR, 2080, _T(""));

	AppendMenu(hMenu_Texture, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Texture_Convert, CLocalizationManager::getInstance()->getTranslatedTextW("Convert").c_str());

	AppendMenu(hMenu_Texture_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Texture_Convert_TXDGame, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_TXD_Game").c_str());
	AppendMenu(hMenu_Texture_Convert_TXDGame, MF_STRING, 1650, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_III").c_str()).c_str());
	AppendMenu(hMenu_Texture_Convert_TXDGame, MF_STRING, 1651, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_VC").c_str()).c_str());
	AppendMenu(hMenu_Texture_Convert_TXDGame, MF_STRING, 1652, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_SA").c_str()).c_str());
	AppendMenu(hMenu_Texture_Convert_TXDGame, MF_STRING, 1653, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_Android_GTA").c_str()).c_str());
	AppendMenu(hMenu_Texture_Convert_TXDGame, MF_STRING, 1654, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_XBOX_GTA").c_str()).c_str());

	AppendMenu(hMenu_Texture_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Texture_Convert_TXDRWVersion, CLocalizationManager::getInstance()->getTranslatedTextW("TXDRWVersion").c_str());
	i = 0;
	for (auto pRWVersion : CRWManager::getInstance()->getVersionManager()->getEntries())
	{
		if (pRWVersion->getVersionType() != TYPE_MODEL)
		{
			continue;
		}

		getIMGF()->m_umapMenuItemMapping_ConvertTXDtoRWVersion[1660 + i] = pRWVersion;
		string strString1 = CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(0, 2)) + "&" + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(2));
		string strMenuText = CLocalizationManager::getInstance()->getTranslatedFormattedText("Menu_Convert_TXD_To", strString1.c_str());
		AppendMenu(hMenu_Texture_Convert_TXDRWVersion, MF_STRING, 1660 + i, CString2::convertStdStringToStdWString(strMenuText).c_str());
		i++;
	}

	AppendMenu(hMenu_Texture_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Texture_Convert_TXDTextureFormat, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_TXD_RasterFormat").c_str());
	i = 0;
	for (auto pRasterDataFormat : CImageManager::getInstance()->getRasterDataFormatManager()->getEntries())
	{
		getIMGF()->m_umapMenuItemMapping_ConvertTXDtoTextureFormat[2100 + i] = pRasterDataFormat;
		AppendMenu(hMenu_Texture_Convert_TXDTextureFormat, MF_STRING, 2100 + i, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", pRasterDataFormat->getTextForMenu().c_str()).c_str());
		i++;
	}

	//AppendMenu(hMenu_Texture_Convert, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Texture_Convert_WTDFileFormat, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_WTDFileFormat").c_str());
	//AppendMenu(hMenu_Texture_Convert_WTDFileFormat, MF_STRING, 2500, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_WTD_TXD").c_str());

	AppendMenu(hMenu_Texture, MF_STRING, 1150, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_TextureList").c_str());
	AppendMenu(hMenu_Texture, MF_STRING, 1620, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingTexture").c_str());

	AppendMenu(hMenu_Collision, MF_STRING, 1681, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_AlignMeshes_COL_DFF").c_str());
	AppendMenu(hMenu_Collision, MF_SEPARATOR, 1702, _T(""));

	AppendMenu(hMenu_Collision, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Collision_Convert, CLocalizationManager::getInstance()->getTranslatedTextW("Convert").c_str());

	i = 0;
	for (auto pCOLVersion : CCOLManager::getInstance()->getVersionManager()->getEntries())
	{
		getIMGF()->m_umapMenuItemMapping_ConvertCOLtoCOLVersion[2400 + i] = pCOLVersion;
		AppendMenu(hMenu_Collision_Convert, MF_STRING, 2400 + i, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_COL_COL", pCOLVersion->getText().c_str()).c_str());
		i++;
	}

	AppendMenu(hMenu_Collision, MF_STRING, 1680, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_CenterCOLMeshes").c_str());

	AppendMenu(hMenu_ItemDefinition, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ItemDefinition_FindOrphanEntries, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_FindOrphanEntries").c_str());
	AppendMenu(hMenu_ItemDefinition_FindOrphanEntries, MF_STRING, 2091, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OrphanEntries_IDE_COL").c_str());
	AppendMenu(hMenu_ItemDefinition_FindOrphanEntries, MF_STRING, 2092, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OrphanEntries_IDE_DFF").c_str());
	AppendMenu(hMenu_ItemDefinition_FindOrphanEntries, MF_STRING, 2093, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OrphanEntries_IDE_IMG").c_str());
	AppendMenu(hMenu_ItemDefinition_FindOrphanEntries, MF_STRING, 2094, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OrphanEntries_IDE_IPL").c_str());
	AppendMenu(hMenu_ItemDefinition_FindOrphanEntries, MF_STRING, 2095, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_OrphanEntries_IDE_TXD").c_str());
	AppendMenu(hMenu_ItemDefinition, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ItemDefinition_FindMissing, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_FindMissingEntries").c_str());
	AppendMenu(hMenu_ItemDefinition_FindMissing, MF_STRING, 2059, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingEntries_IDE_IMG_TXD").c_str());
	AppendMenu(hMenu_ItemDefinition_FindMissing, MF_STRING, 2060, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingEntries_IDE_COL_COL").c_str());
	AppendMenu(hMenu_ItemDefinition_FindMissing, MF_STRING, 2061, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingEntries_IDE_IMG_DFF").c_str());
	AppendMenu(hMenu_ItemDefinition, MF_STRING, 2062, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_SortDATIDEAndIPL_ByObjectId").c_str());

	AppendMenu(hMenu_ItemPlacement, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ItemPlacement_FindMissing, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_FindMissingEntries").c_str());
	AppendMenu(hMenu_ItemPlacement_FindMissing, MF_STRING, 2097, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingEntries_IPL_IDE_DFF").c_str());

	AppendMenu(hMenu_ItemPlacement, MF_STRING, 2058, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MapMoverAndIDShifter").c_str());

	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Import, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import").c_str());
	AppendMenu(hMenu_Entry_Import, MF_STRING, 1160, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import_Files").c_str());
	AppendMenu(hMenu_Entry_Import, MF_STRING, 1161, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import_IDE").c_str());
	AppendMenu(hMenu_Entry_Import, MF_STRING, 1169, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import_TextLines").c_str());
	AppendMenu(hMenu_Entry_Import, MF_STRING, 1170, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Import_Folder").c_str());
	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Export, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1162, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_Selected").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1163, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_IDE").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1350, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_IPL").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1164, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_TextLines").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1192, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_DAT").c_str());
	AppendMenu(hMenu_Entry_Export, MF_SEPARATOR, 1710, _T(""));
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1550, CLocalizationManager::getInstance()->getTranslatedTextW("QuickExport").c_str());
	AppendMenu(hMenu_Entry_Export, MF_SEPARATOR, 1711, _T(""));
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1551, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_AllTabs").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1554, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_AllTabsIntoFolders").c_str());
	AppendMenu(hMenu_Entry_Export, MF_SEPARATOR, 1712, _T(""));
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1552, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_AllTabsIDE").c_str());
	AppendMenu(hMenu_Entry_Export, MF_STRING, 1553, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_AllTabsTextLines").c_str());
	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Remove, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove").c_str());
	AppendMenu(hMenu_Entry_Remove, MF_STRING, 1165, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove_Selected").c_str());
	AppendMenu(hMenu_Entry_Remove, MF_STRING, 1177, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove_IDE").c_str());
	AppendMenu(hMenu_Entry_Remove, MF_STRING, 1178, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove_TextLines").c_str());
	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Replace, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Replace").c_str());
	AppendMenu(hMenu_Entry_Replace, MF_STRING, 1176, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Replace_Selected").c_str());
	AppendMenu(hMenu_Entry_Replace, MF_STRING, 1621, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Replace_Folder").c_str());
	AppendMenu(hMenu_Entry, MF_SEPARATOR, 1700, _T(""));

	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Shift, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift").c_str());
	AppendMenu(hMenu_Entry_Shift, MF_STRING, 1540, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift_Up").c_str());
	AppendMenu(hMenu_Entry_Shift, MF_STRING, 1541, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift_Down").c_str());
	AppendMenu(hMenu_Entry, MF_STRING, 1166, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Rename").c_str());
	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Case, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase").c_str());
	AppendMenu(hMenu_Entry_Case, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_LowerCase, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Lowercase").c_str());
	AppendMenu(hMenu_Entry_LowerCase, MF_STRING, 1500, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_Entry_LowerCase, MF_STRING, 1501, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_Entry_LowerCase, MF_STRING, 1502, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu_Entry_Case, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_UpperCase, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Uppercase").c_str());
	AppendMenu(hMenu_Entry_UpperCase, MF_STRING, 1510, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_Entry_UpperCase, MF_STRING, 1511, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_Entry_UpperCase, MF_STRING, 1512, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu_Entry_Case, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_TitleCase, CLocalizationManager::getInstance()->getTranslatedTextW("Titlecase").c_str());
	AppendMenu(hMenu_Entry_TitleCase, MF_STRING, 1520, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_Entry_TitleCase, MF_STRING, 1521, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_Entry_TitleCase, MF_STRING, 1522, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu_Entry, MF_SEPARATOR, 1701, _T(""));
	AppendMenu(hMenu_Entry, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Entry_Copy, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1530, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_ID").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1531, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_Name").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1532, CLocalizationManager::getInstance()->getTranslatedTextW("Offset").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1533, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_ResourceType").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1534, CLocalizationManager::getInstance()->getTranslatedTextW("RWVersion").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1535, CLocalizationManager::getInstance()->getTranslatedTextW("Size").c_str());
	AppendMenu(hMenu_Entry_Copy, MF_STRING, 1536, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_Type").c_str());
	//AppendMenu(hMenu_Entry, MF_SEPARATOR, 1703, _T(""));

	AppendMenu(hMenu_Settings, MF_STRING, 1580, CLocalizationManager::getInstance()->getTranslatedTextW("Settings").c_str());

	AppendMenu(hMenu_Help, MF_STRING, 1570, CLocalizationManager::getInstance()->getTranslatedTextW("Version").c_str());
	AppendMenu(hMenu_Help, MF_STRING, 1572, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Update").c_str());
	AppendMenu(hMenu_Help, MF_SEPARATOR, 1574, _T(""));
	AppendMenu(hMenu_Help, MF_STRING, 1571, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Website").c_str());
	AppendMenu(hMenu_Help, MF_STRING, 1575, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_ReportIssueOrIdea").c_str());
	AppendMenu(hMenu_Help, MF_SEPARATOR, 1574, _T(""));
	AppendMenu(hMenu_Help, MF_STRING, 1573, CLocalizationManager::getInstance()->getTranslatedTextW("Credits").c_str());

	::SetMenu(getIMGF()->getDialog()->GetSafeHwnd(), hMenubar);
	*/
}

void					CIMGEditor::loadRightClickMenu(int xPos, int yPos)
{
	/*
	todo
	CListCtrl *pListCtrl = (CListCtrl*)getIMGF()->getDialog()->GetDlgItem(37);

	RECT rect;
	pListCtrl->GetWindowRect(&rect);
	getIMGF()->getDialog()->ScreenToClient(&rect);

	POINT point;
	point.x = xPos;
	point.y = yPos;
	ScreenToClient(getIMGF()->getDialog()->GetSafeHwnd(), &point);
	if (point.x >= rect.left &&
		point.x <= rect.right &&
		point.y >= rect.top &&
		point.y <= rect.bottom)
	{
	}
	else
	{
		// cursor right click was not in the main list view
		return;
	}

	// create menu
	HMENU hMenu = CreatePopupMenu();

	// create popup menus
	HMENU hMenu_Shift = CreatePopupMenu();
	HMENU hMenu_NameCase = CreatePopupMenu();
	HMENU hMenu_NameCase_LowerCase = CreatePopupMenu();
	HMENU hMenu_NameCase_UpperCase = CreatePopupMenu();
	HMENU hMenu_NameCase_TitleCase = CreatePopupMenu();
	HMENU hMenu_Copy = CreatePopupMenu();

	HMENU hMenu_ConvertDFF = CreatePopupMenu();
	HMENU hMenu_ConvertDFF_DFFRWVersion = CreatePopupMenu();

	HMENU hMenu_ConvertTXD = CreatePopupMenu();
	HMENU hMenu_ConvertTXD_TXDGame = CreatePopupMenu();
	HMENU hMenu_ConvertTXD_TXDRWVersion = CreatePopupMenu();
	HMENU hMenu_ConvertTXD_TXDTextureFormat = CreatePopupMenu();
	HMENU hMenu_ConvertTXD_WTDFileFormat = CreatePopupMenu();

	HMENU hMenu_ConvertCOL = CreatePopupMenu();

	// IMG Entry
	uint32 i;

	AppendMenu(hMenu, MF_STRING, 1550, CLocalizationManager::getInstance()->getTranslatedTextW("QuickExport").c_str());
	AppendMenu(hMenu, MF_STRING, 1162, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Export_Selected").c_str());
	AppendMenu(hMenu, MF_STRING, 1165, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Remove_Selected").c_str());
	AppendMenu(hMenu, MF_STRING, 1176, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Replace_Selected").c_str());
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Shift, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift").c_str());
	AppendMenu(hMenu_Shift, MF_STRING, 1540, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift_Up").c_str());
	AppendMenu(hMenu_Shift, MF_STRING, 1541, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Shift_Down").c_str());
	AppendMenu(hMenu, MF_STRING, 1166, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Rename").c_str());
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_NameCase, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase").c_str());
	AppendMenu(hMenu_NameCase, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_NameCase_LowerCase, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Lowercase").c_str());
	AppendMenu(hMenu_NameCase_LowerCase, MF_STRING, 1500, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_NameCase_LowerCase, MF_STRING, 1501, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_NameCase_LowerCase, MF_STRING, 1502, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu_NameCase, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_NameCase_UpperCase, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Uppercase").c_str());
	AppendMenu(hMenu_NameCase_UpperCase, MF_STRING, 1510, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_NameCase_UpperCase, MF_STRING, 1511, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_NameCase_UpperCase, MF_STRING, 1512, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu_NameCase, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_NameCase_TitleCase, CLocalizationManager::getInstance()->getTranslatedTextW("Titlecase").c_str());
	AppendMenu(hMenu_NameCase_TitleCase, MF_STRING, 1520, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithExtension").c_str());
	AppendMenu(hMenu_NameCase_TitleCase, MF_STRING, 1521, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_NameWithoutExtension").c_str());
	AppendMenu(hMenu_NameCase_TitleCase, MF_STRING, 1522, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_NameCase_Extension").c_str());
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_Copy, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1530, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_ID").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1531, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_Name").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1532, CLocalizationManager::getInstance()->getTranslatedTextW("Offset").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1533, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_ResourceType").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1534, CLocalizationManager::getInstance()->getTranslatedTextW("RWVersion").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1535, CLocalizationManager::getInstance()->getTranslatedTextW("Size").c_str());
	AppendMenu(hMenu_Copy, MF_STRING, 1536, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Copy_Type").c_str());

	// DFF Entry
	AppendMenu(hMenu, MF_SEPARATOR, 1701, _T(""));

	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertDFF, CLocalizationManager::getInstance()->getTranslatedTextW("ConvertModel").c_str());
	AppendMenu(hMenu_ConvertDFF, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertDFF_DFFRWVersion, CLocalizationManager::getInstance()->getTranslatedTextW("DFFRWVersion").c_str());
	AppendMenu(hMenu, MF_STRING, 2081, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Model_RemoveOrphanTextures").c_str());

	i = 0;
	for (auto pRWVersion : CRWManager::getInstance()->getVersionManager()->getEntries())
	{
		if (pRWVersion->getVersionType() != TYPE_MODEL)
		{
			continue;
		}

		string strMenuText = "Convert DFF to " + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(0, 2)) + "&" + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(2));
		AppendMenu(hMenu_ConvertDFF_DFFRWVersion, MF_STRING, 1590 + i, CString2::convertStdStringToStdWString(strMenuText).c_str());
		i++;
	}

	// TXD Entry
	AppendMenu(hMenu, MF_SEPARATOR, 1701, _T(""));

	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertTXD, CLocalizationManager::getInstance()->getTranslatedTextW("ConvertTexture").c_str());

	AppendMenu(hMenu_ConvertTXD, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertTXD_TXDGame, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_TXD_Game").c_str());
	AppendMenu(hMenu_ConvertTXD_TXDGame, MF_STRING, 1650, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_III").c_str()).c_str());
	AppendMenu(hMenu_ConvertTXD_TXDGame, MF_STRING, 1651, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_VC").c_str()).c_str());
	AppendMenu(hMenu_ConvertTXD_TXDGame, MF_STRING, 1652, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_PC_GTA_SA").c_str()).c_str());
	AppendMenu(hMenu_ConvertTXD_TXDGame, MF_STRING, 1653, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_Android_GTA").c_str()).c_str());
	AppendMenu(hMenu_ConvertTXD_TXDGame, MF_STRING, 1654, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", CLocalizationManager::getInstance()->getTranslatedFormattedText("PlatformedGame_XBOX_GTA").c_str()).c_str());

	AppendMenu(hMenu_ConvertTXD, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertTXD_TXDRWVersion, CLocalizationManager::getInstance()->getTranslatedTextW("TXDRWVersion").c_str());
	i = 0;
	for (auto pRWVersion : CRWManager::getInstance()->getVersionManager()->getEntries())
	{
		if (pRWVersion->getVersionType() != TYPE_MODEL)
		{
			continue;
		}

		string strString1 = CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(0, 2)) + "&" + CString2::escapeMenuText((pRWVersion->getVersionName() + " (" + CLocalizationManager::getInstance()->getTranslatedText(pRWVersion->getLocalizationKey()) + ")").substr(2));
		string strMenuText = CLocalizationManager::getInstance()->getTranslatedFormattedText("Menu_Convert_TXD_To", strString1.c_str());
		AppendMenu(hMenu_ConvertTXD_TXDRWVersion, MF_STRING, 1660 + i, CString2::convertStdStringToStdWString(strMenuText).c_str());
		i++;
	}

	AppendMenu(hMenu_ConvertTXD, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertTXD_TXDTextureFormat, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_TXD_RasterFormat").c_str());
	i = 0;
	for (auto pRasterDataFormat : CImageManager::getInstance()->getRasterDataFormatManager()->getEntries())
	{
		AppendMenu(hMenu_ConvertTXD_TXDTextureFormat, MF_STRING, 2100 + i, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_TXD_To", pRasterDataFormat->getTextForMenu().c_str()).c_str());
		i++;
	}

	//AppendMenu(hMenu_ConvertTXD, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertTXD_WTDFileFormat, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_WTDFileFormat").c_str());
	//AppendMenu(hMenu_ConvertTXD_WTDFileFormat, MF_STRING, 2500, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_Convert_WTD_TXD").c_str());

	AppendMenu(hMenu, MF_STRING, 1150, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_TextureList").c_str());
	AppendMenu(hMenu, MF_STRING, 1620, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_MissingTexture").c_str());

	// COL Entry
	AppendMenu(hMenu, MF_SEPARATOR, 1701, _T(""));

	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMenu_ConvertCOL, CLocalizationManager::getInstance()->getTranslatedTextW("ConvertCollision").c_str());

	i = 0;
	for (auto pCOLVersion : CCOLManager::getInstance()->getVersionManager()->getEntries())
	{
		AppendMenu(hMenu_ConvertCOL, MF_STRING, 2400 + i, CLocalizationManager::getInstance()->getTranslatedFormattedTextW("Menu_Convert_COL_COL", pCOLVersion->getText().c_str()).c_str());
		i++;
	}

	AppendMenu(hMenu, MF_STRING, 1680, CLocalizationManager::getInstance()->getTranslatedTextW("Menu_CenterCOLMeshes").c_str());

	// show menu
	SetForegroundWindow(getIMGF()->getDialog()->GetSafeHwnd());
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, xPos, yPos, 0, getIMGF()->getDialog()->GetSafeHwnd(), NULL);
	PostMessage(getIMGF()->getDialog()->GetSafeHwnd(), WM_NULL, 0, 0);

	// clean up
	//DestroyMenu(hMenu);
	*/
}

void		CIMGEditor::initWindow(void)
{
	uint32 i, i2, x, y, w, h, sw, sh, sw2, sh2;

	// all buttons
	CButtonControl *pButton, *pOpenButton;

	w = 100;
	h = 20;

	CGUIStyles *pButtonStyles = gui::CGUIManager::createStyles();
	pButtonStyles->setStyle("fill-colour", CColour(240, 240, 240));
	pButtonStyles->setStyle("border-colour", CColour(140, 140, 140));
	pButtonStyles->setStyle("text-align-x", string("left"));
	pButtonStyles->setStyle("text-align-y", string("center"));
	pButtonStyles->setStyle("inner-spacing-left", (int32) 8);

	// vertical buttons
	i = 0;
	i2 = 0;
	x = 10;
	y = 25 + 19 + 10 + h + 10;
	sh = 10; // spacing height
	sh2 = 30; // spacing height 2
	
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Open", pButtonStyles);
	pOpenButton = pButton;
	pButton->setControlId(100);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Close", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Close All", pButtonStyles);

	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * ++i2))), CSize2D(w, h), "Import", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Export", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Quick Export", pButtonStyles);

	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * ++i2))), CSize2D(w, h), "Remove", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Rename", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Replace", pButtonStyles);

	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * ++i2))), CSize2D(w, h), "Select All", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Select Inverse", pButtonStyles);

	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * ++i2))), CSize2D(w, h), "Sort", pButtonStyles);
	pButton = addButton(CPoint2D((int32) x, (int32) (y + (h * i) + (sh * i++) + (sh2 * i2))), CSize2D(w, h), "Run LST", pButtonStyles);

	/*
	getWindow()->storeEventBoundFunction(getWindow()->bindEvent(EVENT_onLeftMouseUp, [](void *pControl, void *pTriggerArg)
	{
		if (!pControl) return;
		//if (((CGUIItem*) pControl)->getType() != GUI_ITEM_CONTROL) return;
		if (((CGUIControl*) pControl)->getControlId() == 100)
		{
			getIMGF()->getTaskManager()->getDispatch()->onRequestOpen();
		}
	}, pOpenButton));
	*/

	pOpenButton->storeEventBoundFunction(pOpenButton->bindEvent(EVENT_onLeftMouseUp, [](void *a,void*b){ getIMGF()->getTaskManager()->getDispatch()->onRequestOpen(); }));

	// horizontal buttons
	i = 0;
	i2 = 0;
	x = 10 + w + 10;
	y = 25 + 19 + 10;
	sw = 10; // spacing width
	sw2 = 30; // spacing width 2

	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * i2)), (int32) y), CSize2D(w, h), "Rebuild", pButtonStyles);
	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * i2)), (int32) y), CSize2D(w, h), "Rebuild As", pButtonStyles);
	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * i2)), (int32) y), CSize2D(w, h), "Rebuild All", pButtonStyles);

	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * ++i2)), (int32) y), CSize2D(w, h), "Merge", pButtonStyles);
	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * i2)), (int32) y), CSize2D(w, h), "Split", pButtonStyles);
	pButton = addButton(CPoint2D((int32) (x + (w * i) + (sw * i++) + (sw2 * i2)), (int32) y), CSize2D(w, h), "Convert", pButtonStyles);

	uint32 uiButtonWidth = w;
	uint32 uiButtonHeight = h;



	// IMG instance tab bar
	x = 10 + uiButtonWidth + 10;
	y = 25 + 19 + 10 + uiButtonHeight + 10;
	w = 500;
	h = 20;

	int32 iIMGInstanceTabBarHeight = h;

	CGUIStyles *pIMGInstanceTabBarStyles = gui::CGUIManager::createStyles();
	//pIMGInstanceTabBarStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pIMGInstanceTabBarStyles->setStyle("border-colour", CColour(0, 0, 0));
	pIMGInstanceTabBarStyles->setStyle("text-align-x", string("left"));
	pIMGInstanceTabBarStyles->setStyle("text-align-y", string("center"));
	pIMGInstanceTabBarStyles->setStyle("inner-spacing-x", (int32) 8);

	CTabControl *pIMGInstanceTabBar = addTabBar(CPoint2D(x, y), CSize2D(w, h), pIMGInstanceTabBarStyles);

	pIMGInstanceTabBar->addTab("IMG1.img", true);


	// filter bar: search box
	x = 10 + uiButtonWidth + 10;
	y = 25 + 19 + 10 + uiButtonHeight + 10 + iIMGInstanceTabBarHeight + 10;
	w = 400;
	h = 25;

	CGUIStyles *pSearchBoxStyles = gui::CGUIManager::createStyles();
	pSearchBoxStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pSearchBoxStyles->setStyle("text-colour", CColour(0, 0, 0));
	pSearchBoxStyles->setStyle("text-align-x", string("left"));
	pSearchBoxStyles->setStyle("text-align-y", string("center"));
	pSearchBoxStyles->setStyle("inner-spacing-x", (int32)8);

	CEditControl *pSearchBox = addEdit(CPoint2D(x, y), CSize2D(w, h), "Search", false, pSearchBoxStyles);



	// filter bar: entry type
	x = 10 + uiButtonWidth + 10 + 400 + 10;
	y = 25 + 19 + 10 + uiButtonHeight + 10 + iIMGInstanceTabBarHeight + 10;
	w = 140;
	h = 25;

	CGUIStyles *pEntryTypeFilterStyles = gui::CGUIManager::createStyles();
	pEntryTypeFilterStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pEntryTypeFilterStyles->setStyle("text-colour", CColour(0, 0, 0));
	pEntryTypeFilterStyles->setStyle("text-align-x", string("left"));
	pEntryTypeFilterStyles->setStyle("text-align-y", string("center"));
	pEntryTypeFilterStyles->setStyle("inner-spacing-x", (int32) 8);

	CDropControl *pEntryTypeFilter = addDrop(CPoint2D(x, y), CSize2D(w, h), pEntryTypeFilterStyles);
	pEntryTypeFilter->addItem("Entry Type", true);



	// filter bar: entry version
	x = 10 + uiButtonWidth + 10 + 400 + 10 + w + 10;
	y = 25 + 19 + 10 + uiButtonHeight + 10 + iIMGInstanceTabBarHeight + 10;
	w = 240;
	h = 25;

	CGUIStyles *pEntryVersionFilterStyles = gui::CGUIManager::createStyles();
	pEntryVersionFilterStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pEntryVersionFilterStyles->setStyle("text-colour", CColour(0, 0, 0));
	pEntryVersionFilterStyles->setStyle("text-align-x", string("left"));
	pEntryVersionFilterStyles->setStyle("text-align-y", string("center"));
	pEntryVersionFilterStyles->setStyle("inner-spacing-x", (int32) 8);

	CDropControl *pEntryVersionFilter = addDrop(CPoint2D(x, y), CSize2D(w, h), pEntryVersionFilterStyles);
	pEntryVersionFilter->addItem("Entry Version", true);



	// entry list
	x = 10 + uiButtonWidth + 10;
	y = 25 + 19 + 10 + uiButtonHeight + 10 + 25 + 10 + iIMGInstanceTabBarHeight + 10;
	w = 800;
	h = 450;

	uint32 uiEntryListWidth = w;
	uint32 uiEntryListHeight = h;

	CGUIStyles *pEntryListStyles = gui::CGUIManager::createStyles();
	pEntryListStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pEntryListStyles->setStyle("list-header-row.fill-colour", CColour(250, 255, 235));
	pEntryListStyles->setStyle("list-header-cell.text-align-x", string("left"));
	pEntryListStyles->setStyle("list-header-cell.text-align-y", string("center"));
	pEntryListStyles->setStyle("list-header-cell.inner-spacing-x", (int32) 8);

	CListControl *pEntryListControl = addList(CPoint2D((int32) x, (int32) y), CSize2D(w, h), pEntryListStyles);
	pEntryListControl->setControlId(37);
	setEntryListControl(pEntryListControl);

	int32 uiEntryListY = y = 25 + 19 + 10 + uiButtonHeight + 10 + 25 + 10;



	// IMG stats
	x = 10 + uiButtonWidth + 10;
	y = uiEntryListY + uiEntryListHeight + 10 + iIMGInstanceTabBarHeight + 10;
	w = 100;
	h = 20;
	addText(CPoint2D((int32) x, (int32) y), CSize2D(w, h), "IMG Entries: 0");

	x = 10 + uiButtonWidth + 10 + 200;
	y = uiEntryListY + uiEntryListHeight + 10 + iIMGInstanceTabBarHeight + 10;
	w = 100;
	h = 20;
	addText(CPoint2D((int32) x, (int32) y), CSize2D(w, h), "IMG Version: 2 (GTA SA)");



	// progress bar
	CGUIStyles *pProgressBarStyles = gui::CGUIManager::createStyles();
	pProgressBarStyles->setStyle("fill-colour", CColour(255, 255, 255));
	pProgressBarStyles->setStyle("border-colour", CColour(0, 0, 0));

	w = 100;
	h = 20;
	x = (10 + uiButtonWidth + 10 + uiEntryListWidth) - w;
	y = uiEntryListY + uiEntryListHeight + 10 + iIMGInstanceTabBarHeight + 10;
	addProgress(CPoint2D((int32) x, (int32) y), CSize2D(w, h), pProgressBarStyles);



	// IMG path
	x = 10 + uiButtonWidth + 10;
	y = uiEntryListY + uiEntryListHeight + 10 + iIMGInstanceTabBarHeight + 10 + 25;
	w = 100;
	h = 20;
	addText(CPoint2D((int32) x, (int32) y), CSize2D(w, h), "c:\\blah\\blah.img");



	// init
	addColumnsToMainListView(IMG_UNKNOWN);
}

void		CIMGEditor::render(void)
{
}