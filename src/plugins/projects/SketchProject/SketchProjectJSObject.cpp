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
    ProjectWindow->UpdateEditMenu();
    ProjectWindow->UpdateStructureMenu();

    // was the data changed
    if( changed == "notchanged" ){
        ProjectWindow->Project->SetFlag(EPOF_PROJECT_CHANGED,false);
    } else {
        ProjectWindow->Project->SetFlag(EPOF_PROJECT_CHANGED,true);
    }
}

//------------------------------------------------------------------------------

const QString CSketchProjectJSObject::getInitialStructure(void)
{
    QString initialmol = cleanProject(ProjectWindow->Project->LoadedSmiles);
    ProjectWindow->Project->LoadedSmiles = "";
    return(initialmol);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










