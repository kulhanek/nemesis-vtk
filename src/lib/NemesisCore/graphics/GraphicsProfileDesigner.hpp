#ifndef GraphicsProfileDesignerH
#define GraphicsProfileDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ProObjectDesigner.hpp>
#include "ui_GraphicsProfileDesigner.h"

//------------------------------------------------------------------------------

class CPODesignerGeneral;
class CGraphicsProfile;

//------------------------------------------------------------------------------

class CGraphicsProfileDesigner : public CProObjectDesigner {
    Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CGraphicsProfileDesigner(CGraphicsProfile* p_gp);
    ~CGraphicsProfileDesigner(void);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::GraphicsProfileDesigner     WidgetUI;
    CGraphicsProfile*               Object;
    CPODesignerGeneral*             General;
    CContainerModel*                ProfileModel;

private slots:
    void InitValues(void);
    void ButtonBoxClicked(QAbstractButton*);

    void ProfileTVSelectionChanged(void);
    void ProfileTVDblClicked(const QModelIndex& index);

    void ProfileNewObject(void);
    void ProfileObjectUp(void);
    void ProfileObjectDown(void);
    void ProfileObjectShowHide(void);
    void ProfileObjectRemove(void);
    void ProfileObjectDelete(void);
    void ProfileObjectInfo(void);
    void ProfileActivate(void);

    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
