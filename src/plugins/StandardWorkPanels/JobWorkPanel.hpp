#ifndef JobWorkPanelH
#define JobWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_JobWorkPanel.h"

// -----------------------------------------------------------------------------

class CProject;
class CContainerModel;
class CJob;
class CXMLElement;

// -----------------------------------------------------------------------------

class CJobWorkPanel : public CWorkPanel {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CJobWorkPanel(CProject* p_project);
    ~CJobWorkPanel();

// section of private data ----------------------------------------------------
private:
    Ui::JobWorkPanel    WidgetUI;
    CProject*           Project;
    CContainerModel*    JobModel;
    CContainerModel*    SchedulerModel;
    CJob*               Job;

private slots:
    void OpenProjectDesigner(void);
    void ProjectNameChanged(CProject* p_project);
    void TreeViewClicked(const QModelIndex& index);
    void ModelReset(void);
    void TerminateJob(void);
};

// -----------------------------------------------------------------------------

#endif
