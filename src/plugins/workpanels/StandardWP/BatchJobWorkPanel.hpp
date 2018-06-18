#ifndef BatchJobWorkPanelH
#define BatchJobWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2014 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_BatchJobWorkPanel.h"

// -----------------------------------------------------------------------------

class CContainerModel;
class CBatchJob;
class CXMLElement;

// -----------------------------------------------------------------------------

/// list batch jobs

class CBatchJobWorkPanel : public CWorkPanel {
Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CBatchJobWorkPanel(void);
    ~CBatchJobWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::BatchJobWorkPanel   WidgetUI;
    CContainerModel*        BatchJobModel;
    CBatchJob*              BatchJob;

private slots:
    void ModelReset(void);
    void OpenBatchJob(void);
    void InspectBatchJob(void);
    void ReleaseBatchJob(void);
};

// -----------------------------------------------------------------------------

#endif
