#ifndef ErrorConsoleWorkPanelH
#define ErrorConsoleWorkPanelH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QWidget>
#include <WorkPanel.hpp>
#include <QStandardItemModel>
#include <EventManager.hpp>
#include "ui_ErrorConsoleWorkPanel.h"

// -----------------------------------------------------------------------------

class CErrorConsoleWorkPanel : public CWorkPanel, public CEventManager {

    Q_OBJECT

public:
// constructor and destructors ------------------------------------------------
    CErrorConsoleWorkPanel(void);
    ~CErrorConsoleWorkPanel(void);

// section of private data ----------------------------------------------------
private slots:
    void RefreshList(void);
    void ClearErrors(void);
    void AutoUpdate(int);

private:
    QStandardItemModel* Model;
    Ui::ErrorConsoleWorkPanel WidgetUI;

    virtual bool ProcessEvent(const CEventIdentifier& event,
                                           CEventManager* Sender,void* p_data);
    virtual void LoadWorkPanelSpecificData(CXMLElement* p_ele);
    virtual void SaveWorkPanelSpecificData(CXMLElement* p_ele);
};

// -----------------------------------------------------------------------------

#endif
