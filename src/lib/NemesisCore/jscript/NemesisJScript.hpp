#ifndef NemesisJScriptH
#define NemesisJScriptH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <QScriptEngine>
#include <QTextStream>
#include <NemesisOptions.hpp>
#include <TerminalOStream.hpp>
#include <QOpenGLDebugLogger>

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CNemesisJScript : public QScriptEngine {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CNemesisJScript(void);
    ~CNemesisJScript(void);

// main methods ---------------------------------------------------------------
    /// init engine, e.g. register objects, constructors, etc.
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

    /// post close event to MainWindow
    void PostCloseEvent(void);

    /// parse command arguments
    void ParseArguments(void);

// section of signals ----------------------------------------------------------
signals:
    void SetupLevelChanged(const QString& text);
    void SetupNotificationChanged(const QString& text);
    void OnPostCloseEvent(void);

public slots:
    /// close all projects and quit
    void QuitApplication(void);

    /// quit application when no project is opened
    void ShallQuitApplication(void);

    /// plugin loaded
    void PluginLoaded(const QString& base_name);

    /// increment counter of initializations steps
    void IncrementProgressNoGUI(const QString& text);

    /// receive OpenGLDebug message
    void OpenGLMessageLogged(const QOpenGLDebugMessage& debugMessage);

// section of private data -----------------------------------------------------
private:
    QString         Contents;   // command line contents
    int             InitStatus; // initialization status

    /// print welcome text to console
    void PrintWelcomeText(void);

    /// print final text to console
    void PrintFinalText(void);

    /// init nemesis subsystems
    int  InitSubsystems(void);

    /// finalize all subsystems
    void FinalizeSubsystems(void);

    /// register all JScript objects and constructors
    void RegisterObjects(void);

    /// print import formats
    void PrintIFormats(void);

    /// print openbabel format extensions
    void PrintOBExtensions(void);
};

//------------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CNemesisJScript* NemesisJScript;

//standard output stream redirected to specified terminal
extern NEMESIS_CORE_PACKAGE CTerminalOStream tout;

//------------------------------------------------------------------------------

#endif
