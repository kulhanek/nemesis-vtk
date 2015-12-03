// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QtGui>

#include "TrajectoryProjectModule.hpp"
#include "TrajectoryProjectDesigner.hpp"
#include "TrajectoryProjectDesigner.moc"

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>

#include "TrajectoryProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* TrajectoryProjectDesignerCB(void* p_data);

CExtUUID        TrajectoryProjectDesignerID(
                    "{TRAJECTORY_PROJECT_DESIGNER:7f390c8f-b680-4411-be8d-ec4106c888cd}",
                    "Trajectory");

CPluginObject   TrajectoryProjectDesignerObject(&TrajectoryProjectPlugin,
                    TrajectoryProjectDesignerID,
                    DESIGNER_CAT,
                    ":/images/TrajectoryProject/TrajectoryProject.svg",
                    TrajectoryProjectDesignerCB);

// -----------------------------------------------------------------------------

QObject* TrajectoryProjectDesignerCB(void* p_data)
{
    CTrajectoryProject* p_pro_object = static_cast<CTrajectoryProject*>(p_data);
    QObject* p_object = new CTrajectoryProjectDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajectoryProjectDesigner::CTrajectoryProjectDesigner(CTrajectoryProject* p_owner)
    : CProObjectDesigner(&TrajectoryProjectDesignerObject,p_owner)
{
    Project = p_owner;

    // visual setup
    WidgetUI.setupUi(this);

    // initial values
    InitValues();

    // signals
    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            SLOT(ButtonBoxClicked(QAbstractButton*)));

    connect(WidgetUI.descriptionTE, SIGNAL(textChanged()),
            SLOT(SetChangedFlagTrue()));

    // visual setup - finalization
    show();
}

//------------------------------------------------------------------------------

CTrajectoryProjectDesigner::~CTrajectoryProjectDesigner()
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryProjectDesigner::InitValues(void)
{
    WidgetUI.nameLE->setText(QString(Project->GetName()));
    WidgetUI.descriptionTE->setText(QString(Project->GetDescription()));
    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CTrajectoryProjectDesigner::SetValues(void)
{
    if( IsChangedFlagSet() == false ) return;
    Project->SetDescriptionWH(WidgetUI.descriptionTE->toPlainText());
    WidgetUI.descriptionTE->setText(QString(Project->GetDescription()));
    SetChangedFlag(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajectoryProjectDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset ) {
        InitValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Apply ) {
        SetValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================










