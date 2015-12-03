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

#include "RegExpDialog.moc"
#include <RegExpDialog.hpp>
#include <NemesisCoreModule.hpp>
#include <QDialog>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        RegExpDialogID(
                    "{REGEXP_WORK_PANEL:6011f905-94cf-415d-a819-d6615d700286}",
                    "RegExp Dialog");

CPluginObject   RegExpDialogObject(&NemesisCorePlugin,
                    RegExpDialogID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRegExpDialog::CRegExpDialog(const QString& title,CProject* p_project)
    : CWorkPanel(&RegExpDialogObject,p_project,EWPR_DIALOG)
{
    // set up ui file
    WidgetUI.setupUi(this);

    // populate combo box
    WidgetUI.typeCB->addItem("wildcard");
    WidgetUI.typeCB->addItem("qt regexp");

    // local events
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CRegExpDialog::~CRegExpDialog(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const QRegExp& CRegExpDialog::GetRegExp(void)
{
    return(RegExp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRegExpDialog::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Ok ) {
        if( WidgetUI.caseSensitiveCB->isChecked() ){
            RegExp.setCaseSensitivity(Qt::CaseSensitive);
        } else {
            RegExp.setCaseSensitivity(Qt::CaseInsensitive);
        }
        switch(WidgetUI.typeCB->currentIndex()){
            case 0:
                RegExp.setPatternSyntax(QRegExp::Wildcard);
                break;
            case 1:
                RegExp.setPatternSyntax(QRegExp::RegExp);
                break;
        }

        RegExp.setPattern(WidgetUI.filterLE->text());
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



