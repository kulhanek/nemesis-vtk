#ifndef PeriodicTableWidgetH
#define PeriodicTableWidgetH
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

#include <NemesisCoreMainHeader.hpp>
#include <ElementColors.hpp>
#include <WorkPanel.hpp>
#include <QLabel>
#include <QButtonGroup>

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CPeriodicTableWidget : public QWidget {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CPeriodicTableWidget(QWidget* p_parent);
    ~CPeriodicTableWidget(void);

// informative methods --------------------------------------------------------
    /// get active element
    int  GetActiveElement(void);

    /// update element colors
    void UpdateColors(void);

public slots:
    /// change element
    void ChangeElement(int z);

// public signals -------------------------------------------------------------
signals:
    void OnElementChanged(int z);

// section of private data ----------------------------------------------------
private:
    int             ActiveElement;
    QLabel*         Label;
    QButtonGroup*   ButtonGroup;

    void GeneratePeriodicTableLayout(void);
    void GetPeriodAndGroup(int z, int &period, int &group);

private slots:
    void ChangeElementWithSignal(int z);
};

// -----------------------------------------------------------------------------

#endif
