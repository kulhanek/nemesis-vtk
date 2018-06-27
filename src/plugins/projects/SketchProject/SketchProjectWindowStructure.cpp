// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QWebEnginePage>
#include <QMessageBox>
#include <Structure.hpp>
#include <Graphics.hpp>
#include <GraphicsView.hpp>
#include <TextDialog.hpp>
#include <QMimeData>

#include "SketchProjectWindow.hpp"
#include "SketchProjectJSObject.hpp"
#include "SketchProject.hpp"

#include <iostream>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::ConnectStructureMenu(void)
{
    connect(WidgetUI.actionInsertSMILES,SIGNAL(triggered(bool)),
            this,SLOT(InsertSMILES(void)));
    //-----------------
    connect(WidgetUI.actionShowSMILES,SIGNAL(triggered(bool)),
            this,SLOT(ShowSMILES(void)));
    //-----------------
    connect(WidgetUI.actionZoomIn,SIGNAL(triggered(bool)),
            this,SLOT(ZoomIn(void)));
    //-----------------
    connect(WidgetUI.actionZoomOut,SIGNAL(triggered(bool)),
            this,SLOT(ZoomOut(void)));
    //-----------------
    connect(WidgetUI.actionClean,SIGNAL(triggered(bool)),
            this,SLOT(CleanStructure(void)));
    //-----------------
    connect(WidgetUI.actionConvertTo3D,SIGNAL(triggered(bool)),
            this,SLOT(ConvertTo3D(void)));
    //-----------------
    connect(WidgetUI.actionCopyAsSVG,SIGNAL(triggered(bool)),
            this,SLOT(CopyAsSVG(void)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::InsertSMILES(void)
{
    // get smiles from user
    QString new_smiles;
    CTextDialog* p_dialog = new CTextDialog(tr("Insert SMILES structure"),Project);
    if( p_dialog->ShowAsDialog() == QDialog::Accepted ){
        new_smiles = p_dialog->GetText();
    }
    delete p_dialog;
    if( new_smiles.isEmpty() ) return;

    // get current smiles
    QString current_smiles = JSObject->getSMILESData();

    // mix smiles
    QString final_smiles;

    if( current_smiles.isEmpty() ){
        final_smiles = new_smiles;
    } else {
        final_smiles = current_smiles + "." + new_smiles;
    }

    // layout data
    QString cleaned_mol = JSObject->cleanProject(final_smiles);

    // update view
    QStringList script;
    script << "var data=[";

    QStringList molsep = cleaned_mol.split("\n");
    for(int i =0; i < molsep.size(); i++){
        if( i+1 != molsep.size() ){
            script << "'" + molsep[i] + "',";
        } else {
            script << "'" + molsep[i] + "'";
        }
    }
    script << "];";
    script << "ui.loadMolecule(data.join('\\n'),false);";

    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript(script.join("\n"));
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::ShowSMILES(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_ShowSMILES.call($('show_smiles'));");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectWindow::ZoomIn(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_ZoomIn.call($('zoom_in'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::ZoomOut(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_ZoomOut.call($('zoom_out'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::CleanStructure(void)
{
    QWebEnginePage* p_mf = WebView->page();
    p_mf->runJavaScript("ui.onClick_CleanUp.call($('clean_up'));");
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::ConvertTo3D(void)
{
    // create build project
    CExtUUID mp_uuid(NULL);
    mp_uuid.LoadFromString("{BUILD_PROJECT:b64d16f0-b73f-4747-9a13-212ab9a15d38}");
    CProject* p_project = Projects->NewProject(mp_uuid);
    if( p_project == NULL ){
        ES_ERROR("unable to create new build project");
        QMessageBox::critical(NULL, tr("Build Structure"),
                              tr("An error occurred during project opening!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }
    p_project->ShowProject();
    QApplication::processEvents();

    // synchronize data
    JSObject->updateSMILESData();

    // get actual data
    QString smiles = JSObject->getSMILESData();

    // set structure from smiles
    CStructure* p_mol = p_project->GetActiveStructure();
    if( p_mol == NULL ) return;
    p_mol->InsertSMILES(smiles);

    // autofit scene
    p_project->GetGraphics()->GetPrimaryView()->FitScene(false);
}

//------------------------------------------------------------------------------

void CSketchProjectWindow::CopyAsSVG(void)
{
    // update data - blocking
    JSObject->updateSVGData();

    // get data
    QString svg = JSObject->getSVGData();

    QByteArray byte_array;

    // prepare all data
    byte_array.append("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
    byte_array.append(svg.toUtf8()); // it is essential to keep format in utf-8, since file contains Raphae..l

    // prepare data fro clipboard
    QMimeData* p_mime_data = new QMimeData;
    p_mime_data->setData("image/svg+xml",byte_array);
    p_mime_data->setText(svg);

    QApplication::clipboard()->setMimeData(p_mime_data);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




