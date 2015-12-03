#ifndef DockingProjectInputDIALOG_H
#define DockingProjectInputDIALOG_H
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
/**********************************************************************
  DockingProjectInputDialog - Dialog for generating Autodock Vina input decks

  Copyright (C) 2008-2009 Marcus D. Hanwell
  Copyright (C) 2008 Michael Banck

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.openmolecules.net/>

  Avogadro is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Avogadro is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
 **********************************************************************/

#include <WorkPanel.hpp>

#include <Atom.hpp>
#include "DockingProjectInputVinaExportTool.hpp"

#include "ui_DockingProjectInputExportTool.h"


//------------------------------------------------------------------------------

class CStructure;
class CBoxObject;

//------------------------------------------------------------------------------

extern CExtUUID DockingProjectInputExportToolID;

//------------------------------------------------------------------------------
class CDockingProjectInputExportTool : public CWorkPanel {
Q_OBJECT
public:
    CDockingProjectInputExportTool(CProject* p_project);
    ~CDockingProjectInputExportTool(void);

//------------------------------------------------------------------------------
private:
    Ui::DockingProjectInputExportTool WidgetUI;

    CDockingProject* DockingProject;

    CDockingProjectInputVinaExportTool* SetupVina;

    /// For structure generate text in PDBQT format
    QString GenerateStrucureText (CStructure* structure, bool is_ligand);

    /// Assign checkbox values from GUI into system
    void UpdateInternalValuesFromCheckboxes(void);
    /// Assign checkbox values from GUI into system
    void UpdateInternalValuesFromFields(void);

private slots:
    void UpdateGeneratedText(void);
    void ResetSetup(void);
    void SaveInputFiles(void);
    void PreviewEdited(void);
    void PreviewEditedLigand(void);
    void PreviewEditedReceptor(void);
    void CheckBoxesEdited(int value);
};

//------------------------------------------------------------------------------

#endif
