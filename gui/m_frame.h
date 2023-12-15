// ------------------------------- //
// -------- Start of File -------- //
// -------------------------------s //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  
USA

Main frame for this application.
*/
// ----------------------------------------------------------- // 
#ifndef __M_FRAME_HPP__
#define __M_FRAME_HPP__

#include "app_defs.h"

class MainFrame : public wxMDIParentFrame
{
public:
  MainFrame(wxWindow *parent, const wxWindowID id, 
	    const wxString& title,
            const wxPoint& pos, const wxSize& size,
  	    const long style);
  
public: // Event handlers   
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

  // File menu functions event handlers
  void OnNew(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnImport(wxCommandEvent& event);
  void OnExport(wxCommandEvent& event);
  void OnTemplate(wxCommandEvent& event);
  void OnBackup(wxCommandEvent& event);
  void OnMerge(wxCommandEvent& event);
  void OnCompare(wxCommandEvent& event);
  void OnRebuild(wxCommandEvent& event);
  void OnRebuildConfig(wxCommandEvent& event);
  void OnStats(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);

  // Edit menu functions
#ifdef __USE_CLIPBOARD_FUNCTIONS__
  void OnCopy(wxCommandEvent& event);
  void OnCut(wxCommandEvent& event);
  void OnPaste(wxCommandEvent& event);
#endif
  void OnPreferences(wxCommandEvent& event);

  // Database function event handlers
  void OnAddObject(wxCommandEvent& event);
  void OnDeleteObject(wxCommandEvent& event);
  void OnChangeObject(wxCommandEvent& event);
  void OnFindObject(wxCommandEvent& event);
  void OnDisplay(wxCommandEvent& event);

  // View menu
  void OnClear(wxCommandEvent& event);
  void OnCloseStatusWin(wxCommandEvent& event);
  void OnStatus(wxCommandEvent& event);
  void OnToggleToolbar(wxCommandEvent& event);
  void ToggleRowLabels(wxCommandEvent& event);
  void ToggleColLabels(wxCommandEvent& event);
  void AutoSize(wxCommandEvent&);
  void DisableAutoSize(wxCommandEvent&);
  void CellOverflow(wxCommandEvent&);
  void ToggleGridLines(wxCommandEvent&);
  void SetGridLineColor(wxCommandEvent&);
  void SetSkipLineColor(wxCommandEvent&);
  void ViewSkipLines(wxCommandEvent&);
  void SetGridBackgroundColor(wxCommandEvent&);
  void SetGridTextColor(wxCommandEvent&);
  void SetGridLabelBackgroundColor(wxCommandEvent&);
  void SetGridLabelTextColor(wxCommandEvent&);
  void SetHyperlinkColor(wxCommandEvent&);
  void SetGridLabelFont(wxCommandEvent&);
  void SetGridTextFont(wxCommandEvent&);
  void SetHyperlinkFont(wxCommandEvent&);

  // Print menu function event handlers
#ifdef __USE_APP_TEXT_PRINTING__
  void OnASPrint(wxCommandEvent& event);
#endif

#ifdef __USE_APP_HTM_PRINTING__
  void OnHTMPrint(wxCommandEvent& event);
#endif
  
#ifdef __USE_MSW_PRINTING__
  void OnMSWPrint(wxCommandEvent& event);
  void OnMSWPrintPreview(wxCommandEvent& event);
  void OnMSWPrintPageSetup(wxCommandEvent& event);
  void OnMSWPrintSetup(wxCommandEvent& event);

  // Helper functions
  int MSWTestPaperSize();
  void MSWPrint();
  void MSWPrintPreview();
#endif

#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  void OnPSPrint(wxCommandEvent& event);
  void OnPSPrintPageSetup(wxCommandEvent& event);
  void OnPSPrintSetup(wxCommandEvent& event);
#endif

  // Help menu functions event handlers
  void OnAbout(wxCommandEvent& event);

public: // Public member functions
  CryptDBDocument *ActiveChild() { return set_active_child(); }
  int DatabaseIsOpen(gxString &dbname);

public: // Font, Pen, and Cursor objects
  wxFont *labelFont;
  wxFont *itemFont;
  wxFont *textWindowFont;
  wxFont *hyperlinkFont;
  wxColour *gridlineColor;
  wxColour *cellFontColor;
  wxColour *textreadonlyColor;  
  wxColour *hyperlinkColor;

public: // Public data members
  wxTextCtrl *statusWin;
  StatusFrame *sframe;

  // Database panels
  AddPanel *apanel;
  StatusPanel *spanel;
  ChangePanel *cpanel;
  DisplayPanel *dpanel;
  FindPanel *fpanel;
  NewDatabasePanel *new_db_panel;

  // Tabbed panel functions
  EditTabPanel *e_tab_panel;

  OpenDatabasePanel *open_db_panel;

#ifdef __USE_MSW_PRINTING__
  MSWPagePanel *mswpage;
#endif
  
#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  PSPagePanel *pspage;
  LinePrinterPanel *lppanel;
#endif

public:  // Toolbar functions
  void RecreateToolbar();

private: // Toolbar data members
  int m_smallToolbar;
  int m_horzToolbar;
  size_t m_rows;
  size_t m_nPrint;
  wxToolBar *m_tbar;


private: // Internal processing functions
  void InitStatusWin();

private: // Private data members
  CryptDBDocument *get_active_child();
  CryptDBDocument *set_active_child();

private:
  DECLARE_EVENT_TABLE()
};

// Standalone functions
MainFrame *GetMainFrame(void);

#endif // __M_FRAME_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
