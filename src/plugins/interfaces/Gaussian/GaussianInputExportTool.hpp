#ifndef GAUSSIANINPUTDIALOG_H
#define GAUSSIANINPUTDIALOG_H
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
  GaussianInputDialog - Dialog for generating Gaussian input decks

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
#include "ui_GaussianInputExportTool.h"
#include <Atom.hpp>

//------------------------------------------------------------------------------

class CStructure;

//------------------------------------------------------------------------------

class CGaussianInputExportTool : public CWorkPanel {
Q_OBJECT
public:
    CGaussianInputExportTool(CProject* p_project);
    ~CGaussianInputExportTool(void);

//------------------------------------------------------------------------------
private:
    Ui::GaussianInputExportTool WidgetUI;
    CStructure*                 Structure;
    QMap<CAtom*,int>            FragIndexes;

    // Generate an input deck as a string
    QString GenerateInputDeck(void);

    // Translate enums to strings
    QString GetCalculationType(void);
    QString GetTheoryType(bool spinprefix);
    QString GetTheoryExtras(void);
    QString GetBasisType(void);
    bool IsBasisRequired(void);

    void InitFragmentIndexes(void);

    void GenerateHeader(QTextStream& str);
    void GenerateCoordinates(QTextStream& str);
    void GeneratePBCCoordinates(QTextStream& str);

    virtual void  LoadWorkPanelSpecificData(CXMLElement* p_ele);
    virtual void  SaveWorkPanelSpecificData(CXMLElement* p_ele);

private slots:
    void SetMethod(void);
    void SetTheory(void);
    void UpdatePreviewText(void);
    void ResetSetup(void);
    void SaveInputFile(void);
    void PreviewEdited(void);
    void CoordinateChanged(void);
    void CoordinateSetupChanged(void);
};

//------------------------------------------------------------------------------

#endif
