// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/12/2023
// Copyright (c) 1999-2023 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
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

Application database code
*/
// ----------------------------------------------------------- // 
#include "m_globals.h"
#include "m_dbase.h"

// Global data structures used to organize and store data file nodes addresses
gxList<FAU> db_search_dllist;            // Doubly linked list
gxList<FAU> *dllist = &db_search_dllist; // Doubly linked list pointer
gxListNode<FAU> *dllistptr = 0;          // Doubly linked list node pointer

int gxDatabaseConfig::TestStaticArea(gxDatabase *f)
{
  if(!f) return 0;
  gxFileHeader fh;
  FAU static_area;
  if(f->Read(&fh, sizeof(gxFileHeader), (FAU)0) != gxDBASE_NO_ERROR) {
    return 0;
  }
  // Test the data file's static area
  static_area = fh.gxd_hs_fptr - (FAU_t)f->FileHeaderSize();
  if(static_area < sizeof(gxDatabaseConfig)) return 0;
  return 1;
}

int gxDatabaseConfig::ReadConfig(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;
  if(f->Read(this, this->SizeOf(), (FAU_t)f->FileHeaderSize()) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

FAU_t gxDatabaseConfig::GetFileAddress(gxDatabase *f, gxDatabaseConfigMembers m)
{
  if(!f) return -1;

  FAU_t curr_address = f->FileHeaderSize(); 

  if(m == VERSION_NUMBER) {
    return curr_address;
  }
  if(m == DATABASE_NAME) {
    return curr_address;
  }
  curr_address += sizeof(database_name);
  if(m == COLUMN_NAMES) {
    return curr_address;
  }
  curr_address += sizeof(column_names);
  if(m == VIEW_LABELS) {
    return curr_address;
  }
  curr_address += sizeof(view_labels);
  if(m == VIEW_ROW_NUMBERS) {
    return curr_address;
  }
  curr_address += sizeof(view_row_numbers);
  if(m == COL_SIZES) {
    return curr_address;
  }
  curr_address += sizeof(col_sizes);
  if(m == PRINT_FIELD) {
    return curr_address;
  }
  curr_address += sizeof(print_field);
  if(m == CELL_ATTRIB) {
    return curr_address;
  }
  curr_address += sizeof(cell_attrib);
  if(m == CELL_ALIGN) {
    return curr_address;
  }
  curr_address += sizeof(cell_align);
  if(m == CELL_BORDER) {
    return curr_address;
  }
  curr_address += sizeof(cell_border);
  if(m == CELL_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(cell_color);
  if(m == CELL_FILL) {
    return curr_address;
  }
  curr_address += sizeof(cell_fill);
  if(m == CELL_TEXT_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(cell_text_color);
  if(m == CELL_TEXT_FONT) {
    return curr_address;
  }
  curr_address += sizeof(cell_text_font);
  if(m == CELL_TYPE) {
    return curr_address;
  }
  curr_address += sizeof(cell_type);
  if(m == CELL_PATTERN) {
    return curr_address;
  }
  curr_address += sizeof(cell_pattern);
  if(m == CELL_PROTECTION) {
    return curr_address;
  }
  curr_address += sizeof(cell_protection);
  if(m == LABEL_ALIGN) {
    return curr_address;
  }
  curr_address += sizeof(label_align);
  if(m == LABEL_BORDER) {
    return curr_address;
  }
  curr_address += sizeof(label_border);
  if(m == LABEL_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(label_color);
  if(m == LABEL_FILL) {
    return curr_address;
  }
  curr_address += sizeof(label_fill);
  if(m == LABEL_TEXT_FONT) {
    return curr_address;
  }
  curr_address += sizeof(label_text_font);
  if(m == LABEL_TEXT_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(label_text_color);
  if(m == LABEL_PATTERN) {
    return curr_address;
  }
  curr_address += sizeof(label_pattern);
  if(m == LABEL_PROTECTION) {
    return curr_address;
  }
  curr_address += sizeof(label_protection);
  if(m == DISPLAY_FIELD) {
    return curr_address;
  }
  curr_address += sizeof(display_field);
  if(m == CELL_MISC) {
    return curr_address;
  }
  curr_address += sizeof(cell_misc);
  if(m == LABEL_MISC) {
    return curr_address;
  }
  curr_address += sizeof(auto_size);
  if(m == CELL_OVERFLOW) {
    return curr_address;
  }
  curr_address += sizeof(cell_overflow);
  if(m == VIEW_GRID_LINES) {
    return curr_address;
  }
  curr_address += sizeof(view_grid_lines);
  if(m == GRID_LINE_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(grid_line_color);
  if(m == VIEW_SKIP_LINES) {
    return curr_address;
  }
  curr_address += sizeof(view_skip_lines);
  if(m == SKIP_LINE_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(skip_line_color);
  if(m == GRID_BACKGROUND_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(grid_background_color);
  if(m == GRID_TEXT_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(grid_text_color);
  if(m == GRID_LABEL_BACKGROUND_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(grid_label_background_color);
  if(m == GRID_LABEL_TEXT_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(grid_label_text_color);
  if(m == HYPERLINK_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(hyperlink_color);
  if(m == GRID_LABEL_FONT) {
    return curr_address;
  }
  curr_address += sizeof(grid_label_font);
  if(m == GRID_LABEL_FONT_NAME) {
    return curr_address;
  }
  curr_address += sizeof(grid_label_font_name);
  if(m == GRID_TEXT_FONT) {
    return curr_address;
  }
  curr_address += sizeof(grid_text_font);
  if(m == GRID_TEXT_FONT_NAME) {
    return curr_address;
  }
  curr_address += sizeof(grid_text_font_name);
  if(m == HYPERLINK_FONT) {
    return curr_address;
  }
  curr_address += sizeof(hyperlink_font);
  if(m == HYPERLINK_FONT_NAME) {
    return curr_address;
  }
  curr_address += sizeof(hyperlink_font_name);
  if(m == PRINT_DOC_NAME) {
    return curr_address;
  }
  curr_address += sizeof(print_doc_name);
  if(m == PRINT_DOC_TIME_AND_DATE) {
    return curr_address;
  }
  curr_address += sizeof(print_doc_time_and_date);
  if(m == PRINT_GRID_LINES) {
    return curr_address;
  }
  curr_address += sizeof(print_grid_lines);
  if(m == PRINT_ORIENTATION) {
    return curr_address;
  }
  curr_address += sizeof(print_orientation);
  if(m == PRINT_LEFT_MARGIN_SIZE) {
    return curr_address;
  }
  curr_address += sizeof(print_left_margin_size);
  if(m == PRINT_PAPER_SIZE) {
    return curr_address;
  }
  curr_address += sizeof(print_paper_size);
  if(m == PRINT_PAGE_HEADER) {
    return curr_address;
  }
  curr_address += sizeof(print_page_header);
  if(m == PRINT_DOC_CUSTOM_HEADER) {
    return curr_address;
  }
  curr_address += sizeof(print_doc_custom_header);
  if(m == PRINT_HEADER_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(print_header_color);
  if(m == PRINT_HEADER_FONT) {
    return curr_address;
  }
  curr_address += sizeof(print_header_font);
  if(m == PRINT_HEADER_FONT_NAME) {
    return curr_address;
  }
  curr_address += sizeof(print_header_font_name);
  if(m == PRINT_PAGE_FOOTER) {
    return curr_address;
  }
  curr_address += sizeof(print_page_footer);
  if(m == PRINT_DOC_CUSTOM_FOOTER) {
    return curr_address;
  }
  curr_address += sizeof(print_doc_custom_footer);
  if(m == PRINT_FOOTER_COLOR) {
    return curr_address;
  }
  curr_address += sizeof(print_footer_color);
  if(m == PRINT_FOOTER_FONT) {
    return curr_address;
  }
  curr_address += sizeof(print_footer_font);
  if(m == PRINT_FOOTER_FONT_NAME) {
    return curr_address;
  }
  curr_address += sizeof(print_footer_font_name);
  if(m == PRINT_SKIP_LINES) {
    return curr_address;
  }
  curr_address += sizeof(print_skip_lines);
  if(m == PRINT_GRID_LABELS) {
    return curr_address;
  }
  curr_address += sizeof(print_grid_labels);
  if(m == PRINT_ROW_NUMBERS) {
    return curr_address;
  }
  curr_address += sizeof(print_row_numbers);
  if(m == PRINT_PAGE_SCALE) {
    return curr_address;
  }
  curr_address += sizeof(print_page_scale);
  if(m == CELL_TEXT_FONTS) {
    return curr_address;
  }
  curr_address += sizeof(cell_text_fonts);
  if(m == CELL_TEXT_FONT_NAMES) {
    return curr_address;
  }
  curr_address += sizeof(cell_text_font_names);
  if(m == LABEL_TEXT_FONTS) {
    return curr_address;
  }
  curr_address += sizeof(label_text_fonts);
  if(m == LABEL_TEXT_FONT_NAMES) {
    return curr_address;
  }
  curr_address += sizeof(label_text_font_names);
  if(m == TEXT_DELIMITER) {
    return curr_address;
  }
  return -1;
}

long gxDatabaseConfig::Version()
// Return the current version number
{
  return 2023107;
}

void gxDatabaseConfig::Copy(const gxDatabaseConfig &ob)
{
  int i, j;

  version_number = ob.version_number;
  database_name = ob.database_name;

  for(i = 0; i < NumDataMembers; i++) {
    column_names[i] = ob.column_names[i];
  }

  view_labels = ob.view_labels;
  view_row_numbers = ob.view_row_numbers;

  for(i = 0; i < NumDataMembers; i++) {
    col_sizes[i] = ob.col_sizes[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    print_field[i] = ob.print_field[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_attrib[i] = ob.cell_attrib[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_align[i] = ob.cell_align[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_border[i] = ob.cell_border[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_color[i] = ob.cell_color[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_fill[i] = ob.cell_fill[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_text_color[i] = ob.cell_text_color[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_text_font[i] = ob.cell_text_font[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_type[i] = ob.cell_type[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_pattern[i] = ob.cell_pattern[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_protection[i] = ob.cell_protection[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_align[i] = ob.label_align[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_border[i] = ob.label_border[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_color[i] = ob.label_color[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_fill[i] = ob.label_fill[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_text_font[i] = ob.label_text_font[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_text_color[i] = ob.label_text_color[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_pattern[i] = ob.label_pattern[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_protection[i] = ob.label_protection[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    display_field[i] = ob.display_field[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    cell_misc[i] = ob.cell_misc[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    label_misc[i] = ob.label_misc[i];
  }
  
  auto_size = ob.auto_size;
  cell_overflow = ob.cell_overflow;
  view_grid_lines = ob.view_grid_lines; 
  grid_line_color = ob.grid_line_color;
  view_skip_lines = ob.view_skip_lines;
  skip_line_color = ob.skip_line_color;
  grid_background_color = ob.grid_background_color;
  grid_text_color = ob.grid_text_color;
  grid_label_background_color = ob.grid_label_background_color;
  grid_label_text_color = ob.grid_label_text_color;
  hyperlink_color = ob.hyperlink_color;

  for(i = 0; i < FontElements; i++) {
    grid_label_font[i] = ob.grid_label_font[i];
  }

  grid_label_font_name = ob.grid_label_font_name;

  for(i = 0; i < FontElements; i++) {
   grid_text_font[i] = ob.grid_text_font[i];
  }

  grid_text_font_name = ob.grid_text_font_name;

  for(i = 0; i < FontElements; i++) {
   hyperlink_font[i] = ob.hyperlink_font[i];
  }

  hyperlink_font_name = ob.hyperlink_font_name;

  print_doc_name = ob.print_doc_name;
  print_doc_time_and_date = ob.print_doc_time_and_date;
  print_grid_lines = ob.print_grid_lines;
  print_orientation = ob.print_orientation;
  print_left_margin_size = ob.print_left_margin_size;
  print_paper_size = ob.print_paper_size;
  print_page_header = ob.print_page_header;
  print_doc_custom_header = ob.print_doc_custom_header;
  print_header_color = ob.print_header_color;

  for(i = 0; i < FontElements; i++) {
   print_header_font[i] = ob.print_header_font[i];
  }

  print_header_font_name = ob.print_header_font_name;
  print_page_footer = ob.print_page_footer;
  print_doc_custom_footer = ob.print_doc_custom_footer;
  print_footer_color = ob.print_footer_color;

  for(i = 0; i < FontElements; i++) {
   print_footer_font[i] = ob.print_footer_font[i];
  }

  print_footer_font_name = ob.print_footer_font_name;
  print_skip_lines = ob.print_skip_lines;
  print_grid_labels = ob.print_grid_labels;
  print_row_numbers = ob.print_row_numbers;
  print_page_scale = ob.print_page_scale;

  for(i = 0; i < NumDataMembers; i++) {
    for(j = 0; j < FontElements; j++) {
      cell_text_fonts[i][j] = ob.cell_text_fonts[i][j];
    }
  }  
  for(i = 0; i < NumDataMembers; i++) {
    cell_text_font_names[i] = ob.cell_text_font_names[i];
  }
  for(i = 0; i < NumDataMembers; i++) {
    for(j = 0; j < FontElements; j++) {
      label_text_fonts[i][j] = ob.label_text_fonts[i][j];
    }
  }  
  for(i = 0; i < NumDataMembers; i++) {
    label_text_font_names[i] = ob.label_text_font_names[i];
  }

  memmove(text_delimiter, ob.text_delimiter, 4);
}

gxDatabaseConfig &gxDatabaseConfig::operator=(const gxDatabaseConfig &ob) 
{
  if(&ob != this) Copy(ob);
  return *this;
}

void gxDatabaseConfig::Clear()
{
  unsigned i, j;

  version_number = Version();

  database_name.Clear();
  for(i = 0; i < (unsigned)NumDataMembers; i++) col_sizes[i] = DefaultColSize;
  for(i = 0; i < (unsigned)NumDataMembers; i++) column_names[i].Clear();
  for(i = 0; i < (unsigned)NumDataMembers; i++) print_field[i] = 1;
  for(i = 0; i < (unsigned)NumDataMembers; i++) display_field[i] = 1;
  view_labels = 1;
  view_row_numbers = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_align[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_attrib[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_border[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_color[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_fill[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_text_color[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_text_font[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_type[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_pattern[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_protection[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) cell_misc[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_align[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_border[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_color[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_fill[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_text_font[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_text_color[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_pattern[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_protection[i] = (gxINT32)0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) label_misc[i] = (gxINT32)0;

  // Extended grid functions  
  auto_size = 1;
  cell_overflow = 1;
  view_grid_lines = 1; 
  grid_line_color = (gxINT32)0;
  view_skip_lines = (gxINT32)0;
  skip_line_color = (gxINT32)0;
  grid_background_color = (gxINT32)0;
  grid_text_color = (gxINT32)0;
  grid_label_background_color = (gxINT32)0;
  grid_label_text_color = (gxINT32)0;
  hyperlink_color = (gxINT32)0;
  for(i = 0; i < (unsigned)FontElements; i++) grid_label_font[i] = (gxINT32)0;
  grid_label_font_name.Clear();
  for(i = 0; i < (unsigned)FontElements; i++) grid_text_font[i] = (gxINT32)0;
  grid_text_font_name.Clear();
  for(i = 0; i < (unsigned)FontElements; i++) hyperlink_font[i] = (gxINT32)0;
  hyperlink_font_name.Clear();

  // Extended print functions
  print_doc_name.Clear();
  print_doc_time_and_date = 1;
  print_grid_lines = 1;
  print_orientation = 1; // Default to portrait
  print_left_margin_size = 10;
  print_paper_size = (gxINT32)0;  // Default to letter
  print_page_header = 1;
  print_doc_custom_header.Clear();
  print_header_color = (gxINT32)0;
  for(i = 0; i < (unsigned)FontElements; i++) print_header_font[i] = 0;
  print_header_font_name.Clear();
  print_page_footer = 1;
  print_doc_custom_footer.Clear();
  print_footer_color = (gxINT32)0;
  for(i = 0; i < (unsigned)FontElements; i++) print_footer_font[i] = 0;
  print_footer_font_name.Clear();
  print_skip_lines = (gxINT32)0;
  print_grid_labels = 1;
  print_row_numbers = (gxINT32)0;
  print_page_scale = 100;

  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    for(j = 0; j < (unsigned)FontElements; j++) cell_text_fonts[i][j] = 0;
  }
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    for(j = 0; j < (unsigned)FontElements; j++) label_text_fonts[i][j] = 0;
  }

  text_delimiter[0] = '\t';
  text_delimiter[1] = 0;
  text_delimiter[2] = 0;
  text_delimiter[3] = 0;
}

int gxDatabaseConfig::WriteTextDelimiter(gxDatabase *f, char dchar)
{
  FAU_t address = GetFileAddress(f, TEXT_DELIMITER);
  if(address < (FAU_t)0) return 0;
  text_delimiter[0] = dchar;
  text_delimiter[1] = 0;
  text_delimiter[2] = 0;
  text_delimiter[3] = 0;

  if(f->Write(&text_delimiter, sizeof(text_delimiter), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

#ifdef __wxWINALL__
int gxDatabaseConfig::WriteGridLineColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, GRID_LINE_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&grid_line_color, color_buf, 4);
  if(f->Write(&grid_line_color, sizeof(grid_line_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetGridLineColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &grid_line_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WriteViewLabels(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, VIEW_LABELS);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&view_labels, sizeof(view_labels), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteViewRowNumbers(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, VIEW_ROW_NUMBERS);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&view_row_numbers, sizeof(view_row_numbers), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteViewGridLines(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, VIEW_GRID_LINES);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&view_grid_lines, sizeof(view_grid_lines), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteAutoSize(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, AUTO_SIZE);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&auto_size, sizeof(auto_size), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintField(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_FIELD);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_field, sizeof(print_field), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteDisplayField(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, DISPLAY_FIELD);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&display_field, sizeof(display_field), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteCellOverflow(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, CELL_OVERFLOW);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&cell_overflow, sizeof(cell_overflow), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WriteViewSkipLines(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, VIEW_SKIP_LINES);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&view_skip_lines, sizeof(view_skip_lines), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WriteSkipLineColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, SKIP_LINE_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&skip_line_color, color_buf, 4);
  if(f->Write(&skip_line_color, sizeof(skip_line_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WriteGridBackgroundColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, GRID_BACKGROUND_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&grid_background_color, color_buf, 4);
  if(f->Write(&grid_background_color, sizeof(grid_background_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetGridBackgroundColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &grid_background_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WriteGridTextColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, GRID_TEXT_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&grid_text_color, color_buf, 4);
  if(f->Write(&grid_text_color, sizeof(grid_text_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetGridTextColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &grid_text_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}


int gxDatabaseConfig::GetSkipLineColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &skip_line_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::GetGridLabelBackgroundColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &grid_label_background_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WriteGridLabelBackgroundColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, GRID_LABEL_BACKGROUND_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&grid_label_background_color, color_buf, 4);
  if(f->Write(&grid_label_background_color, sizeof(grid_label_background_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetGridLabelTextColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &grid_label_text_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WriteGridLabelTextColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, GRID_LABEL_TEXT_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&grid_label_text_color, color_buf, 4);
  if(f->Write(&grid_label_text_color, sizeof(grid_label_text_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetHyperlinkColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &hyperlink_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WriteHyperlinkColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, HYPERLINK_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&hyperlink_color, color_buf, 4);
  if(f->Write(&hyperlink_color, sizeof(hyperlink_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::get_font(gxINT32 *elements, DBString *font_name, 
			       wxFont *font)
{
  if(elements[0] != 1) return 0; // The font was not set

  int font_encoding = (int)elements[1]; 
  font->SetDefaultEncoding((wxFontEncoding)font_encoding);
  int font_family = (int)elements[2];
  font->SetFamily((wxFontFamily)font_family);
  font->SetPointSize((int)elements[3]);
  font->SetStyle((int)elements[4]);
  if(elements[5] == (gxINT32)0) { 
    font->SetUnderlined(FALSE);
  }
  else {
    font->SetUnderlined(TRUE);
  }
  int font_weight = (int)elements[6];
  font->SetWeight((wxFontWeight)font_weight);

  // Set the font name if this font has a face name
  if(elements[7] == 1) {
    if(!font_name->is_null()) {
      char dest[DBStringLength];
      wxString face_name = font_name->c_str(dest);
      font->SetFaceName(face_name);
    }
  }

  if(!font->Ok()) return 0; // This is not a valid font

  return 1;
}

int gxDatabaseConfig::write_font(gxDatabase *f, 
				 gxDatabaseConfigMembers element_address,
 				 gxDatabaseConfigMembers name_address,
				 gxINT32 *elements, DBString *font_name,
				 wxFont *font)
{
  FAU_t address = GetFileAddress(f, element_address);
  if(address < (FAU_t)0) return 0;

  elements[0] = 1;
  elements[1] = (int)font->GetDefaultEncoding();
  elements[2] = font->GetFamily(); 
  elements[3] = font->GetPointSize();
  elements[4] = font->GetStyle(); 
  elements[5] = (int)font->GetUnderlined();
  elements[6] = font->GetWeight();

  if(font->GetFaceName().IsNull()) {
    elements[7] = (gxINT32)0;
    font_name->Clear();
  }
  else {
    elements[7] = 1;
    font_name->SetString(font->GetFaceName().c_str()); 
  }

  // Write the font elements
  if(f->Write(elements, (sizeof(FAU) * FontElements), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  // Write the font name if this font has a face name
  if(elements[7] == 1) {
    address = GetFileAddress(f, name_address);
    if(address < (FAU_t)0) return 0;
    if(f->Write(font_name, sizeof(DBString), address) !=
       gxDBASE_NO_ERROR) {
      return 0;
    }
  }

  return 1;
}

int gxDatabaseConfig::GetGridLabelFont(wxFont *font)
{
  return get_font(grid_label_font, &grid_label_font_name, font);
}

int gxDatabaseConfig::WriteGridLabelFont(gxDatabase *f, wxFont *font)
{
  return write_font(f, GRID_LABEL_FONT, GRID_LABEL_FONT_NAME,
		    grid_label_font, &grid_label_font_name, font);
}

int gxDatabaseConfig::GetGridTextFont(wxFont *font)
{
  return get_font(grid_text_font, &grid_text_font_name, font);
}

int gxDatabaseConfig::WriteGridTextFont(gxDatabase *f, wxFont *font)
{
  return write_font(f, GRID_TEXT_FONT, GRID_TEXT_FONT_NAME,
		    grid_text_font, &grid_text_font_name, font);
}

int gxDatabaseConfig::GetHyperlinkFont(wxFont *font)
{
  return get_font(hyperlink_font, &hyperlink_font_name, font);
}

int gxDatabaseConfig::WriteHyperlinkFont(gxDatabase *f, wxFont *font)
{
  return write_font(f, HYPERLINK_FONT, HYPERLINK_FONT_NAME,
		    hyperlink_font, &hyperlink_font_name, font);
}

#endif

int gxDatabaseConfig::WritePrintGridLines(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_GRID_LINES);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_grid_lines, sizeof(print_grid_lines), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintDocName(gxDatabase *f, const char *s)
{
  if(!TestStaticArea(f)) return 0;
  if(!s) return 0;

  print_doc_name = s;
  FAU_t address = GetFileAddress(f, PRINT_DOC_NAME);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_doc_name, sizeof(print_doc_name), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WritePrintDocCustomHeader(gxDatabase *f, const char *s)
{
  if(!TestStaticArea(f)) return 0;
  if(!s) return 0;

  print_doc_custom_header = s;
  FAU_t address = GetFileAddress(f, PRINT_DOC_CUSTOM_HEADER);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_doc_custom_header, sizeof(print_doc_custom_header), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WritePrintDocCustomFooter(gxDatabase *f, const char *s)
{
  if(!TestStaticArea(f)) return 0;
  if(!s) return 0;

  print_doc_custom_footer = s;
  FAU_t address = GetFileAddress(f, PRINT_DOC_CUSTOM_FOOTER);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_doc_custom_footer, sizeof(print_doc_custom_footer), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WritePrintDocTimeAndDate(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_DOC_TIME_AND_DATE);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_doc_time_and_date, sizeof(print_doc_time_and_date), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintOrientation(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_ORIENTATION);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_orientation, sizeof(print_orientation), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintLeftMarginSize(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_LEFT_MARGIN_SIZE);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_left_margin_size, sizeof(print_left_margin_size), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintPaperSize(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_PAPER_SIZE);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_paper_size, sizeof(print_paper_size), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintPageHeader(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_PAGE_HEADER);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_page_header, sizeof(print_page_header), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintPageFooter(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_PAGE_FOOTER);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_page_footer, sizeof(print_page_footer), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintGridLabels(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_GRID_LABELS);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_grid_labels, sizeof(print_grid_labels), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintRowNumbers(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_ROW_NUMBERS);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_row_numbers, sizeof(print_row_numbers), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WritePrintPageScale(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;

  FAU_t address = GetFileAddress(f, PRINT_PAGE_SCALE);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&print_page_scale, sizeof(print_page_scale), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

#ifdef __wxWINALL__
int gxDatabaseConfig::GetPrintHeaderColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &print_header_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WritePrintHeaderColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, PRINT_HEADER_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&print_header_color, color_buf, 4);
  if(f->Write(&print_header_color, sizeof(print_header_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetPrintFooterColor(wxColour *color)
{
  unsigned char color_buf[4];
  memmove(color_buf, &print_footer_color, 4);
  if(color_buf[0] != 1) return 0; // The color was not set
  color->Set(color_buf[1], color_buf[2], color_buf[3]);
  return 1;
}

int gxDatabaseConfig::WritePrintFooterColor(gxDatabase *f, wxColour *color)
{
  FAU_t address = GetFileAddress(f, PRINT_FOOTER_COLOR);
  if(address < (FAU_t)0) return 0;
  unsigned char color_buf[4];
  color_buf[0] = 1;
  color_buf[1] = color->Red();
  color_buf[2] = color->Green();
  color_buf[3] = color->Blue();
  memmove(&print_footer_color, color_buf, 4);
  if(f->Write(&print_footer_color, sizeof(print_footer_color), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::GetPrintHeaderFont(wxFont *font)
{
  return get_font(print_header_font, &print_header_font_name, font);
}

int gxDatabaseConfig::WritePrintHeaderFont(gxDatabase *f, wxFont *font)
{
  return write_font(f, PRINT_HEADER_FONT, PRINT_HEADER_FONT_NAME,
		    print_header_font, &print_header_font_name, font);

}

int gxDatabaseConfig::GetPrintFooterFont(wxFont *font)
{
  return get_font(print_footer_font, &print_footer_font_name, font);
}

int gxDatabaseConfig::WritePrintFooterFont(gxDatabase *f, wxFont *font)
{
  return write_font(f, PRINT_FOOTER_FONT, PRINT_FOOTER_FONT_NAME,
		    print_footer_font, &print_footer_font_name, font);

}

#endif

int gxDatabaseConfig::WriteDBName(gxDatabase *f, const char *s)
{
  if(!TestStaticArea(f)) return 0;
  if(!s) return 0;

  database_name = s;
  FAU_t address = GetFileAddress(f, DATABASE_NAME);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&database_name, sizeof(database_name), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WriteColSize(gxDatabase *f, int p, int value)
{
  if(!TestStaticArea(f)) return 0;
  if(p > (NumDataMembers-1)) return 0;
  col_sizes[p] = (gxINT32)value;

  FAU_t address = GetFileAddress(f, COL_SIZES);
  if(address < (FAU_t)0) return 0;
  FAU offset = (FAU_t)(p * sizeof(FAU));
  address += offset;
  if(f->Write(&col_sizes[p], sizeof(FAU), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::WriteColSize(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;
  FAU_t address = GetFileAddress(f, COL_SIZES);
  if(address < (FAU_t)0) return 0;

  if(f->Write(&col_sizes, sizeof(col_sizes), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteColName(gxDatabase *f, int p, const char *s)
{
  if(!TestStaticArea(f)) return 0;
  if(p > NumDataMembers-1) return 0; // Check the boundaries
  if(!s) return 0;

  SetColName(p, s);

  FAU_t address = GetFileAddress(f, COLUMN_NAMES);
  if(address < (FAU_t)0) return 0;
  FAU offset = (FAU_t)(p*sizeof(DBString));
  address += offset;

  if(f->Write(&column_names[p], sizeof(DBString), address) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

char *gxDatabaseConfig::GetColName(int col, char *sbuf)
{
  // Check the boundaries
  if(col > NumDataMembers-1) return DBStringNULLPtr::DBStringNUllStr; 
  if(!sbuf) return DBStringNULLPtr::DBStringNUllStr;
  return column_names[col].c_str(sbuf);
}

void gxDatabaseConfig::SetColName(int col, const char *s)
{
  // Check the boundaries
  if(col > NumDataMembers-1) return;
  if(!s) return;
  column_names[col] = s;
}

void gxDatabaseConfig::SetDatabaseName(const char *s)
{
  if(!s) return;
  database_name = s;
}

void gxDatabaseConfig::SetPrintDocName(const char *s)
{
  if(!s) return;
  print_doc_name = s;
}

int gxDatabaseConfig::test_color(gxINT32 &c)
{
  unsigned char color_buf[4];
  memmove(color_buf, &c, 4);
  if(color_buf[0] != 1) {
    c = (gxINT32)0;
    return 0;
  }
  return 1;
}

int gxDatabaseConfig::is_string(const char *s)
{
  if(!s) return 0;

  int i;
  int is_string = 0; 
  int null_position = -1;

  for(i = 0; i < (int)DBStringLength; i++) {
    if(s[i] == 0) {
      null_position = i; // This is a null terminated string 
      if(i == 0) is_string = 1; // Allow null strings
      break; 
    }
    if((isgraph(s[i])) || 
       (s[i] == ' ')) {
      is_string++; // Count the number of printable characters
    }
    else {
      // The first non-printable character should be the null-terminator
      is_string = 0;
      break;
    }
  }
  if(null_position < 0) return 0;
  if(!is_string) return 0;

  return 1; 
}

int gxDatabaseConfig::Validate()
// Validate the config loaded from file and return false if a fatal 
// error is detected. The Caller must clear or reload the config if a 
// fatal error is encountered. Returns true if the config checks good with
// any non-fatal errors corrected.
{
  unsigned i;

  // Test the database name first. 
  // All valid datbase headers will have a name set.
  if(!is_string(&database_name)) return 0;

  // Validate the Grid label to prevent a program crash
  for(i = 0; i < (int)NumDataMembers; i++) {
    if(!is_string(&column_names[i])) column_names[i].Clear();
  }

  // NOTE: Valid all other string values here except font names
  if(!is_string(&print_doc_name)) print_doc_name.Clear();
  if(!is_string(&print_doc_custom_header)) print_doc_custom_header.Clear();
  if(!is_string(&print_doc_custom_footer)) print_doc_custom_footer.Clear();

  // Validate and correct the column sizes to prevent display 
  // problems if the header is corrupt.
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    // if(col_sizes[i] < (FAU)MinColSize) col_sizes[i] = (FAU)DefaultColSize;
    // if(col_sizes[i] > (FAU)MaxColSize) col_sizes[i] = (FAU)DefaultColSize;
    if(col_sizes[i] < (gxINT32)MinColSize) col_sizes[i] = (gxINT32)DefaultColSize;
    if(col_sizes[i] > (gxINT32)MaxColSize) col_sizes[i] = (gxINT32)DefaultColSize;
  }

  if((view_labels < (gxINT32)0) || (view_labels > 1)) view_labels = 1;
  if((view_row_numbers < (gxINT32)0) || (view_row_numbers > 1)) view_row_numbers = (gxINT32)0;
  if((view_grid_lines < (gxINT32)0) || (view_grid_lines > 1)) view_grid_lines = 1;
  if((auto_size < (gxINT32)0) || (auto_size > 1)) auto_size = 1;
  if((cell_overflow < (gxINT32)0) || (cell_overflow > 1)) cell_overflow = 1;
  if((view_skip_lines < (gxINT32)0) || (view_skip_lines > 1)) view_skip_lines = (gxINT32)0;


  int num_includes = 0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    if((print_field[i] < (gxINT32)0) || (print_field[i] > 1)) print_field[i] = 1;
    if(print_field[i] == 1) num_includes++;
  }
  if(num_includes == 0) { // Must include at least on field
    for(i = 0; i < (unsigned)NumDataMembers; i++) {
      print_field[i] = 1;
    }
  }
  num_includes = 0;
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    if((display_field[i] < (gxINT32)0) || (display_field[i] > 1)) display_field[i] = 1;
    if(display_field[i] == 1) num_includes++;
  }
  if(num_includes == 0) { // Must include at least on field
    for(i = 0; i < (unsigned)NumDataMembers; i++) {
      display_field[i] = 1;
    }
  }

  // NOTE: Validate all color values here
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(cell_color[i]);
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(cell_fill[i]);
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(cell_text_color[i]);
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(label_color[i]);
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(label_fill[i]);
  for(i = 0; i < (unsigned)NumDataMembers; i++) test_color(label_text_color[i]);
  test_color(grid_line_color);
  test_color(skip_line_color);
  test_color(grid_background_color);
  test_color(grid_text_color);
  test_color(grid_label_background_color);
  test_color(grid_label_text_color);
  test_color(hyperlink_color);
  test_color(print_header_color);
  test_color(print_footer_color);

  // NOTE: Validate all font values here
  if(grid_label_font[0] == 1) {
    if(!is_string(&grid_label_font_name)) grid_label_font_name.Clear();
  }
  if(grid_text_font[0] == 1) {
    if(!is_string(&grid_text_font_name)) grid_text_font_name.Clear();
  }
  if(hyperlink_font[0] == 1) {
    if(!is_string(&hyperlink_font_name)) hyperlink_font_name.Clear();
  }
  if(print_header_font[0] == 1) {
    if(!is_string(&print_header_font_name)) print_header_font_name.Clear();
  }
  if(print_footer_font[0] == 1) {
    if(!is_string(&print_footer_font_name)) print_footer_font_name.Clear();
  }

  // NOTE: Validate all extendted print values
  if((print_doc_time_and_date < (gxINT32)0) || (print_doc_time_and_date > 1)) print_doc_time_and_date = 1;
  if((print_grid_lines < (gxINT32)0) || (print_grid_lines > 1)) print_grid_lines = 1;
  if((print_page_header < (gxINT32)0) || (print_page_header > 1)) print_page_header = 1;
  if((print_page_footer < (gxINT32)0) || (print_page_footer > 1)) print_page_footer = 1;
  if((print_skip_lines < (gxINT32)0) || (print_skip_lines > 1)) print_skip_lines = 0;
  if((print_grid_labels < (gxINT32)0) || (print_grid_labels > 1)) print_grid_labels = 1;
  if((print_row_numbers < (gxINT32)0) || (print_row_numbers > 1)) print_row_numbers = 0;

  // Landscape = 0, Portrait = 1
  if((print_orientation < (gxINT32)0) || (print_orientation > 1)) print_orientation = 1;

  // Paper codes must match the PaperChoiceStrings array
  if((print_paper_size < (gxINT32)0) || (print_paper_size > 4)) print_paper_size = 0;

  if((print_left_margin_size < 1) || (print_left_margin_size > 20)) print_left_margin_size = 10; 

  if((print_page_scale < 25) || (print_page_scale > 100)) print_page_scale = 100;

  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    if(cell_text_fonts[i][0] == 1) {
      if(!is_string(&cell_text_font_names[i])) cell_text_font_names[i].Clear();
    }
  }
  for(i = 0; i < (unsigned)NumDataMembers; i++) {
    if(label_text_fonts[i][0] == 1) {
      if(!is_string(&label_text_font_names[i])) label_text_font_names[i].Clear();
    }
  }

  if(text_delimiter[0] != '\t') text_delimiter[0] = '\t';
  if(text_delimiter[1] != 0) text_delimiter[1] = 0;
  if(text_delimiter[2] != 0) text_delimiter[2] = 0;
  if(text_delimiter[3] != 0) text_delimiter[3] = 0;


  // Reset grid label parameters to account for auto sizing 
  if(auto_size == 1) {
    for(i = 0; i < (unsigned)NumDataMembers; i++) {
      if(column_names[i].is_null()) column_names[i] = " ";
    }
  }

  return 1;
}

int gxDatabaseConfig::Flush(gxDatabase *f)
{
  if(!TestStaticArea(f)) return 0;
  
  if(f->Flush() != gxDBASE_NO_ERROR) {
    return 0;
  }

  return 1;
}

int gxDatabaseConfig::WriteConfig(gxDatabase *f, int flushdb)
{
  if(!TestStaticArea(f)) return 0;
  
  if(f->Write(this, this->SizeOf(), (FAU_t)f->FileHeaderSize()) !=
     gxDBASE_NO_ERROR) {
    return 0;
  }

  if(flushdb) Flush(f);

  return 1;
}

unsigned gxDatabaseConfig::SizeOf()
{
  unsigned size = 0;
  
  size += sizeof(version_number);
  size += sizeof(database_name);
  size += sizeof(column_names);
  size += sizeof(view_labels);
  size += sizeof(view_row_numbers);
  size += sizeof(col_sizes);
  size += sizeof(print_field);
  size += sizeof(cell_attrib);
  size += sizeof(cell_align);
  size += sizeof(cell_border);
  size += sizeof(cell_color);
  size += sizeof(cell_fill);
  size += sizeof(cell_text_color);
  size += sizeof(cell_text_font);
  size += sizeof(cell_type);
  size += sizeof(cell_pattern);
  size += sizeof(cell_protection);
  size += sizeof(label_align);
  size += sizeof(label_border);
  size += sizeof(label_color);
  size += sizeof(label_fill);
  size += sizeof(label_text_font);
  size += sizeof(label_text_color);
  size += sizeof(label_pattern);
  size += sizeof(label_protection);
  size += sizeof(display_field);
  size += sizeof(cell_misc);
  size += sizeof(label_misc);
  size += sizeof(auto_size);
  size += sizeof(cell_overflow);
  size += sizeof(view_grid_lines); 
  size += sizeof(grid_line_color);
  size += sizeof(view_skip_lines);
  size += sizeof(skip_line_color);
  size += sizeof(grid_background_color);
  size += sizeof(grid_text_color);
  size += sizeof(grid_label_background_color);
  size += sizeof(grid_label_text_color);
  size += sizeof(hyperlink_color);
  size += sizeof(grid_label_font);
  size += sizeof(grid_label_font_name);
  size += sizeof(grid_text_font);
  size += sizeof(grid_text_font_name);
  size += sizeof(hyperlink_font);
  size += sizeof(hyperlink_font_name);
  size += sizeof(print_doc_name);
  size += sizeof(print_doc_time_and_date);
  size += sizeof(print_grid_lines);
  size += sizeof(print_orientation);
  size += sizeof(print_left_margin_size);
  size += sizeof(print_paper_size);
  size += sizeof(print_page_header);
  size += sizeof(print_doc_custom_header);
  size += sizeof(print_header_color);
  size += sizeof(print_header_font);
  size += sizeof(print_header_font_name);
  size += sizeof(print_page_footer);
  size += sizeof(print_doc_custom_footer);
  size += sizeof(print_footer_color);
  size += sizeof(print_footer_font);
  size += sizeof(print_footer_font_name);
  size += sizeof(print_skip_lines);
  size += sizeof(print_grid_labels);
  size += sizeof(print_row_numbers);
  size += sizeof(print_page_scale);
  size += sizeof(cell_text_fonts); 
  size += sizeof(cell_text_font_names);
  size += sizeof(label_text_fonts);
  size += sizeof(label_text_font_names);
  size += sizeof(text_delimiter);
  return size;
}

gxDatabaseParms::gxDatabaseParms()
{
  // Display status messages by default
  display_status = 1;

  // Database configuration and GUI member initialization
  WildCard1 = "*";
  
  // Default number of grid rows for empty databases. This number will be
  // automatically adjusted during import and merge operations.
  default_num_rows = 25;  
  column_label_size = 50; // Column label size
  row_label_size = 50;    // Row label size
  
  // Initialize the string arrays
  int i;
  for(i = 0; i < NumDataMembers; i++) dbgrid_string[i].Clear();
  for(i = 0; i < NumDataMembers; i++) {
    dbgrid_labels[i] << clear << "Member " << i;
  }
  
  // Set the default column names for this database
  // TODO: This must correspond to your total number of data
  // memebers for this database application.
  dbgrid_labels[0] = "Title";
  dbgrid_labels[1] = "Username";
  dbgrid_labels[2] = "Password";
  dbgrid_labels[3] = "Previous Password";
  dbgrid_labels[4] = "Security Questions";
  dbgrid_labels[5] = "Account Information";
  dbgrid_labels[6] = "Website Link";
  dbgrid_labels[7] = "Notes";
                      
  // Database specific configuration parameters stored with every datafile
  db_config.database_name = "EncryptedPasswordDatabase"; 

  for(i = 0; i < NumDataMembers; i++) {
    db_config.col_sizes[i] = (FAU)DefaultColSize;
  }
  for(i = 0; i < NumDataMembers; i++) {
    db_config.SetColName(i, dbgrid_labels[i].c_str());
  }
  adding_cell_keyname = 0;  
  changing_cell_keyname = 0;
  current_row = current_col = prev_row = prev_col = 0;

  // Database Interface
  pod = 0;
  admin_rights = 1;
  
  static_area = (FAU_t)(DB_AUTH_STATIC_AREA_SIZE * 2);
  num_trees = InfoHogNumTrees;
  node_order = InfoHogNodeOrder;
  data_file_extension = ".ehd";
  index_file_extension = ".ehx";
  data_file = "EncryptedPasswordDatabase";
  data_file += data_file_extension;
  index_file = "EncryptedPasswordDatabase";
  index_file += index_file_extension;
  database_revision = 'E'; // Database revision letter
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
