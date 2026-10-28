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

10/27/2026: Icons, colors and DPI helpers, see epdb_art.h
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"
#include "epdb_icons.h"

static const char *EPDBIconSource(const char *name)
{
  for(int i = 0; epdb_icon_svgs[i].name; i++) {
    if(strcmp(epdb_icon_svgs[i].name, name) == 0) return epdb_icon_svgs[i].svg;
  }
  return 0;
}

int EPDBIsDark()
{
  return wxSystemSettings::GetAppearance().IsDark() ? 1 : 0;
}

wxBitmapBundle EPDBIcon(const char *name, int size_dip)
{
  const char *src = EPDBIconSource(name);
  if(!src) return wxBitmapBundle();
  wxString svg(src);
  // Light ink on a dark background, the accent stays the same
  if(EPDBIsDark()) svg.Replace("#1B1F24", "#E6E9EE");
  wxScopedCharBuffer buf = svg.utf8_str();
  return wxBitmapBundle::FromSVG(buf.data(), wxSize(size_dip, size_dip));
}

wxBitmapBundle EPDBAppIconBundle(int size_dip)
{
  const char *src = EPDBIconSource("app");
  if(!src) return wxBitmapBundle();
  return wxBitmapBundle::FromSVG(src, wxSize(size_dip, size_dip));
}

wxIconBundle EPDBAppIcons()
{
#ifdef __WXMSW__
  // Every size of the .ico in the resources (encrypted_password_database.rc)
  wxIconBundle icons("appicon", 0);
  if(icons.IsOk() && !icons.IsEmpty()) return icons;
#endif
  wxIconBundle icons2;
  wxBitmapBundle b = EPDBAppIconBundle(64);
  const int sizes[] = { 16, 20, 24, 32, 40, 48, 64, 128, 256 };
  for(size_t i = 0; i < WXSIZEOF(sizes); i++) {
    wxIcon icon;
    icon.CopyFromBitmap(b.GetBitmap(wxSize(sizes[i], sizes[i])));
    icons2.AddIcon(icon);
  }
  return icons2;
}

wxColour EPDBAccentColour()
{
  return EPDBIsDark() ? wxColour(0x6E, 0x9E, 0xEB) : wxColour(0x1F, 0x5F, 0xBF);
}

wxColour EPDBMutedTextColour()
{
  return EPDBIsDark() ? wxColour(0xA8, 0xAE, 0xB8) : wxColour(0x5B, 0x63, 0x6E);
}

wxColour EPDBWarningColour()
{
  return EPDBIsDark() ? wxColour(0xF2, 0xB1, 0x4B) : wxColour(0x9A, 0x5B, 0x00);
}

wxColour EPDBOkColour()
{
  return EPDBIsDark() ? wxColour(0x6C, 0xC9, 0x8A) : wxColour(0x1E, 0x7B, 0x3E);
}

void EPDBFitOnScreen(wxTopLevelWindow *w, int maximize)
{
  if(!w || w->IsMaximized() || w->IsIconized()) return;
  int d = wxDisplay::GetFromWindow(w);
  if(d == wxNOT_FOUND) d = wxDisplay::GetFromPoint(w->GetPosition());
  if(d == wxNOT_FOUND) d = 0;
  wxRect area = wxDisplay((unsigned)d).GetClientArea();
  wxRect r = w->GetRect();
  if(r.width > area.width || r.height > area.height) {
    if(maximize) {
      w->Maximize(true);
      return;
    }
    r.width = wxMin(r.width, area.width);
    r.height = wxMin(r.height, area.height);
  }
  if(r.x < area.x) r.x = area.x;
  if(r.y < area.y) r.y = area.y;
  if(r.GetRight() > area.GetRight()) r.x = area.GetRight() - r.width + 1;
  if(r.GetBottom() > area.GetBottom()) r.y = area.GetBottom() - r.height + 1;
  if(r != w->GetRect()) w->SetSize(r);
}

wxStaticText *EPDBHeading(wxWindow *parent, const wxString &text, int larger)
{
  wxStaticText *t = new wxStaticText(parent, wxID_ANY, text);
  wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  f.MakeBold();
  if(larger) f = f.Scaled(1.15f);
  t->SetFont(f);
  return t;
}

wxStaticText *EPDBNote(wxWindow *parent, const wxString &text, int wrap_dip)
{
  wxStaticText *t = new wxStaticText(parent, wxID_ANY, text);
  t->SetForegroundColour(EPDBMutedTextColour());
  if(wrap_dip > 0) t->Wrap(parent->FromDIP(wrap_dip));
  return t;
}

wxFont EPDBMonoFont()
{
  wxFont gui = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  wxFont f(wxFontInfo(gui.GetFractionalPointSize()).Family(wxFONTFAMILY_TELETYPE));
  return f;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
