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

#include "TextDialog.moc"
#include <TextDialog.hpp>
#include <NemesisCoreModule.hpp>

#include <QDialog>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        TextDialogID(
                    "{TEXT_WORK_PANEL:d814835f-f7d6-4363-a106-a8ed4a0dee4e}",
                    "Text Dialog");

CPluginObject   TextDialogObject(&NemesisCorePlugin,
                    TextDialogID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTextDialog::CTextDialog(const QString& title,CProject* p_project)
    : CWorkPanel(&TextDialogObject,p_project,EWPR_DIALOG)
{
    // set up ui file
    WidgetUI.setupUi(this);

    // local events
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CTextDialog::~CTextDialog(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QString CTextDialog::GetText(void)
{
    return(WidgetUI.textEdit->toPlainText());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTextDialog::ButtonBoxClicked(QAbstractButton *button)
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



