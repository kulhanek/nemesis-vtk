#ifndef GOptTrajSegmentDesignerH
#define GOptTrajSegmentDesignerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ui_GOptTrajSegmentDesigner.h"
#include <HistoryList.hpp>

//------------------------------------------------------------------------------

class CGOptTrajSegment;
class CPODesignerGeneral;
class CPODesignerStructures;

//------------------------------------------------------------------------------

/// xyz trajectory segment designer

class CGOptTrajSegmentDesigner : public CProObjectDesigner {
Q_OBJECT
public:
// constructor and destructor -------------------------------------------------
    CGOptTrajSegmentDesigner(CGOptTrajSegment* p_owner);

    /// initialize visualization of properties
    void InitAllValues(void);

    /// update object properties according to visual setup
    void ApplyAllValues(void);

// section of private data ----------------------------------------------------
private:
    CGOptTrajSegment*           Object;
    Ui::GOptTrajSegmentDesigner WidgetUI;
    CPODesignerGeneral*         General;

private slots:
    void ButtonBoxClicked(QAbstractButton* p_button);
    void InitValues(void);
    void ApplyValues(void);
    void ProjectLockChanged(EHistoryChangeMessage message);
    void InitPointerValues(void);
};

//------------------------------------------------------------------------------

#endif
