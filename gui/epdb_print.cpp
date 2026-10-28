// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
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

10/27/2026: Printing, see epdb_print.h
*/
// ----------------------------------------------------------- //
#include "app_defs.h"
#include <algorithm>
#include <cmath>
#include <wx/spinctrl.h>

// ----------------------------------------------------------- //
// Settings
// ----------------------------------------------------------- //
static const wxPaperSize epdb_papers[] = { wxPAPER_LETTER, wxPAPER_LEGAL, wxPAPER_A4, wxPAPER_A3, wxPAPER_TABLOID };
static const char *epdb_paper_names[] = { "Letter", "Legal", "A4", "A3", "Tabloid" };
static const int epdb_num_papers = 5;

EPDBPrintSettings::EPDBPrintSettings()
{
  layout = EPDB_PRINT_CARDS;
  records = EPDB_PRINT_ALL;
  for(int i = 0; i < NumDataMembers; i++) field[i] = IsPasswordField(i) ? 0 : 1;
  paper = wxPAPER_LETTER;
  landscape = 0;
  for(int i = 0; i < 4; i++) margin_mm[i] = 13;
  header = info_line = date = page_numbers = 1;
  shading = 1;
}

void EPDBPrintSettings::FromDatabase(const gxDatabaseConfig &cfg_in)
// The page settings kept in the database by 2026.103 and earlier. The
// passwords stay off: printing them is chosen in the Print dialog.
{
  gxDatabaseConfig &cfg = (gxDatabaseConfig &)cfg_in;
  for(int i = 0; i < NumDataMembers; i++) {
    field[i] = (cfg.print_field[i] == 0) ? 0 : 1;
    if(IsPasswordField(i)) field[i] = 0;
  }
  landscape = (cfg.print_orientation == 0); // 0 was Landscape
  switch(cfg.print_paper_size) {
    case 1: paper = wxPAPER_LEGAL; break;
    case 2: paper = wxPAPER_TABLOID; break;
    case 3: paper = wxPAPER_A3; break;
    case 4: paper = wxPAPER_A4; break;
    default: paper = wxPAPER_LETTER; break;
  }
  if(cfg.print_left_margin_size >= 3 && cfg.print_left_margin_size <= 60) margin_mm[0] = cfg.print_left_margin_size;
  header = cfg.print_page_header != 0;
  date = cfg.print_doc_time_and_date != 0;
  page_numbers = cfg.print_page_footer != 0;
}

int EPDBPrintSettings::PrintsPasswords() const
{
  for(int i = 0; i < NumDataMembers; i++) if(IsPasswordField(i) && field[i]) return 1;
  return 0;
}

static wxString EPDBPrintSettingsFile()
{
  wxFileName fn(wxString::FromUTF8(progcfg->cfgFile.c_str()));
  fn.SetFullName("print_settings.txt");
  return fn.GetFullPath();
}

static wxString EPDBPrintSettingsLine(const EPDBPrintSettings &s)
{
  wxString f;
  for(int i = 0; i < NumDataMembers; i++) f << (s.field[i] ? '1' : '0');
  return wxString::Format("layout=%d records=%d fields=%s paper=%d landscape=%d margins=%d,%d,%d,%d "
			  "header=%d info=%d date=%d pages=%d shading=%d",
			  s.layout, s.records, f, (int)s.paper, s.landscape, s.margin_mm[0], s.margin_mm[1],
			  s.margin_mm[2], s.margin_mm[3], s.header, s.info_line, s.date, s.page_numbers, s.shading);
}

int EPDBPrintSettings::Load(const wxString &db_path)
// One database per line: the path, a tab and the settings
{
  wxString name = EPDBPrintSettingsFile();
  wxFile f;
  if(!wxFileExists(name) || !f.Open(name)) return 0;
  wxString text;
  f.ReadAll(&text, wxConvUTF8);
  wxStringTokenizer lines(text, "\n");
  while(lines.HasMoreTokens()) {
    wxString line = lines.GetNextToken();
    line.Trim();
    wxString path = line.BeforeFirst('\t');
    if(path != db_path) continue;
    wxStringTokenizer kv(line.AfterFirst('\t'), " ");
    while(kv.HasMoreTokens()) {
      wxString t = kv.GetNextToken();
      wxString k = t.BeforeFirst('='), v = t.AfterFirst('=');
      long n = 0;
      v.ToLong(&n);
      if(k == "layout") layout = (n == EPDB_PRINT_TABLE) ? EPDB_PRINT_TABLE : EPDB_PRINT_CARDS;
      else if(k == "records") records = (n >= 0 && n <= 2) ? (int)n : 0;
      else if(k == "fields") {
	for(int i = 0; i < NumDataMembers && i < (int)v.length(); i++) field[i] = (v[i] == '1');
      }
      else if(k == "paper") paper = (wxPaperSize)n;
      else if(k == "landscape") landscape = n != 0;
      else if(k == "margins") {
	wxArrayString m = wxSplit(v, ',');
	for(int i = 0; i < 4 && i < (int)m.size(); i++) {
	  long mm = 0;
	  if(m[i].ToLong(&mm) && mm >= 3 && mm <= 60) margin_mm[i] = (int)mm;
	}
      }
      else if(k == "header") header = n != 0;
      else if(k == "info") info_line = n != 0;
      else if(k == "date") date = n != 0;
      else if(k == "pages") page_numbers = n != 0;
      else if(k == "shading") shading = n != 0;
    }
    return 1;
  }
  return 0;
}

void EPDBPrintSettings::Save(const wxString &db_path) const
{
  wxString name = EPDBPrintSettingsFile();
  wxString text, old;
  wxFile in;
  if(wxFileExists(name) && in.Open(name)) {
    in.ReadAll(&old, wxConvUTF8);
    in.Close();
  }
  wxStringTokenizer lines(old, "\n");
  int n = 0;
  while(lines.HasMoreTokens() && n < 200) {
    wxString line = lines.GetNextToken();
    line.Trim();
    if(line.IsEmpty() || line.BeforeFirst('\t') == db_path) continue;
    text << line << "\n";
    n++;
  }
  text << db_path << "\t" << EPDBPrintSettingsLine(*this) << "\n";
  wxScopedCharBuffer b = text.utf8_str();
  gxString err;
  WritePrivateFile(name.utf8_str(), b.data(), (unsigned)b.length(), err);
}

// ----------------------------------------------------------- //
// The job: records read from the database
// ----------------------------------------------------------- //
static wxString EPDBColumnLabel(CryptDBDocument *doc, int i)
{
  char dest[DBStringLength];
  gxString name = doc->DBParms()->db_config.GetColName(i, dest);
  if(name.is_null()) name = progcfg->global_dbparms.dbgrid_labels[i];
  return EPDBFromDB(name.c_str());
}

static void EPDBGridTitles(CryptDBDocument *doc, int which, std::vector<wxString> &titles)
// The titles of the rows shown (search results) or selected
{
  titles.clear();
  wxGrid *g = doc->GetGrid();
  if(!g) return;
  wxArrayInt sel;
  if(which == EPDB_PRINT_SELECTED) {
    sel = g->GetSelectedRows();
    for(int r = 0; r < g->GetNumberRows(); r++) {
      if(sel.Index(r) != wxNOT_FOUND) continue;
      if(g->IsInSelection(r, 0)) sel.Add(r);
    }
    if(sel.IsEmpty() && g->GetGridCursorRow() >= 0) sel.Add(g->GetGridCursorRow());
  }
  for(int r = 0; r < g->GetNumberRows(); r++) {
    wxString t = g->GetCellValue(r, 0);
    if(t.IsEmpty()) continue;
    if(which == EPDB_PRINT_SEARCH && !g->IsRowShown(r)) continue;
    if(which == EPDB_PRINT_SELECTED && sel.Index(r) == wxNOT_FOUND) continue;
    titles.push_back(t);
  }
}

static int EPDBCountGrid(CryptDBDocument *doc, int which)
{
  std::vector<wxString> t;
  EPDBGridTitles(doc, which, t);
  return (int)t.size();
}

int EPDBMakePrintJob(CryptDBDocument *doc, EPDBPrintJob &job, const EPDBPrintSettings &s)
{
  job.settings = s;
  job.records.clear();
  char dest[DBStringLength];
  for(int i = 0; i < NumDataMembers; i++) job.label[i] = EPDBColumnLabel(doc, i);

  std::vector<wxString> want;
  if(s.records != EPDB_PRINT_ALL) {
    EPDBGridTitles(doc, s.records, want);
    std::sort(want.begin(), want.end());
  }
  int n = BuildVirtualDB(doc);
  gxListNode<FAU> *p = dllist->GetHead();
  while(p && n >= 0) {
    INFOHOG ih(doc->GetPOD());
    if(ih.ReadObject(p->data) && ih.GetMemberLen(0) == sizeof(INFOHOG_t)) {
      EPDBPrintRecord r;
      for(int i = 0; i < NumDataMembers; i++) {
	if(ih.GetMemberLen(i) == sizeof(INFOHOG_t)) {
	  INFOHOG_t *v = (INFOHOG_t *)ih.GetMember(i);
	  if(!v->is_null()) r.value[i] = EPDBFromDB(v->c_str(dest));
	  r.value[i].Replace("\r\n", "\n");
	}
      }
      int keep = 1;
      if(s.records != EPDB_PRINT_ALL) keep = std::binary_search(want.begin(), want.end(), r.value[0]);
      if(keep) job.records.push_back(r);
    }
    p = p->next;
  }

  // Header: the document name set in the database, else the database
  // name, else the file name. A custom header replaces $NAME with it.
  gxDatabaseConfig &cfg = doc->DBParms()->db_config;
  wxString name = EPDBFromDB(cfg.print_doc_name.c_str(dest));
  if(name.IsEmpty()) name = EPDBFromDB(cfg.database_name.c_str(dest));
  if(name.IsEmpty()) name = wxFileName(EPDBFromDB(doc->DBParms()->data_file.c_str())).GetFullName();
  wxString custom = EPDBFromDB(cfg.print_doc_custom_header.c_str(dest));
  if(!custom.IsEmpty()) {
    custom.Replace("$NAME", name);
    custom.Replace("$name", name);
    name = custom;
  }
  job.title = name;

  wxString info = wxString::Format(job.records.size() == 1 ? "%d record" : "%d records", (int)job.records.size());
  if(s.records == EPDB_PRINT_SEARCH) {
    wxString text = doc->search_ctrl ? doc->search_ctrl->GetValue() : wxString();
    if(!text.IsEmpty()) info << ", search \"" << text << "\"";
  }
  else if(s.records == EPDB_PRINT_SELECTED) {
    info << ", selected";
  }
  info << (s.PrintsPasswords() ? ", PASSWORDS ARE PRINTED" : ", passwords not printed");
  job.info = info;
  job.date = wxDateTime::Now().Format("%Y-%m-%d %H:%M");
  return (int)job.records.size();
}

// ----------------------------------------------------------- //
// The printout
// ----------------------------------------------------------- //
EPDBPrintout::EPDBPrintout(const EPDBPrintJob &j) : wxPrintout(j.title)
{
  job = j;
  planned = 0;
  line_h = title_h = small_h = header_h = 0;
  left = top = right = bottom = content_top = content_bottom = label_w = 0;
  ppmm_x = ppmm_y = 1;
}

double EPDBPrintout::MM(double mm, int vertical) const
{
  return mm * (vertical ? ppmm_y : ppmm_x);
}

void EPDBPrintout::SetUpDC(wxDC *dc)
// Logical units are screen pixels mapped to the paper, the same for the
// preview and the printer
{
  MapScreenSizeToPaper();
  int sx = 96, sy = 96;
  GetPPIScreen(&sx, &sy);
  if(sx <= 0) sx = 96;
  if(sy <= 0) sy = 96;
  ppmm_x = sx / 25.4;
  ppmm_y = sy / 25.4;
  wxRect paper = GetLogicalPaperRect();
  const EPDBPrintSettings &s = job.settings;
  left = paper.x + MM(s.margin_mm[0], 0);
  top = paper.y + MM(s.margin_mm[1], 1);
  right = paper.x + paper.width - MM(s.margin_mm[2], 0);
  bottom = paper.y + paper.height - MM(s.margin_mm[3], 1);
  // Keep inside the printable area of the printer
  wxRect page = GetLogicalPageRect();
  if(left < page.x) left = page.x;
  if(top < page.y) top = page.y;
  if(right > page.x + page.width) right = page.x + page.width;
  if(bottom > page.y + page.height) bottom = page.y + page.height;
  dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
}

void EPDBPrintout::MakeFonts(wxDC *dc)
// The system GUI font face at about 9.5 pt on paper. The size is checked
// on this device: a line of 9.5 pt text is about 1.2 x 9.5 pt high.
{
  wxFont base = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  int sx = 96, sy = 96;
  GetPPIScreen(&sx, &sy);
  if(sy <= 0) sy = 96;
  double want = 9.5 * 1.2 * sy / 72.0; // Logical height of a 9.5 pt line
  double size = 9.5;
  for(int pass = 0; pass < 2; pass++) {
    wxFont f(base);
    f.SetFractionalPointSize(size);
    dc->SetFont(f);
    wxCoord w, h;
    dc->GetTextExtent("Ag", &w, &h);
    if(h <= 0) break;
    double ratio = want / h;
    if(ratio > 0.8 && ratio < 1.25) break;
    size = size * ratio;
  }
  double k = size / 9.5;
  body_font = base;
  body_font.SetFractionalPointSize(size);
  bold_font = body_font;
  bold_font.SetWeight(wxFONTWEIGHT_BOLD);
  title_font = bold_font;
  title_font.SetFractionalPointSize(11 * k);
  header_font = bold_font;
  header_font.SetFractionalPointSize(12 * k);
  small_font = body_font;
  small_font.SetFractionalPointSize(8 * k);
  wxCoord w, h;
  dc->SetFont(body_font); dc->GetTextExtent("Ag", &w, &h); line_h = h;
  dc->SetFont(title_font); dc->GetTextExtent("Ag", &w, &h); title_h = h;
  dc->SetFont(small_font); dc->GetTextExtent("Ag", &w, &h); small_h = h;
  dc->SetFont(header_font); dc->GetTextExtent("Ag", &w, &h); header_h = h;
}

void EPDBPrintout::Wrap(wxDC *dc, const wxString &text, double width, std::vector<wxString> &out)
// Lines that fit in width: at spaces, or anywhere in a word (a URL) that
// is wider than the line. Line breaks in the text are kept.
{
  out.clear();
  width *= 0.97; // Room for small font metric differences between the preview and the printer
  if(width < 10) width = 10;
  wxArrayString paras = wxSplit(text, '\n', 0);
  if(paras.empty()) paras.Add(wxString());
  for(size_t pi = 0; pi < paras.size(); pi++) {
    wxArrayString words = wxSplit(paras[pi], ' ', 0);
    wxString line;
    int have_line = 0;
    for(size_t wi = 0; wi < words.size(); wi++) {
      wxString word = words[wi];
      wxString trial = have_line ? line + " " + word : word;
      wxCoord w, h;
      dc->GetTextExtent(trial, &w, &h);
      if(w <= width) { line = trial; have_line = 1; continue; }
      if(have_line) {
	out.push_back(line);
	line.Clear();
	have_line = 0;
      }
      // Break a word wider than the line where it reaches the edge
      for(;;) {
	dc->GetTextExtent(word, &w, &h);
	if(w <= width) { line = word; have_line = 1; break; }
	wxArrayInt widths;
	dc->GetPartialTextExtents(word, widths);
	size_t n = 0;
	while(n < word.length() && n < widths.size() && widths[n] <= width) n++;
	if(n == 0) n = 1;
	out.push_back(word.Left(n));
	word = word.Mid(n);
	if(word.IsEmpty()) break;
      }
    }
    if(have_line || words.empty()) out.push_back(line);
  }
}

void EPDBPrintout::DrawHeader(wxDC *dc, int page)
{
  const EPDBPrintSettings &s = job.settings;
  wxPen rule(wxColour(150, 150, 150), 1);
  dc->SetTextForeground(*wxBLACK);
  double y = top;
  if(s.header) {
    dc->SetFont(header_font);
    dc->DrawText(job.title, (int)left, (int)y);
    if(s.date) {
      dc->SetFont(small_font);
      wxCoord w, h;
      dc->GetTextExtent(job.date, &w, &h);
      dc->DrawText(job.date, (int)(right - w), (int)(y + header_h - h));
    }
    y += header_h;
  }
  if(s.info_line) {
    dc->SetFont(small_font);
    dc->SetTextForeground(job.settings.PrintsPasswords() ? wxColour(160, 0, 0) : wxColour(80, 80, 80));
    dc->DrawText(job.info, (int)left, (int)y);
    dc->SetTextForeground(*wxBLACK);
    y += small_h;
  }
  if(s.header || s.info_line) {
    y += MM(1, 1);
    dc->SetPen(rule);
    dc->DrawLine((int)left, (int)y, (int)right, (int)y);
  }
  if(s.page_numbers) {
    dc->SetFont(small_font);
    wxString t = wxString::Format("Page %d of %d", page, NumPages());
    wxCoord w, h;
    dc->GetTextExtent(t, &w, &h);
    dc->DrawText(t, (int)((left + right - w) / 2), (int)(bottom - h));
  }
}

void EPDBPrintout::OnPreparePrinting()
{
  wxDC *dc = GetDC();
  if(!dc) return;
  SetUpDC(dc);
  MakeFonts(dc);
  const EPDBPrintSettings &s = job.settings;
  content_top = top;
  if(s.header) content_top += header_h;
  if(s.info_line) content_top += small_h;
  if(s.header || s.info_line) content_top += MM(3, 1);
  content_bottom = bottom;
  if(s.page_numbers) content_bottom -= small_h + MM(2, 1);
  if(content_bottom - content_top < 3 * line_h) content_bottom = content_top + 3 * line_h;
  // 10/27/2026: One plan for the preview and the print made from it, so
  // both have the same pages. A different paper or orientation chosen in
  // the system print dialog makes a new plan.
  wxRect paper = GetLogicalPaperRect();
  if(job.plan && job.plan->p.valid && std::fabs(job.plan->p.width - paper.width) <= 0.01 * paper.width &&
     std::fabs(job.plan->p.height - paper.height) <= 0.01 * paper.height) {
    const Plan &pl = job.plan->p;
    pages = pl.pages; columns = pl.columns; col_x = pl.col_x; col_w = pl.col_w;
    table_cells = pl.table_cells; label_w = pl.label_w;
    planned = 1;
    return;
  }
  pages.clear();
  if(s.layout == EPDB_PRINT_TABLE) PlanTable(dc);
  else PlanCards(dc);
  if(pages.empty()) pages.push_back(Page());
  planned = 1;
  if(job.plan) {
    Plan &pl = job.plan->p;
    pl.pages = pages; pl.columns = columns; pl.col_x = col_x; pl.col_w = col_w;
    pl.table_cells = table_cells; pl.label_w = label_w; pl.line_h = line_h;
    pl.width = paper.width; pl.height = paper.height; pl.valid = 1;
  }
}

void EPDBPrintout::PlanCards(wxDC *dc)
// One block per record: the title, then a label and the wrapped value of
// each field. A block that does not fit moves to the next page, a block
// taller than a page goes on with "(continued)".
{
  const EPDBPrintSettings &s = job.settings;
  dc->SetFont(bold_font);
  label_w = 0;
  for(int i = 1; i < NumDataMembers; i++) {
    if(!s.field[i]) continue;
    wxCoord w, h;
    dc->GetTextExtent(job.label[i] + ":", &w, &h);
    if(w > label_w) label_w = w;
  }
  label_w += MM(3, 0);
  double width = right - left;
  if(label_w > width * 0.4) label_w = width * 0.4;
  double gap = MM(3, 1);

  Page page;
  double y = content_top;
  for(size_t r = 0; r < job.records.size(); r++) {
    const EPDBPrintRecord &rec = job.records[r];
    std::vector<Line> lines;
    std::vector<wxString> wrapped;
    dc->SetFont(title_font);
    Wrap(dc, s.field[0] ? rec.value[0] : wxString::Format("Record %d", (int)r + 1), width, wrapped);
    for(size_t k = 0; k < wrapped.size(); k++) {
      Line l; l.rec = (int)r; l.kind = 0; l.field = 0; l.first = (k == 0); l.text = wrapped[k];
      lines.push_back(l);
    }
    dc->SetFont(body_font);
    for(int i = 1; i < NumDataMembers; i++) {
      if(!s.field[i] || rec.value[i].IsEmpty()) continue;
      Wrap(dc, rec.value[i], width - label_w, wrapped);
      for(size_t k = 0; k < wrapped.size(); k++) {
	Line l; l.rec = (int)r; l.kind = 1; l.field = i; l.first = (k == 0); l.text = wrapped[k];
	lines.push_back(l);
      }
    }
    double block = 0;
    for(size_t k = 0; k < lines.size(); k++) block += lines[k].kind == 0 ? title_h : line_h;
    double avail = content_bottom - content_top;
    if(y > content_top && y + block > content_bottom && block <= avail) {
      pages.push_back(page);
      page = Page();
      y = content_top;
    }
    for(size_t k = 0; k < lines.size(); k++) {
      double h = lines[k].kind == 0 ? title_h : line_h;
      if(y + h > content_bottom && y > content_top) {
	pages.push_back(page);
	page = Page();
	y = content_top;
	// The rest of the record goes on under its title
	Item c; c.kind = 0; c.rec = (int)r; c.first = c.count = 0; c.y = y; c.h = title_h;
	c.line.rec = (int)r; c.line.kind = 2; c.line.field = 0; c.line.first = 1;
	c.line.text = (s.field[0] ? rec.value[0].BeforeFirst('\n') : wxString::Format("Record %d", (int)r + 1)) + " (continued)";
	page.items.push_back(c);
	y += title_h;
	// A field split here shows its label again
	if(lines[k].kind == 1) lines[k].first = 1;
      }
      Item it; it.kind = 0; it.rec = (int)r; it.first = it.count = 0; it.y = y; it.h = h; it.line = lines[k];
      page.items.push_back(it);
      y += h;
    }
    // A rule between records
    Item rule; rule.kind = 3; rule.rec = (int)r; rule.first = rule.count = 0;
    rule.y = y + gap / 2; rule.h = gap;
    page.items.push_back(rule);
    y += gap;
  }
  if(!page.items.empty()) pages.push_back(page);
}

void EPDBPrintout::PlanTable(wxDC *dc)
// The chosen fields as columns fitted to the page width by their
// content, cells wrapped, the header row on every page
{
  const EPDBPrintSettings &s = job.settings;
  columns.clear();
  for(int i = 0; i < NumDataMembers; i++) if(s.field[i]) columns.push_back(i);
  if(columns.empty()) columns.push_back(0);
  size_t n = columns.size();
  double width = right - left;
  double pad = MM(1.5, 0);
  std::vector<double> natural(n, 0), minimum(n, 0);
  for(size_t c = 0; c < n; c++) {
    wxCoord w, h;
    dc->SetFont(bold_font);
    // The label may wrap at spaces, its longest word is the minimum
    wxArrayString words = wxSplit(job.label[columns[c]], ' ');
    for(size_t k = 0; k < words.size(); k++) {
      dc->GetTextExtent(words[k], &w, &h);
      if(w > minimum[c]) minimum[c] = w;
    }
    dc->GetTextExtent(job.label[columns[c]], &w, &h);
    natural[c] = w;
    dc->SetFont(body_font);
    for(size_t r = 0; r < job.records.size(); r++) {
      wxArrayString lines = wxSplit(job.records[r].value[columns[c]], '\n', 0);
      for(size_t k = 0; k < lines.size(); k++) {
	dc->GetTextExtent(lines[k], &w, &h);
	if(w > natural[c]) natural[c] = w;
      }
    }
    natural[c] += 2 * pad;
    minimum[c] += 2 * pad;
  }
  double avail = width;
  double total = 0;
  for(size_t c = 0; c < n; c++) total += natural[c];
  col_w.assign(n, 0);
  if(total <= 0) total = 1;
  if(total <= avail) {
    // Everything fits: the spare width goes to the columns by their size
    for(size_t c = 0; c < n; c++) col_w[c] = natural[c] + (avail - total) * natural[c] / total;
  }
  else {
    // Each column gets its minimum (at most an equal share), the rest is
    // shared by how much more each column needs
    double share = avail / n, used = 0, need = 0;
    for(size_t c = 0; c < n; c++) {
      if(minimum[c] > share) minimum[c] = share;
      if(minimum[c] > natural[c]) minimum[c] = natural[c];
      col_w[c] = minimum[c];
      used += minimum[c];
      need += natural[c] - minimum[c];
    }
    double spare = avail - used;
    for(size_t c = 0; c < n; c++) {
      if(need > 0) col_w[c] += spare * (natural[c] - minimum[c]) / need;
    }
  }
  col_x.assign(n, left);
  for(size_t c = 1; c < n; c++) col_x[c] = col_x[c-1] + col_w[c-1];

  // The header row
  std::vector<std::vector<wxString> > header(n);
  dc->SetFont(bold_font);
  size_t header_lines = 1;
  for(size_t c = 0; c < n; c++) {
    Wrap(dc, job.label[columns[c]], col_w[c] - 2 * pad, header[c]);
    if(header[c].size() > header_lines) header_lines = header[c].size();
  }
  table_cells.clear();
  table_cells.push_back(header);
  double header_row_h = header_lines * line_h + MM(1.5, 1);

  dc->SetFont(body_font);
  Page page;
  double y = content_top;
  Item hi; hi.kind = 1; hi.rec = -1; hi.first = 0; hi.count = (int)header_lines; hi.y = y; hi.h = header_row_h;
  page.items.push_back(hi);
  y += header_row_h;
  double row_pad = MM(1.2, 1);
  for(size_t r = 0; r < job.records.size(); r++) {
    std::vector<std::vector<wxString> > cells(n);
    size_t rows = 1;
    for(size_t c = 0; c < n; c++) {
      Wrap(dc, job.records[r].value[columns[c]], col_w[c] - 2 * pad, cells[c]);
      if(cells[c].size() > rows) rows = cells[c].size();
    }
    table_cells.push_back(cells);
    int cell_index = (int)table_cells.size() - 1;
    double h = rows * line_h + row_pad;
    double row_avail = content_bottom - (content_top + header_row_h);
    if(y + h > content_bottom && h <= row_avail) {
      pages.push_back(page);
      page = Page();
      y = content_top;
      hi.y = y;
      page.items.push_back(hi);
      y += header_row_h;
    }
    // A row taller than a page is split by lines
    size_t first = 0;
    while(first < rows) {
      size_t fit = (size_t)((content_bottom - y - row_pad) / line_h);
      if(fit < 1) fit = 1;
      size_t count = rows - first;
      if(count > fit) count = fit;
      Item it; it.kind = 2; it.rec = cell_index; it.first = (int)first; it.count = (int)count;
      it.y = y; it.h = count * line_h + row_pad;
      it.line.rec = (int)r;
      page.items.push_back(it);
      y += it.h;
      first += count;
      if(first < rows) {
	pages.push_back(page);
	page = Page();
	y = content_top;
	hi.y = y;
	page.items.push_back(hi);
	y += header_row_h;
      }
    }
  }
  pages.push_back(page);
}

bool EPDBPrintout::OnPrintPage(int page_num)
{
  wxDC *dc = GetDC();
  if(!dc || !planned) return false;
  if(page_num < 1 || page_num > NumPages()) return false;
  SetUpDC(dc);
  const Page &page = pages[page_num - 1];
  const EPDBPrintSettings &s = job.settings;
  wxPen rule(wxColour(190, 190, 190), 1);
  wxPen dark(wxColour(110, 110, 110), 1);
  wxColour label_colour(85, 85, 85);
  double pad = MM(1.5, 0);
  for(size_t k = 0; k < page.items.size(); k++) {
    const Item &it = page.items[k];
    dc->SetTextForeground(*wxBLACK);
    if(it.kind == 0) { // Card line
      const Line &l = it.line;
      if(l.kind == 0 || l.kind == 2) {
	dc->SetFont(title_font);
	dc->DrawText(l.text, (int)left, (int)it.y);
      }
      else {
	if(l.first) {
	  dc->SetFont(bold_font);
	  dc->SetTextForeground(label_colour);
	  dc->DrawText(job.label[l.field] + ":", (int)left, (int)it.y);
	  dc->SetTextForeground(*wxBLACK);
	}
	dc->SetFont(body_font);
	dc->DrawText(l.text, (int)(left + label_w), (int)it.y);
      }
    }
    else if(it.kind == 3) { // Rule between cards
      dc->SetPen(rule);
      dc->DrawLine((int)left, (int)it.y, (int)right, (int)it.y);
    }
    else if(it.kind == 1) { // Table header row
      dc->SetPen(*wxTRANSPARENT_PEN);
      dc->SetBrush(wxBrush(wxColour(225, 225, 225)));
      dc->DrawRectangle((int)left, (int)it.y, (int)(right - left), (int)it.h);
      dc->SetFont(bold_font);
      const std::vector<std::vector<wxString> > &cells = table_cells[0];
      for(size_t c = 0; c < columns.size(); c++) {
	for(size_t j = 0; j < cells[c].size(); j++) {
	  dc->DrawText(cells[c][j], (int)(col_x[c] + pad), (int)(it.y + MM(0.75, 1) + j * line_h));
	}
      }
      dc->SetPen(dark);
      dc->DrawLine((int)left, (int)(it.y + it.h), (int)right, (int)(it.y + it.h));
    }
    else if(it.kind == 2) { // Table row lines
      if(s.shading && (it.line.rec % 2) == 1) {
	dc->SetPen(*wxTRANSPARENT_PEN);
	dc->SetBrush(wxBrush(wxColour(242, 242, 242)));
	dc->DrawRectangle((int)left, (int)it.y, (int)(right - left), (int)it.h);
      }
      dc->SetFont(body_font);
      const std::vector<std::vector<wxString> > &cells = table_cells[it.rec];
      for(size_t c = 0; c < columns.size(); c++) {
	for(int j = 0; j < it.count; j++) {
	  size_t li = it.first + j;
	  if(li >= cells[c].size()) break;
	  dc->DrawText(cells[c][li], (int)(col_x[c] + pad), (int)(it.y + MM(0.6, 1) + j * line_h));
	}
      }
      dc->SetPen(rule);
      dc->DrawLine((int)left, (int)(it.y + it.h), (int)right, (int)(it.y + it.h));
    }
  }
  DrawHeader(dc, page_num);
  return true;
}

bool EPDBPrintout::HasPage(int page)
{
  return page >= 1 && page <= NumPages();
}

void EPDBPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = NumPages() > 0 ? NumPages() : 1;
  *selPageFrom = 1;
  *selPageTo = *maxPage;
}

// ----------------------------------------------------------- //
// The Print dialog
// ----------------------------------------------------------- //
class EPDBPrintDialog : public wxDialog
{
public:
  EPDBPrintDialog(wxWindow *parent, CryptDBDocument *d, EPDBPrintSettings &s);
  void Store(EPDBPrintSettings &s);

private:
  void OnChange(wxCommandEvent &event);
  void OnButton(wxCommandEvent &event);
  void UpdateState();

  CryptDBDocument *doc;
  wxRadioBox *layout, *records, *orientation;
  wxCheckBox *fields[NumDataMembers];
  wxStaticText *password_warning;
  wxChoice *paper, *margins;
  wxSpinCtrl *margin[4];
  wxCheckBox *header, *info_line, *date, *page_numbers, *shading;
};

static const int epdb_margin_presets[][4] = { { 13, 13, 13, 13 }, { 6, 6, 6, 6 }, { 25, 25, 25, 25 } };

EPDBPrintDialog::EPDBPrintDialog(wxWindow *parent, CryptDBDocument *d, EPDBPrintSettings &s)
  : wxDialog(parent, wxID_ANY, "Print", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  doc = d;
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(12), gap = FromDIP(6);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *cols = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *c1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *c2 = new wxBoxSizer(wxVERTICAL);

  wxString layouts[2] = { "Cards: one block per record, every field in full", "Table: one row per record" };
  layout = new wxRadioBox(this, wxID_ANY, "Layout", wxDefaultPosition, wxDefaultSize, 2, layouts, 1, wxRA_SPECIFY_COLS);
  layout->SetSelection(s.layout);
  c1->Add(layout, 0, wxEXPAND | wxBOTTOM, gap);

  int n_all = doc->GridFrame() ? doc->GridFrame()->NumRecords() : 0;
  int n_search = EPDBCountGrid(doc, EPDB_PRINT_SEARCH);
  int n_sel = EPDBCountGrid(doc, EPDB_PRINT_SELECTED);
  wxString recs[3] = { wxString::Format("All records (%d)", n_all),
		       wxString::Format("Search results (%d)", n_search),
		       wxString::Format("Selected records (%d)", n_sel) };
  records = new wxRadioBox(this, wxID_ANY, "Records", wxDefaultPosition, wxDefaultSize, 3, recs, 1, wxRA_SPECIFY_COLS);
  int searching = doc->search_ctrl && !doc->search_ctrl->GetValue().IsEmpty();
  records->Enable(1, searching != 0);
  records->Enable(2, n_sel > 0);
  int rsel = s.records;
  if(rsel == EPDB_PRINT_SEARCH && !searching) rsel = EPDB_PRINT_ALL;
  if(rsel == EPDB_PRINT_SELECTED && n_sel == 0) rsel = EPDB_PRINT_ALL;
  records->SetSelection(rsel);
  c1->Add(records, 0, wxEXPAND | wxBOTTOM, gap);

  wxStaticBoxSizer *fb = new wxStaticBoxSizer(wxVERTICAL, this, "Fields");
  wxGridSizer *fg = new wxGridSizer(2, gap / 2, gap);
  for(int i = 0; i < NumDataMembers; i++) {
    fields[i] = new wxCheckBox(fb->GetStaticBox(), wxID_ANY, EPDBColumnLabel(doc, i));
    fields[i]->SetValue(s.field[i] != 0);
    fg->Add(fields[i], 0);
  }
  fb->Add(fg, 0, wxEXPAND | wxALL, gap / 2);
  password_warning = new wxStaticText(fb->GetStaticBox(), wxID_ANY,
				      "The passwords will be printed in clear text. Anyone who sees "
				      "the paper can read them.");
  password_warning->SetForegroundColour(EPDBWarningColour());
  password_warning->Wrap(FromDIP(330));
  fb->Add(password_warning, 0, wxEXPAND | wxALL, gap / 2);
  c1->Add(fb, 0, wxEXPAND);

  wxStaticBoxSizer *pb = new wxStaticBoxSizer(wxVERTICAL, this, "Page");
  wxFlexGridSizer *pg = new wxFlexGridSizer(2, gap, gap);
  pg->Add(new wxStaticText(pb->GetStaticBox(), wxID_ANY, "Paper"), 0, wxALIGN_CENTER_VERTICAL);
  paper = new wxChoice(pb->GetStaticBox(), wxID_ANY);
  int psel = 0;
  for(int i = 0; i < epdb_num_papers; i++) {
    paper->Append(epdb_paper_names[i]);
    if(epdb_papers[i] == s.paper) psel = i;
  }
  paper->SetSelection(psel);
  pg->Add(paper, 0);
  pb->Add(pg, 0, wxALL, gap / 2);
  wxString ors[2] = { "Portrait", "Landscape" };
  orientation = new wxRadioBox(pb->GetStaticBox(), wxID_ANY, "Orientation", wxDefaultPosition, wxDefaultSize, 2, ors, 2, wxRA_SPECIFY_COLS);
  orientation->SetSelection(s.landscape ? 1 : 0);
  pb->Add(orientation, 0, wxEXPAND | wxALL, gap / 2);
  wxBoxSizer *mr = new wxBoxSizer(wxHORIZONTAL);
  mr->Add(new wxStaticText(pb->GetStaticBox(), wxID_ANY, "Margins"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, gap);
  margins = new wxChoice(pb->GetStaticBox(), wxID_ANY);
  margins->Append("Normal (13 mm)");
  margins->Append("Narrow (6 mm)");
  margins->Append("Wide (25 mm)");
  margins->Append("Custom");
  int msel = 3;
  for(int p = 0; p < 3; p++) {
    int same = 1;
    for(int i = 0; i < 4; i++) if(epdb_margin_presets[p][i] != s.margin_mm[i]) same = 0;
    if(same) { msel = p; break; }
  }
  margins->SetSelection(msel);
  mr->Add(margins, 0);
  pb->Add(mr, 0, wxALL, gap / 2);
  wxFlexGridSizer *mg = new wxFlexGridSizer(4, gap / 2, gap);
  const char *mnames[4] = { "Left mm", "Top mm", "Right mm", "Bottom mm" };
  for(int i = 0; i < 4; i++) {
    wxBoxSizer *one = new wxBoxSizer(wxVERTICAL);
    one->Add(new wxStaticText(pb->GetStaticBox(), wxID_ANY, mnames[i]), 0);
    margin[i] = new wxSpinCtrl(pb->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition,
			       wxSize(FromDIP(70), -1), wxSP_ARROW_KEYS, 3, 60, s.margin_mm[i]);
    one->Add(margin[i], 0);
    mg->Add(one, 0);
  }
  pb->Add(mg, 0, wxALL, gap / 2);
  c2->Add(pb, 0, wxEXPAND | wxBOTTOM, gap);

  wxStaticBoxSizer *hb = new wxStaticBoxSizer(wxVERTICAL, this, "Header and footer");
  header = new wxCheckBox(hb->GetStaticBox(), wxID_ANY, "Database name at the top");
  info_line = new wxCheckBox(hb->GetStaticBox(), wxID_ANY, "Record count, search and the passwords note");
  date = new wxCheckBox(hb->GetStaticBox(), wxID_ANY, "Print date");
  page_numbers = new wxCheckBox(hb->GetStaticBox(), wxID_ANY, "Page numbers (Page n of N)");
  shading = new wxCheckBox(hb->GetStaticBox(), wxID_ANY, "Shade every other row (table)");
  header->SetValue(s.header != 0);
  info_line->SetValue(s.info_line != 0);
  date->SetValue(s.date != 0);
  page_numbers->SetValue(s.page_numbers != 0);
  shading->SetValue(s.shading != 0);
  wxCheckBox *hc[5] = { header, info_line, date, page_numbers, shading };
  for(int i = 0; i < 5; i++) hb->Add(hc[i], 0, wxALL, gap / 2);
  c2->Add(hb, 0, wxEXPAND);

  cols->Add(c1, 1, wxEXPAND | wxRIGHT, pad);
  cols->Add(c2, 1, wxEXPAND);
  top->Add(cols, 1, wxEXPAND | wxALL, pad);

  wxBoxSizer *btns = new wxBoxSizer(wxHORIZONTAL);
  btns->AddStretchSpacer();
  wxButton *cancel = new wxButton(this, wxID_CANCEL);
  wxButton *preview = new wxButton(this, wxID_PREVIEW, "Preview");
  wxButton *print = new wxButton(this, wxID_PRINT, "Print...");
  print->SetDefault();
  btns->Add(cancel, 0, wxRIGHT, gap);
  btns->Add(preview, 0, wxRIGHT, gap);
  btns->Add(print, 0);
  top->Add(btns, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizerAndFit(top);
  CentreOnParent();

  Bind(wxEVT_CHECKBOX, &EPDBPrintDialog::OnChange, this);
  Bind(wxEVT_CHOICE, &EPDBPrintDialog::OnChange, this);
  Bind(wxEVT_RADIOBOX, &EPDBPrintDialog::OnChange, this);
  preview->Bind(wxEVT_BUTTON, &EPDBPrintDialog::OnButton, this);
  print->Bind(wxEVT_BUTTON, &EPDBPrintDialog::OnButton, this);
  UpdateState();
}

void EPDBPrintDialog::UpdateState()
{
  int pw = 0;
  for(int i = 0; i < NumDataMembers; i++) if(IsPasswordField(i) && fields[i]->GetValue()) pw = 1;
  password_warning->Show(pw != 0);
  int custom = margins->GetSelection() == 3;
  if(!custom) {
    for(int i = 0; i < 4; i++) margin[i]->SetValue(epdb_margin_presets[margins->GetSelection()][i]);
  }
  for(int i = 0; i < 4; i++) margin[i]->Enable(custom != 0);
  shading->Enable(layout->GetSelection() == EPDB_PRINT_TABLE);
  int any = 0;
  for(int i = 0; i < NumDataMembers; i++) if(fields[i]->GetValue()) any = 1;
  FindWindow(wxID_PRINT)->Enable(any != 0);
  FindWindow(wxID_PREVIEW)->Enable(any != 0);
  Layout();
  GetSizer()->Fit(this);
}

void EPDBPrintDialog::OnChange(wxCommandEvent &event)
{
  event.Skip();
  UpdateState();
}

void EPDBPrintDialog::OnButton(wxCommandEvent &event)
{
  EndModal(event.GetId());
}

void EPDBPrintDialog::Store(EPDBPrintSettings &s)
{
  s.layout = layout->GetSelection();
  s.records = records->GetSelection();
  for(int i = 0; i < NumDataMembers; i++) s.field[i] = fields[i]->GetValue() ? 1 : 0;
  int p = paper->GetSelection();
  s.paper = (p >= 0 && p < epdb_num_papers) ? epdb_papers[p] : wxPAPER_LETTER;
  s.landscape = orientation->GetSelection() == 1;
  for(int i = 0; i < 4; i++) s.margin_mm[i] = margin[i]->GetValue();
  s.header = header->GetValue();
  s.info_line = info_line->GetValue();
  s.date = date->GetValue();
  s.page_numbers = page_numbers->GetValue();
  s.shading = shading->GetValue();
}

static void EPDBLoadPrintSettings(CryptDBDocument *doc, EPDBPrintSettings &s)
{
  wxString path = EPDBFromDB(doc->DBParms()->data_file.c_str());
  if(!s.Load(path)) s.FromDatabase(doc->DBParms()->db_config);
}

int EPDBShowPrintDialog(wxWindow *parent, CryptDBDocument *doc, EPDBPrintJob &job)
{
  EPDBPrintSettings s;
  EPDBLoadPrintSettings(doc, s);
  EPDBPrintDialog dlg(parent, doc, s);
  int rv = dlg.ShowModal();
  if(rv == wxID_CANCEL) return rv;
  dlg.Store(s);
  s.Save(EPDBFromDB(doc->DBParms()->data_file.c_str()));
  EPDBMakePrintJob(doc, job, s);
  return rv;
}

void EPDBRunPrint(wxWindow *parent, CryptDBDocument *doc, EPDBPrintJob &job, int preview)
{
  if(job.records.empty()) {
    wxMessageBox("There are no records to print.", "Print", wxOK | wxICON_INFORMATION, parent);
    return;
  }
  wxPrintData &pd = doc->print_data;
  pd.SetPaperId((wxPaperSize)job.settings.paper);
  pd.SetOrientation(job.settings.landscape ? wxLANDSCAPE : wxPORTRAIT);
  wxPrintDialogData data(pd);
  job.plan = std::make_shared<EPDBPrintPlan>();
  if(preview) {
    wxPrintPreview *pv = new wxPrintPreview(new EPDBPrintout(job), new EPDBPrintout(job), &data);
    if(!pv->IsOk()) {
      delete pv;
      wxMessageBox("The print preview could not be made. Check the printer setup.", "Print Preview", wxOK | wxICON_ERROR, parent);
      return;
    }
    wxPreviewFrame *f = new wxPreviewFrame(pv, parent, "Print Preview - " + job.title);
    f->SetIcons(EPDBAppIcons());
    f->Initialize();
    f->Maximize(true);
    f->Show(true);
    return;
  }
  wxPrinter printer(&data);
  EPDBPrintout printout(job);
  if(!printer.Print(parent, &printout, true)) {
    if(wxPrinter::GetLastError() == wxPRINTER_ERROR) {
      wxMessageBox("There was a problem printing. Check the printer setup.", "Print", wxOK | wxICON_ERROR, parent);
    }
    return;
  }
  // Keep the printer, paper and orientation chosen in the system dialog
  pd = printer.GetPrintDialogData().GetPrintData();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
