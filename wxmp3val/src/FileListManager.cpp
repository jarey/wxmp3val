/*
 * This file is part of the wxMP3val and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "FileListManager.h"
#include "Constants.h"

#include <wx/tokenzr.h>
#include <wx/dir.h>

FileListManager::FileListManager(wxListCtrl *owner) : mp_owner(owner) {
    mp_lstFilesData = new std::list<FileInfo>();
}

FileListManager::~FileListManager() {
    delete mp_lstFilesData;
}

void FileListManager::insertFilesAndDir(const wxArrayString &filenames) {
    wxArrayString files;

    // Check if is a directory or a file
    for (unsigned long int n = 0; n < filenames.GetCount(); n++) {
        wxString filename = filenames[n];
        if (wxFileName::DirExists(filename))
            insertDir(filename);
        else
            files.Add(filename);
    }
    insertFiles(files);
}

void FileListManager::insertFiles(const wxArrayString &filenames) {
    unsigned long int nFiles = filenames.GetCount();

    // Add files in wxListCtrl
    for (unsigned long int n = 0; n < nFiles; n++) {
        wxFileName file(filenames[n]);

        if (checkValidExtension(file)) {
            // Don't insert repeated filenames
            bool repeated = false;

            unsigned long int i = 0;
            for (std::list<FileInfo>::iterator fileInfo = mp_lstFilesData->begin();
                 fileInfo != mp_lstFilesData->end(); fileInfo++, i++) {
                wxFileName filenameInput = (*fileInfo).getFileName();
                if (filenameInput.GetFullPath() == filenames[n]) {
                    repeated = true;
                }
            }
            if (!repeated) {
                mp_owner->InsertItem(mp_owner->GetItemCount(), file.GetFullName());
                mp_owner->SetItem(i, 1, file.GetPath());
                mp_lstFilesData->push_back(FileInfo(filenames[n]));
            }
        }
    }
}

void FileListManager::insertDir(const wxString &dirname) {
    wxArrayString files;
    wxDir::GetAllFiles(dirname, &files);

    insertFiles(files);
}

bool FileListManager::checkValidExtension(const wxFileName &file) const {
    wxStringTokenizer strToken(APP_OPEN_EXT, _T(";"));
    while (strToken.HasMoreTokens()) {
        wxString token = strToken.GetNextToken();

        if (file.GetExt().CmpNoCase(token) == 0)
            return true;
    }
    return false;
}

void FileListManager::deleteItem(unsigned long int index) {
    std::list<FileInfo>::iterator fileInfo = mp_lstFilesData->begin();
    std::advance(fileInfo, index);
    mp_lstFilesData->erase(fileInfo);
}

void FileListManager::clear() {
    mp_owner->DeleteAllItems();
    mp_lstFilesData->clear();
}

long unsigned int FileListManager::size() {
    return mp_lstFilesData->size();
}

FileInfo &FileListManager::getItem(unsigned long int index) {
    std::list<FileInfo>::iterator fileInfo = mp_lstFilesData->begin();
    std::advance(fileInfo, index);
    return *fileInfo;
}

wxListCtrl &FileListManager::getOwner() {
    return *mp_owner;
}
