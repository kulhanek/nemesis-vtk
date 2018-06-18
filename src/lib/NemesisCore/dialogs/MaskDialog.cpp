// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>

#include <SelectionList.hpp>

#include <MaskDialog.hpp>
#include <NemesisCoreModule.hpp>
#include <QDialog>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        MaskDialogID(
                    "{MASK_WORK_PANEL:3c952eb1-b7c1-478d-b2f5-9845b43fd45c}",
                    "Mask Dialog");

CPluginObject   MaskDialogObject(&NemesisCorePlugin,
                    MaskDialogID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMaskDialog::CMaskDialog(const QString& title,CProject* p_project)
    : CWorkPanel(&MaskDialogObject,p_project,EWPR_DIALOG)
{
    // set up ui file
    WidgetUI.setupUi(this);

    // populate combo box
    WidgetUI.typeCB->addItem("ASL");

    // local events
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CMaskDialog::~CMaskDialog(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CMaskDialog::GetMask(void)
{
    return(WidgetUI.maskLE->text());
}

//------------------------------------------------------------------------------

EAtomMaskType CMaskDialog::GetMaskType(void)
{
    switch(WidgetUI.typeCB->currentIndex()){
        case 0:
        default:
            return(EAMT_ASL);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMaskDialog::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Ok ) {
        SetDialogResult(QDialog::Accepted);
        close();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Cancel ) {
        SetDialogResult(QDialog::Rejected);
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



