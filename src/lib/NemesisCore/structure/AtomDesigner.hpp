#ifndef AtomDesignerH
#define AtomDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ProObjectDesigner.hpp>
#include <Atom.hpp>
#include "ui_AtomDesigner.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CPODesignerGeneral;
class CPODesignerFlags;
class CPODesignerRefBy;

//------------------------------------------------------------------------------

/// Object designer

class CAtomDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CAtomDesigner(CAtom* p_atom);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    Ui::AtomDesigner    WidgetUI;
    CAtom*              Object;
    CPODesignerGeneral* General;
    CPODesignerFlags*   Flags;
    CPODesignerRefBy*   RefBy;
    int                 Z;
    CContainerModel*    Bonds;
    CContainerModel*    RegObjects;

private slots:
    void ObjectDestroyed(void);

    void InitValues(void);
    void ApplyValues(void);

    void OpenPeriodicTableDialog(void);
    void ButtonBoxClicked(QAbstractButton *);
    void ObjectTVDblClicked(const QModelIndex& index);
    void UpdateStatistics(void);
    void GeometryChanged(void);
    void ResidueAboutToBeChanged(CProObject* p_obj);
    void ProjectLockChanged(EHistoryChangeMessage message);
};

//------------------------------------------------------------------------------

#endif
