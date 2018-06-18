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
#include <PluginDatabase.hpp>

#include <GraphicsTypeDialog.hpp>
#include <NemesisCoreModule.hpp>
#include <QDialog>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsTypeDialogCB(void* p_data);

CExtUUID        GraphicsTypeDialogID(
                    "{GRAPHICS_TYPE_WORK_PANEL:bfb0f639-6be6-4719-a3f0-709a17563a70}",
                    "Graphics Type Dialog");

CPluginObject   GraphicsTypeDialogObject(&NemesisCorePlugin,
                    GraphicsTypeDialogID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsTypeDialog::CGraphicsTypeDialog(const QString& title,CProject* p_project)
    : CWorkPanel(&GraphicsTypeDialogObject,p_project,EWPR_DIALOG)
{
    // set up ui file
    WidgetUI.setupUi(this);

    CSimpleIteratorC<CPluginModule> I(PluginDatabase.GetModuleList());
    CPluginModule* p_module;

    // loop over modules
    while ((p_module = I.Current()) != NULL) {
        CSimpleIteratorC<CPluginObject> J(p_module->GetObjectList());
        CPluginObject* p_object;
        // loop over objects(
        while ((p_object = J.Current()) != NULL) {
            if (p_object->GetCategoryUUID() == GRAPHICS_OBJECT_CAT ) {
                WidgetUI.typeCB->addItem(p_object->GetName());
                Types.push_back(p_object->GetObjectUUID());
            }
            J++;
        }
        I++;
    }

    // local events
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CGraphicsTypeDialog::~CGraphicsTypeDialog(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CUUID& CGraphicsTypeDialog::GetType(void)
{
    static CUUID zero;
    if( WidgetUI.typeCB->currentIndex() < 0 ) return(zero);
    return(Types[WidgetUI.typeCB->currentIndex()]);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsTypeDialog::ButtonBoxClicked(QAbstractButton *button)
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



