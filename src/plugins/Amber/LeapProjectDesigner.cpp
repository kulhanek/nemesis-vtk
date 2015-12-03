// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QtGui>

#include "AmberModule.hpp"
#include "LeapProjectDesigner.hpp"
#include "LeapProjectDesigner.moc"

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>

#include "LeapProject.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* LeapProjectDesignerCB(void* p_data);

CExtUUID        LeapProjectDesignerID(
                    "{LEAP_PROJECT_DESIGNER:b065e3a9-ed4a-4564-9181-3393c7cc8661}",
                    "nLEaP");

CPluginObject   LeapProjectDesignerObject(&AmberPlugin,
                    LeapProjectDesignerID,
                    DESIGNER_CAT,
                    ":/images/AmberPlugin/nLEaPProject.svg",
                    LeapProjectDesignerCB);

// -----------------------------------------------------------------------------

QObject* LeapProjectDesignerCB(void* p_data)
{
    CLeapProject* p_pro_object = static_cast<CLeapProject*>(p_data);
    QObject* p_object = new CLeapProjectDesigner(p_pro_object);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLeapProjectDesigner::CLeapProjectDesigner(CLeapProject* p_owner)
    : CProObjectDesigner(&LeapProjectDesignerObject,p_owner)
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

}

//------------------------------------------------------------------------------

CLeapProjectDesigner::~CLeapProjectDesigner()
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectDesigner::InitValues(void)
{
    WidgetUI.nameLE->setText(QString(Project->GetName()));
    WidgetUI.descriptionTE->setText(QString(Project->GetDescription()));
    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CLeapProjectDesigner::SetValues(void)
{
    if( IsChangedFlagSet() == false ) return;
    Project->SetDescriptionWH(WidgetUI.descriptionTE->toPlainText());
    WidgetUI.descriptionTE->setText(QString(Project->GetDescription()));
    SetChangedFlag(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CLeapProjectDesigner::ButtonBoxClicked(QAbstractButton* p_button)
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










