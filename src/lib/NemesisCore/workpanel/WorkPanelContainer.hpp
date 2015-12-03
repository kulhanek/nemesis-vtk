#ifndef WorkPanelContainerH
#define WorkPanelContainerH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>

//------------------------------------------------------------------------------

enum EWorkPanelType {
    EWPT_SINGLE_WINDOW, // single window
    EWPT_DOCK_WINDOW,   // dock window
    EWPT_DIALOG         // dialog window
};

//------------------------------------------------------------------------------

class CWorkPanelDockWidget;
class CWorkPanelDialogWidget;

//------------------------------------------------------------------------------

class CWorkPanelContainer {
public:
    CWorkPanelContainer(void);

    EWorkPanelType          Type;

    union {
        CWorkPanelDockWidget*   DockWidget;
        CWorkPanelDialogWidget* Dialog;
    } Widget;
};

//------------------------------------------------------------------------------

#endif
