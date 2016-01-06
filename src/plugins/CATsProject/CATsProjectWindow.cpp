// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Tomas Marek, xmarek11@stud.fit.vutbr.cz,
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

#include <ProjectList.hpp>
#include <Project.hpp>
#include <ProjectStatusBar.hpp>

#include "CATsProjectWindow.hpp"
#include "CATsProjectWindow.moc"
#include "CATsProject.hpp"
#include "highlighter.h"
#include <QAction>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
//#ifndef QT_NO_SCRIPTTOOLS
#include <QtScriptTools/QScriptEngineDebugger>
//#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCATsProjectWindow::CCATsProjectWindow(CCATsProject* p_project)
    : CMainWindow(p_project)
{/*
    // init ui part
    WidgetUI.setupUi(this);

    // shortcuts
    Project = p_project;

    // status bar
//    ProjectStatusBar = new CProjectStatusBar(this,Project);
//    setStatusBar(ProjectStatusBar);*/
    
    WidgetUI.setupUi(this);
    setupMenu();
    //setupHelpMenu();
    setupEditor();   

    workingDir = "";
    currentFile = "";
}

void CCATsProjectWindow::setupMenu()
{
    //WidgetUI.menuBar->show();
    //WidgetUI.mainToolBar->show();
    //QAction::setVisible(WidgetUI.mainToolBar);
    //WidgetUI.mainToolBar->setVisible(true);
    WidgetUI.toolBar->setVisible(true);
/*
    if (WidgetUI.menuBar->isVisible())
        WidgetUI.plainTextEdit->setPlainText("VISIBLE");
    else
        WidgetUI.plainTextEdit->setPlainText("INVISIBLE");
*/
    connect(WidgetUI.action_Open, SIGNAL(triggered()), this, SLOT(loadFile()));
    connect(WidgetUI.action_Save_script, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(WidgetUI.actionSave_script_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(WidgetUI.action_Exit, SIGNAL(triggered()), this, SLOT(exitProgram()));
    connect(WidgetUI.actionChange_working_directory, SIGNAL(triggered()), this, SLOT(setWorkingDirectory()));
    connect(WidgetUI.actionRun_script, SIGNAL(triggered()), this, SLOT(runScript()));
    connect(WidgetUI.actionDebug, SIGNAL(triggered()), this, SLOT(debugScript()));
    connect(WidgetUI.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(refreshTabs()));
}

void CCATsProjectWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    //editor = new QPlainTextEdit;
    editor = WidgetUI.plainTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());
}

void CCATsProjectWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));
    if (fileName == "")
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    WidgetUI.plainTextEdit->setPlainText(in.readAll());

    currentFile = fileName.toStdString();

    if (workingDir == "")
    {
        QFileInfo info(fileName);
        workingDir = info.filePath().toStdString();
    }
}

void CCATsProjectWindow::saveFile()
{
    QString fileName = QString::fromStdString(currentFile);

    if (fileName == "")
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));

        if (fileName == "")
        {
            return;
        }
        else
        {
            currentFile = fileName.toStdString();
        }
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << WidgetUI.plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void CCATsProjectWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));

    if (fileName == "")
    {
        return;
    }
    else
    {
        currentFile = fileName.toStdString();
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << WidgetUI.plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void CCATsProjectWindow::setWorkingDirectory()
{
    QString dirName = QFileDialog::getExistingDirectory(this,tr("Choose working directory..."), QString::fromStdString(workingDir));

    if (dirName == "")
    {
        return;
    }
    else
    {
        workingDir = dirName.toStdString();
    }
}

QScriptValue QtPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
    QString result;
    for (int i = 0; i < context->argumentCount(); ++i) {
        if (i > 0)
            result.append(" ");
        result.append(context->argument(i).toString());
    }

    QScriptValue calleeData = context->callee().data();
    QTextBrowser *edit = qobject_cast<QTextBrowser*>(calleeData.toQObject());
    edit->append(result);

    return engine->undefinedValue();
}

void CCATsProjectWindow::runScript()
{
    WidgetUI.textBrowser->clear();

    QScriptEngine JSEngine;

    QScriptValue printFunction = JSEngine.newFunction(QtPrintFunction);

    printFunction.setData(JSEngine.newQObject(WidgetUI.textBrowser));
    JSEngine.globalObject().setProperty("print", printFunction);

    QScriptValue result = JSEngine.evaluate(WidgetUI.plainTextEdit->toPlainText());

    if (JSEngine.hasUncaughtException())
    {
        int line = JSEngine.uncaughtExceptionLineNumber();
        QString err = result.toString();
        QString msg = QString("Error at line %1: %2").arg(line).arg(err);

        WidgetUI.tabWidget->setCurrentIndex(0);

        console_content = msg;

        refreshTabs();
    }
    else
    {
        //output_content = result.toString();
        output_content = WidgetUI.textBrowser->document()->toPlainText();

        WidgetUI.tabWidget->setCurrentIndex(1);

        refreshTabs();
    }
}

void CCATsProjectWindow::debugScript()
{
    QScriptEngine JSEngine;

    QScriptValue printFunction = JSEngine.newFunction(QtPrintFunction);

    printFunction.setData(JSEngine.newQObject(WidgetUI.textBrowser));
    JSEngine.globalObject().setProperty("print", printFunction);

    QScriptEngineDebugger* debugger = new QScriptEngineDebugger();
    debugger->attachTo(&JSEngine);
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    JSEngine.evaluate(WidgetUI.plainTextEdit->toPlainText());
}

void CCATsProjectWindow::refreshTabs()
{
    if (WidgetUI.tabWidget->currentIndex() == 0)
    {
        WidgetUI.textBrowser->setText(console_content);
    }

    if (WidgetUI.tabWidget->currentIndex() == 1)
    {
        WidgetUI.textBrowser->setText(output_content);
    }

    if (WidgetUI.tabWidget->currentIndex() == 2)
    {
        WidgetUI.textBrowser->setText(debugger_tools_content);
    }

    if (WidgetUI.tabWidget->currentIndex() == 3)
    {
        WidgetUI.textBrowser->setText(stack_view_content);
    }
}

void CCATsProjectWindow::exitProgram()
{
    this->close();
}

//------------------------------------------------------------------------------

CCATsProjectWindow::~CCATsProjectWindow(void)
{

}

//------------------------------------------------------------------------------

void CCATsProjectWindow::ConnectAllMenu(void)
{
    // restore desktop was already called

}

//------------------------------------------------------------------------------

void CCATsProjectWindow::keyPressEvent(QKeyEvent* p_event)
{
    if( p_event->key() == Qt::Key_Escape ){
//        JobScheduler->TerminateProjectRunningJobs(Project);
    }
    CMainWindow::keyPressEvent(p_event);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



