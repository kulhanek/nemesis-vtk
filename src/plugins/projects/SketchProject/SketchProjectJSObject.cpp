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

#include <QtGui>

#include "SketchProjectWindow.hpp"
#include "SketchProjectJSObject.hpp"
#include <indigo.h>
#include <iostream>
#include <QSvgRenderer>
#include "SketchProject.hpp"

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSketchProjectJSObject::CSketchProjectJSObject(CSketchProjectWindow* p_owner)
    : QObject(p_owner)
{   
    ProjectWindow = p_owner;
    DataVersion = 0;
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::getInitialStructure(void)
{
    QString initialmol = cleanProject(ProjectWindow->Project->LoadedSmiles);
    ProjectWindow->Project->LoadedSmiles = "";
    SMILESData = initialmol;
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::updateSMILESData(void)
{
    int version = DataVersion;
    emit onUpdateSMILESData();
    while( version == DataVersion ){
        QApplication::processEvents();
    }
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::updateSVGData(void)
{
    int version = DataVersion;
    emit onUpdateSVGData();
    while( version == DataVersion ){
        QApplication::processEvents();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSketchProjectJSObject::newProject(void)
{
    ProjectWindow->NewProject();
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::openProject(void)
{
    ProjectWindow->OpenProject();
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::saveProject(void)
{
    ProjectWindow->SaveProject();
}

//------------------------------------------------------------------------------

const QString CSketchProjectJSObject::cleanProject(const QString& inmol)
{
    // load mol
    int molid = indigoLoadMoleculeFromString(inmol.toStdString().c_str());

    // layout mol
    indigoLayout(molid);

    // save mol
    const char* outstring = indigoMolfile(molid);
    QString outmol(outstring);

    // free molecule
    indigoFree(molid);

    return(outmol);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::updateMenu(const QString& changed)
{
    emit onMenuUpdate();

    // was the data changed
    if( changed == "notchanged" ){
        ProjectWindow->Project->SetFlag(EPOF_PROJECT_CHANGED,false);
    } else {
        ProjectWindow->Project->SetFlag(EPOF_PROJECT_CHANGED,true);
    }
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setSMILESData(const QString& data)
{
    SMILESData = data;
    DataVersion++;
}

//------------------------------------------------------------------------------

const QString& CSketchProjectJSObject::getSMILESData(void)
{
    return(SMILESData);
}

//------------------------------------------------------------------------------

void  CSketchProjectJSObject::setSVGData(const QString& data)
{
    SVGData = data;
    DataVersion++;
}

//------------------------------------------------------------------------------

const QString& CSketchProjectJSObject::getSVGData(void)
{
    return(SVGData);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setUndoEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionUndo->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setRedoEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionRedo->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setCutEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionCutStructure->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setCopyEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionCopyStructure->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setPasteEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionPasteStructure->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setZoomInEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionZoomIn->setEnabled(set);
}

//------------------------------------------------------------------------------

void CSketchProjectJSObject::setZoomOutEnabled(bool set)
{
    ProjectWindow->WidgetUI.actionZoomOut->setEnabled(set);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










