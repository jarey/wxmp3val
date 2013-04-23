/*
 * This file is part of the wxMP3gain and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PROGRESS_H
#define PROGRESS_H

#include "ConfigBase.h"
#include "Global.h"

#include <wx/listctrl.h>

enum
{
    TOOL_SCAN = 0,
    TOOL_FIX = 1,
};

class Progress
{
public:
    Progress(wxWindow* parent, ConfigBase* configBase, wxListCtrl* listFiles, int workType);
    virtual ~Progress();
    void Execute();

protected:

private:
    void ProcessFile(int fileIterator);
    void ProcessOutputString(int fileIterator);
    wxWindow* parent;
    ConfigBase* configBase;
    wxListCtrl* listFiles;
    int workType;
    wxArrayString inputString;
};

#endif
