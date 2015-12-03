#ifndef PODesignerDockingTabsResultsH
#define PODesignerDockingTabsResultsH
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
#include <QObject>
#include <NemesisCoreMainHeader.hpp>
#include "ui_PODesignerDockingTabsResults.h"


// -----------------------------------------------------------------------------

class CTrajectory;
class CTrajectorySegment;
class CContainerModel;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPODesignerDockingTabsResults : public QWidget {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CPODesignerDockingTabsResults(QWidget* p_owner, CTrajectorySegment *p_object,
                               CWorkPanel* p_workpanel);
    ~CPODesignerDockingTabsResults();

// executive methods -----------------------------------------------------------
    /// initialize visualization of general properties
    void InitValues(void);

    /// update object general properties according to visual setup
    void ApplyValues(void);

// information metods ----------------------------------------------------
    /// get segment which is hold by widget
    CTrajectorySegment* GetTrajectorySegment(void) const;

// section of private data ----------------------------------------------------
private:
    Ui::PODesignerDockingTabsResults      WidgetUI;

    // TRAJECTORY OF Docking Segment ===============================================================
private:
    CWorkPanel*                     WorkPanel;
    CTrajectory*                    DockingTrajectory;
    CTrajectorySegment*             DockingSegment;
    CContainerModel*                PosesModel;

    long int OffsetOfSnapshots;

    bool SliderUpdating;
    bool SpinBoxUpdating;
    int  SliderMin;
    int  SliderMax;
    int  SpinBoxMin;
    int  SpinBoxMax;

private slots:    
    void UpdateResultsMenu(void);

    void SnapshotsPosesClicked(const QModelIndex& index);

    void SliderValueChanged(int pos);
    void SpinBoxValueChanged(int pos);

    void PrevSnapshot(void);
    void NextSnapshot(void);

    void SegmentDisconnect(QObject* qobject);

};

// -----------------------------------------------------------------------------

#endif
