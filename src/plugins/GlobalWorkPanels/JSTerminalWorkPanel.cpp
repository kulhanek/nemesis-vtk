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

#include <QtGui>

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <NemesisJScript.hpp>
#include <QVBoxLayout>

#include "JSTerminalWorkPanel.moc"
#include "JSTerminalWorkPanel.hpp"
#include "GlobalWorkPanelsModule.hpp"

#include <TerminalWidget.hpp>
#include <TerminalOStream.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* JSTerminalWorkPanelCB(void* p_data);

CExtUUID        JSTerminalWorkPanelID(
                    "{JS_TERMINAL_WORK_PANEL:4329f25a-6268-4478-a9c1-7bbb3dd44c20}",
                    "Java Script Console");

CPluginObject   JSTerminalWorkPanelObject(&GlobalWorkPanelsPlugin,
                    JSTerminalWorkPanelID,WORK_PANEL_CAT,
                    ":/images/GlobalWorkPanels/JavaScriptConsole.svg",
                    JSTerminalWorkPanelCB);

// -----------------------------------------------------------------------------

CJSTerminalWorkPanel* p_jst_window = NULL;

// -----------------------------------------------------------------------------

QObject* JSTerminalWorkPanelCB(void* p_data)
{
    if( p_jst_window != NULL ) return(p_jst_window);
    p_jst_window = new CJSTerminalWorkPanel;
    return(p_jst_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJSTerminalWorkPanel::CJSTerminalWorkPanel(void)
    : CWorkPanel(&JSTerminalWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    DefaultPrompt = "[nms]$ ";
    ContPrompt = "> ";

    Terminal = new CTerminalWidget(this,DefaultPrompt);
    tout.SetDevice(Terminal);

    QVBoxLayout* p_layout = new QVBoxLayout(this);
    p_layout->addWidget(Terminal);
    p_layout->setContentsMargins(0,0,0,0);
    setLayout(p_layout);

    connect(Terminal,SIGNAL(OnNewLineEntered(const QString&)),
            this,SLOT(NewLineEntered(const QString&)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CJSTerminalWorkPanel::~CJSTerminalWorkPanel()
{
    tout.SetDevice(&cout);
    SaveWorkPanelSetup();
    p_jst_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CJSTerminalWorkPanel::NewLineEntered(const QString& line)
{
    if( NemesisJScript == NULL ) return;

    CTerminalOStream stream;
    stream.SetDevice(Terminal);

    Contents += line;

    // test syntax
    QScriptSyntaxCheckResult syntax = QScriptEngine::checkSyntax(Contents);
    switch(syntax.state()) {
    case QScriptSyntaxCheckResult::Error:
        if( syntax.errorMessage() != "" ) {
            stream << "nemesis: " << CSmallString(syntax.errorMessage()) << endl;
        } else {
            stream << "nemesis: syntax error" << endl;
        }

        Contents = "";
        Terminal->SetPrompt(DefaultPrompt);
        break;

    case QScriptSyntaxCheckResult::Intermediate:
        Terminal->SetPrompt(ContPrompt);
        break;

    case QScriptSyntaxCheckResult::Valid:
        QScriptValue rvalue = NemesisJScript->evaluate(Contents);
        if( NemesisJScript->hasUncaughtException() ) {
            stream << "nemesis: " << CSmallString(rvalue.toString()) << endl;
        }
        Contents = "";
        Terminal->SetPrompt(DefaultPrompt);
        break;
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

