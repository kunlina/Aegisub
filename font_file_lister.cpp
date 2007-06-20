// Copyright (c) 2007, Niels Martin Hansen, Rodrigo Braz Monteiro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------
//
// AEGISUB
//
// Website: http://aegisub.cellosoft.com
// Contact: mailto:zeratul@cellosoft.com
//


#pragma once


////////////
// Includes
#include <wx/dir.h>
#ifdef WIN32
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_SFNT_NAMES_H
#include <shlobj.h>
#endif
#include "font_file_lister.h"
#include "text_file_writer.h"
#include "text_file_reader.h"


////////////////////
// Static instances
FontFileLister *FontFileLister::instance = NULL;


///////////////
// Constructor
FontFileLister::FontFileLister() {
#ifdef WIN32
	// Initialize freetype2
	FT_Init_FreeType(&ft2lib);
#endif
}


//////////////
// Destructor
FontFileLister::~FontFileLister() {
}


////////////////////////////////////////
// Get instance and call function there
wxArrayString FontFileLister::GetFilesWithFace(wxString facename) {
	if (!instance) instance = new FontFileLister();
	return instance->DoGetFilesWithFace(facename);
}
void FontFileLister::GatherData() {
	if (!instance) instance = new FontFileLister();
	instance->DoGatherData();
}
void FontFileLister::ClearData() {
	if (!instance) instance = new FontFileLister();
	instance->DoClearData();
}


////////////////////////////////////////////////
// Get list of files that match a specific face
wxArrayString FontFileLister::DoGetFilesWithFace(wxString facename) {
	FontMap::iterator iter = fontTable.find(facename);
	if (iter != fontTable.end()) return iter->second;
	else return wxArrayString();
}


//////////////
// Clear data
void FontFileLister::DoClearData() {
	fontFiles.clear();
	fontTable.clear();
}


///////////////////////////
// Gather data from system
void FontFileLister::DoGatherData() {
#ifdef WIN32

	// Get fonts folder
	wxString source;
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_FONTS,NULL,0,szPath))) {
		source = wxString(szPath);
	}
	else source = wxGetOSDirectory() + _T("\\fonts");
	source += _T("\\");

	// Get the list of fonts in the fonts folder
	wxArrayString fontfiles;
	wxDir::GetAllFiles(source, &fontfiles, wxEmptyString, wxDIR_FILES);

	// Loop through each file
	int fterr;
	for (unsigned int i=0;i<fontfiles.Count(); i++) {
		// Check if it's cached
		if (IsFilenameCached(fontfiles[i])) continue;

		// Loop through each face in the file
		for (int facenum=0;true;facenum++) {
			// Get font face
			FT_Face face;
			fterr = FT_New_Face(ft2lib, fontfiles[i].mb_str(*wxConvFileName), facenum, &face);
			if (fterr) break;

			// Add font
			AddFont(fontfiles[i],wxString(face->family_name, wxConvLocal));
			FT_Done_Face(face);
		}
	}

#else

	// TODO: implement fconfig
	return;

#endif
}


////////////
// Add font
void FontFileLister::AddFont(wxString filename,wxString facename) {
	// Add filename to general list
	if (fontFiles.Index(filename) == wxNOT_FOUND) {
		fontFiles.Add(filename);
	}

	// Add filename to mapping of this face
	wxArrayString &arr = fontTable[facename];
	if (arr.Index(filename) == wxNOT_FOUND) arr.Add(filename);
}


/////////////////////////////////
// Check if a filename is cached
bool FontFileLister::IsFilenameCached(wxString filename) {
	return fontFiles.Index(filename) != wxNOT_FOUND;
}


//////////////
// Save cache
void FontFileLister::SaveCache() {
	// TODO
}


//////////////
// Load cache
void FontFileLister::LoadCache() {
	// TODO
}
