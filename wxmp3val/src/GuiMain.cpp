#include "GuiMain.h"
#include "GuiSettings.h"
#include "Progress.h"
#include "Global.h"

#include <wx/aboutdlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>

GuiMain::GuiMain(wxWindow* parent)
: Main(parent) {
    // Disable status bar pane used to display menu and toolbar help
    SetStatusBarPane(-1);

    // Aux list for wxListctrl
    mp_lstFilesData = new ArrayOfFiles();

    // Support Drag & Drop
    mp_dndFile = new DndFile(g_lstFiles, mp_lstFilesData);
    g_lstFiles->SetDropTarget(mp_dndFile);

    // Configuration file
    mp_configBase = new ConfigBase(APP_NAME);

    // Window title
    SetTitle(APP_NAME_WITH_VERSION);

    // Load resource
    loadResources();

    // Title List
    g_lstFiles->InsertColumn(0, _("File"), wxLIST_FORMAT_LEFT, 350);
    g_lstFiles->InsertColumn(1, _("Version"), wxLIST_FORMAT_LEFT, 100);
    g_lstFiles->InsertColumn(2, _("Tags"), wxLIST_FORMAT_LEFT, 100);
    g_lstFiles->InsertColumn(3, _("CBR"), wxLIST_FORMAT_LEFT, 80);
    g_lstFiles->InsertColumn(4, _("State"), wxLIST_FORMAT_LEFT, 80);

    // Updates the controls
    updateControls();
}

GuiMain::~GuiMain() {
    delete mp_lstFilesData;
    delete mp_configBase;
}

void GuiMain::OnlstFilesDeleteItem(wxListEvent& event) {
    mp_lstFilesData->Detach(event.GetIndex());
    updateControls();
    event.Skip();
}

void GuiMain::OnlstFilesInsertItem(wxListEvent& event) {
    /*
     * :KLUDGE:
     * EVT_LIST_INSERT_ITEM is triggered before or after item is added:
     *   on wxGTK -> triggered before item is added;
     *   on wxMSW -> triggered after item is added.
     */
    updateControlsDelayed();
    event.Skip();
}

void GuiMain::OnlstFilesItemSelect(wxListEvent& event) {
    updateControls();
    event.Skip();
}

void GuiMain::OnlstFilesItemRClick(wxListEvent& event) {
    updateControls();

    // Displays the popup menu when you click a list item
    g_lstFiles->PopupMenu(g_mainMenu);
    event.Skip();
}

void GuiMain::OnlstFilesKeyDown(wxListEvent& event) {
    // Remove files with Delete key
    int keyCode = event.GetKeyCode();
    if (keyCode == WXK_DELETE)
        mnuRemoveFiles(event);

    event.Skip();
}

void GuiMain::mnuAddDirectory(wxCommandEvent& event) {
    wxDirDialog dirDialog(this, _("Select directory"), wxEmptyString, wxDD_DEFAULT_STYLE);

    // Read the last directory used
    dirDialog.SetPath(mp_configBase->getLastOpenDir());
    if (dirDialog.ShowModal() == wxID_OK) {
        SetCursor(wxCURSOR_WAIT);
        mp_dndFile->insertFileListDir(dirDialog.GetPath());

        // Remembers the last used directory
        mp_configBase->setLastOpenDir(dirDialog.GetPath());
        SetCursor(wxCURSOR_ARROW);
    }
}

void GuiMain::mnuAddFiles(wxCommandEvent& event) {
    wxArrayString files;
    wxFileDialog fileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, APP_WILDCARD_EXT, wxFD_OPEN | wxFD_MULTIPLE);

    // Read the last directory used
    fileDialog.SetDirectory(mp_configBase->getLastOpenDir());

    if (fileDialog.ShowModal() == wxID_OK) {
        SetCursor(wxCURSOR_WAIT);

        // Get the file(s) the user selected
        fileDialog.GetPaths(files);
        mp_dndFile->insertFileList(files);

        // Remembers the last used directory
        mp_configBase->setLastOpenDir(fileDialog.GetDirectory());
        SetCursor(wxCURSOR_ARROW);
    }
}

void GuiMain::mnuExit(wxCommandEvent& event) {
    // Terminates the program
    Close();
}

void GuiMain::mnuRemoveFiles(wxCommandEvent& event) {
    int itemCount = g_lstFiles->GetSelectedItemCount();
    SetCursor(wxCURSOR_WAIT);
    for (int i = 0; i < itemCount; i++)
        g_lstFiles->DeleteItem(g_lstFiles->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED));
    SetCursor(wxCURSOR_ARROW);

    updateControls();
}

void GuiMain::mnuClearList(wxCommandEvent& event) {
    // Deletes all items from the list
    g_lstFiles->DeleteAllItems();
    mp_lstFilesData->Clear();

    updateControls();
}

void GuiMain::mnuSettings(wxCommandEvent& event) {
    // Displays the "Settings" window
    GuiSettings guiSettings(this, mp_configBase);
    guiSettings.ShowModal();

    updateControls();
}

void GuiMain::mnuScan(wxCommandEvent& event) {
    // Displays the "Progress" window
    Progress progressDialog(this, mp_configBase, g_lstFiles, mp_lstFilesData, TOOL_SCAN);
    progressDialog.execute();
}

void GuiMain::mnuRepair(wxCommandEvent& event) {
    // Displays the "Progress" window
    Progress progressDialog(this, mp_configBase, g_lstFiles, mp_lstFilesData, TOOL_FIX);
    progressDialog.execute();
}

void GuiMain::mnuWebsite(wxCommandEvent& event) {
    wxLaunchDefaultBrowser(APP_WEBSITE);
}

void GuiMain::mnuToolWebsite(wxCommandEvent& event) {
    wxLaunchDefaultBrowser(_T("http://mp3val.sourceforge.net/"));
}

void GuiMain::mnuAbout(wxCommandEvent& event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(APP_NAME);
    aboutInfo.SetVersion(APP_VERSION);
    aboutInfo.SetDescription(_("Free front-end for the MP3val"));
    aboutInfo.SetCopyright(APP_COPYRIGHT);
    aboutInfo.SetWebSite(APP_WEBSITE);
    aboutInfo.AddDeveloper(APP_AUTHOR);
    wxAboutBox(aboutInfo);
}

void GuiMain::OnTimer1Trigger(wxTimerEvent& event) {
    updateControls();
}

void GuiMain::loadResources() {
    wxString resourceDir = GetResourceDir();

    // Window icon
    wxIcon FrameIcon;
    FrameIcon.CopyFromBitmap(wxBitmap(wxImage(resourceDir + _T("icon2.ico"))));
    SetIcon(FrameIcon);

    // Toolbar bitmaps
    g_mainToolBar->SetToolNormalBitmap(ID_ADD_FILES, wxBitmap(wxImage(resourceDir + _T("toolbar/add.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_REMOVE_FILES, wxBitmap(wxImage(resourceDir + _T("toolbar/remove.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_CLEAR_LIST, wxBitmap(wxImage(resourceDir + _T("toolbar/clear.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_SCAN, wxBitmap(wxImage(resourceDir + _T("toolbar/scan.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_REPAIR, wxBitmap(wxImage(resourceDir + _T("toolbar/fix.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_SETTINGS, wxBitmap(wxImage(resourceDir + _T("toolbar/settings.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_ABOUT, wxBitmap(wxImage(resourceDir + _T("toolbar/about.png"))));
    g_mainToolBar->SetToolNormalBitmap(ID_ADD_FOLDER, wxBitmap(wxImage(resourceDir + _T("toolbar/folder.png"))));
}

void GuiMain::updateControls() {
    wxString newExeTool = mp_configBase->getToolExecutable();
    if (!m_exeTool.IsSameAs(newExeTool, false)) {
        m_exeInputString.Clear();
        m_exeInputErrorString.Clear();
        m_exeTool = newExeTool;
        // Execute external application
        wxExecute(m_exeTool + _T(" -v"), m_exeInputString, m_exeInputErrorString, wxEXEC_NODISABLE);

        // Show the version of tool
        if (!m_exeInputString.IsEmpty())
            g_mainStatusBar->SetStatusText(_("Using MP3val version: ") + m_exeInputString.Item(0).AfterLast(' '), 0);
        else
            g_mainStatusBar->SetStatusText(_("MP3val not found!"), 0);
    }

    // Show the number of files in list on status bar
    g_mainStatusBar->SetStatusText(wxString::Format(_T("%i "), g_lstFiles->GetItemCount()) + _("files"), 1);

    // Disables the menu item "Remove files" if no item is selected
    g_mainMenu->Enable(ID_REMOVE_FILES, g_lstFiles->GetSelectedItemCount() > 0);
    g_mainMenuBar->Enable(ID_REMOVE_FILES, g_lstFiles->GetSelectedItemCount() > 0);
    g_mainToolBar->EnableTool(ID_REMOVE_FILES, g_lstFiles->GetSelectedItemCount() > 0);

    // Disables the menu item "Clear list" if there is no item in the list
    g_mainMenu->Enable(ID_CLEAR_LIST, g_lstFiles->GetItemCount() > 0);
    g_mainMenuBar->Enable(ID_CLEAR_LIST, g_lstFiles->GetItemCount() > 0);
    g_mainToolBar->EnableTool(ID_CLEAR_LIST, g_lstFiles->GetItemCount() > 0);

    // Disables menus Scan and Repair case there is no item in the list
    g_mainMenuBar->Enable(ID_SCAN, g_lstFiles->GetItemCount() > 0);
    g_mainMenuBar->Enable(ID_REPAIR, g_lstFiles->GetItemCount() > 0);
    g_mainToolBar->EnableTool(ID_SCAN, g_lstFiles->GetItemCount() > 0);
    g_mainToolBar->EnableTool(ID_REPAIR, g_lstFiles->GetItemCount() > 0);
}

void GuiMain::updateControlsDelayed() {
    m_timer1.Start(10, true);
}

void GuiMain::setFilesCmdLine(const wxArrayString& filenames) {
    mp_dndFile->OnDropFiles(0, 0, filenames);
}