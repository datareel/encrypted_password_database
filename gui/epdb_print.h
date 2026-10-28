// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: epdb_print.h
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ---------- Include File Description and Details  ---------- //
// ----------------------------------------------------------- //
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
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

10/27/2026: Printing, replacing the mswPrintout and Page Setup code.

The Print dialog picks the layout (one card per record, or a table),
the records (all, the search results or the selected rows), the fields
(the passwords are left out unless ticked), the paper, orientation and
the four margins, and the header and footer. Its settings are kept for
each database in print_settings.txt next to the config file.

The printout measures everything on the device it prints to, after
the system print dialog, and plans the pages once, so the preview and
the paper match. Logical units are screen pixels mapped to the paper
(wxPrintout::MapScreenSizeToPaper), so fonts have their point size on
paper.
*/
// ----------------------------------------------------------- //
#ifndef __EPDB_PRINT_HPP__
#define __EPDB_PRINT_HPP__

#include <vector>
#include <memory>
#include <wx/print.h>
#include <wx/printdlg.h>

enum { EPDB_PRINT_CARDS = 0, EPDB_PRINT_TABLE };
enum { EPDB_PRINT_ALL = 0, EPDB_PRINT_SEARCH, EPDB_PRINT_SELECTED };

struct EPDBPrintSettings
{
  EPDBPrintSettings();
  void FromDatabase(const gxDatabaseConfig &cfg); // The 2026.103 page settings
  int Load(const wxString &db_path);              // 1 if saved settings were found
  void Save(const wxString &db_path) const;
  int PrintsPasswords() const;

  int layout;    // EPDB_PRINT_CARDS or EPDB_PRINT_TABLE
  int records;   // EPDB_PRINT_ALL, _SEARCH or _SELECTED
  int field[NumDataMembers];
  int paper;     // wxPaperSize
  int landscape;
  int margin_mm[4]; // Left, top, right, bottom
  int header;       // Database name across the top
  int info_line;    // Record count, search and the passwords note
  int date;         // Print date in the header
  int page_numbers; // "Page n of N" at the bottom
  int shading;      // Shade every other table row
};

struct EPDBPrintRecord
{
  wxString value[NumDataMembers];
};

struct EPDBPrintPlan; // The planned pages, shared by the copies of a job

// What is printed, read from the database once
struct EPDBPrintJob
{
  std::shared_ptr<EPDBPrintPlan> plan; // The preview and its Print button share one plan
  EPDBPrintSettings settings;
  std::vector<EPDBPrintRecord> records;
  wxString label[NumDataMembers];
  wxString title;       // Header text
  wxString info;        // Record count, search, passwords note
  wxString date;
};

class EPDBPrintout : public wxPrintout
{
public:
  EPDBPrintout(const EPDBPrintJob &j);

  virtual void OnPreparePrinting() wxOVERRIDE;
  virtual bool OnPrintPage(int page) wxOVERRIDE;
  virtual bool HasPage(int page) wxOVERRIDE;
  virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) wxOVERRIDE;

  int NumPages() const { return (int)pages.size(); }

private:
  struct Line {
    int rec;          // Record index
    int kind;         // 0 title, 1 field line, 2 continued title
    int field;        // Field number of a field line
    int first;        // First line of the field (the label is drawn)
    wxString text;
    std::vector<wxString> cells; // Table: the text of each column
  };
  struct Item {
    int kind;         // 0 card line, 1 table header, 2 table row lines, 3 card rule
    int rec;
    int first, count; // Lines of a table row
    double y, h;
    Line line;
  };
  struct Page { std::vector<Item> items; };
public:
  struct Plan {
    std::vector<Page> pages;
    std::vector<int> columns;
    std::vector<double> col_x, col_w;
    std::vector<std::vector<std::vector<wxString> > > table_cells;
    double label_w, line_h, width, height;
    int valid;
    Plan() { label_w = line_h = width = height = 0; valid = 0; }
  };
private:

  void SetUpDC(wxDC *dc);
  void MakeFonts(wxDC *dc);
  double MM(double mm, int vertical) const;
  void Wrap(wxDC *dc, const wxString &text, double width, std::vector<wxString> &out);
  void PlanCards(wxDC *dc);
  void PlanTable(wxDC *dc);
  void DrawHeader(wxDC *dc, int page);
  double ContentTop() const { return content_top; }

  EPDBPrintJob job;
  std::vector<Page> pages;
  std::vector<int> columns;     // Table: the fields printed
  std::vector<double> col_x, col_w;
  std::vector<std::vector<std::vector<wxString> > > table_cells; // Row, column, lines
  wxFont body_font, bold_font, title_font, small_font, header_font;
  double line_h, title_h, small_h, header_h;
  double left, top, right, bottom; // Logical page area inside the margins
  double content_top, content_bottom;
  double label_w;
  double ppmm_x, ppmm_y;
  int planned;
};

class CryptDBDocument;

// The Print dialog. Returns wxID_CANCEL, wxID_PREVIEW or wxID_PRINT and
// fills job when the user printed or previewed.
int EPDBShowPrintDialog(wxWindow *parent, CryptDBDocument *doc, EPDBPrintJob &job);

// Prints or previews the job (preview = 1)
void EPDBRunPrint(wxWindow *parent, CryptDBDocument *doc, EPDBPrintJob &job, int preview);

// Builds a job with the saved settings, for Print Preview from the menu
int EPDBMakePrintJob(CryptDBDocument *doc, EPDBPrintJob &job, const EPDBPrintSettings &s);

struct EPDBPrintPlan { EPDBPrintout::Plan p; };

#endif // __EPDB_PRINT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
