#ifndef CATsProjectWindowH
#define CATsProjectWindowH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <MainWindow.hpp>
#include "ui_CATsProjectWindow.h"
#include "highlighter.h"

#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <fstream>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptable>

//------------------------------------------------------------------------------

class CCATsProject;
class CProjectStatusBar;
class CGraphicsPrimaryView;

//------------------------------------------------------------------------------
/*
class CCATsProjectWindow : public CMainWindow {
    Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CCATsProjectWindow(CCATsProject* p_project);
    virtual ~CCATsProjectWindow(void);

    /// connect all menu - do not call it in constructor
    // it requires restored desktop
    void ConnectAllMenu(void);

// section of private data -----------------------------------------------------
private:
    CCATsProject*          Project;
    CGraphicsPrimaryView*   OpenGL;
    CProjectStatusBar*      ProjectStatusBar;
    Ui::CATsProjectWindow  WidgetUI;

// events -----------------------------------
    // process ESC key to stop project running jobs
    virtual void keyPressEvent(QKeyEvent* p_event);
};*/

class CCATsProjectWindow : public CMainWindow
{
    Q_OBJECT

public:
    // constructor and destructors -------------------------------------------------
        CCATsProjectWindow(CCATsProject* p_project);
        virtual ~CCATsProjectWindow(void);

        /// connect all menu - do not call it in constructor
        // it requires restored desktop
        void ConnectAllMenu(void);
    //CCATsProjectWindow(QWidget *parent = 0);

public slots:
        void loadFile();
        void saveFile();
        void saveFileAs();
        void exitProgram();
        void setWorkingDirectory();
        void runScript();
        void debugScript();
        void refreshTabs();
    //void about();
    //void newFile();
    //void openFile(const QString &path = QString());

private:
    CCATsProject*          Project;
    CGraphicsPrimaryView*   OpenGL;
    CProjectStatusBar*      ProjectStatusBar;
    Ui::MainWindow  WidgetUI;
    void setupEditor();
    void setupMenu();

    std::string workingDir;
    std::string currentFile;
    QString console_content;
    QString output_content;
    QString debugger_tools_content;
    QString stack_view_content;
    //void setupHelpMenu();
    //CCATsProjectWindow *ui;

    QPlainTextEdit *editor;
    Highlighter *highlighter;

    virtual void keyPressEvent(QKeyEvent* p_event);
};

//------------------------------------------------------------------------------

#endif
