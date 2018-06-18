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

#include <WorkPanel.hpp>
#include "ui_CPMDInputExportTool.h"

//------------------------------------------------------------------------------

class CStructure;

//------------------------------------------------------------------------------

enum ECalculationType{
    WF_OPT,
    GEO_OPT,
    CP_DYN
};

//------------------------------------------------------------------------------

class CCPMDInputExportTool : public CWorkPanel {
Q_OBJECT
public:
    CCPMDInputExportTool(CProject* p_project);
    ~CCPMDInputExportTool(void);

//------------------------------------------------------------------------------
private:
    Ui::CPMDInputExportTool WidgetUI;
    CStructure*             Structure;

    // Internal data structure for the calculation
    ECalculationType    CalculationType;

    // generate input file
    QString GenerateInputDeck();
    QString GetLMax(int z);

    virtual void  LoadWorkPanelSpecificData(CXMLElement* p_ele);
    virtual void  SaveWorkPanelSpecificData(CXMLElement* p_ele);

    void GenerateHeader(QTextStream& str);
    void GenerateCPMD(QTextStream& str);
    void GenerateSystem(QTextStream& str);
    void GenerateDFT(QTextStream& str);
    void GenerateAtoms(QTextStream& str);

private Q_SLOTS:
    void ResetSetup(void);
    void SaveInputFile(void);
    void SaveMapFile(void);
    void PreviewEdited(void);
    void UpdatePreviewText(void);
};

//------------------------------------------------------------------------------

#endif
