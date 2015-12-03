// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreModule.hpp>
#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <GraphicsObject.hpp>

#include <GODesignerGeneral.hpp>
#include <GODesignerFlags.hpp>
#include <GODesignerObjects.hpp>
#include <GODesignerSetup.hpp>
#include <GODesignerRefBy.hpp>

#include <FreezedAtomsObjectDesigner.hpp>
#include "FreezedAtomsObjectDesigner.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FreezedAtomsObjectDesignerCB(void* p_data);

CExtUUID        FreezedAtomsObjectDesignerID(
                    "{FREEZED_ATOMS_OBJECT_DESIGNER:8d24850a-dc86-46ea-bf24-64be730d18b0}",
                    "Freezed atoms");

CPluginObject   FreezedAtomsObjectDesignerObject(&NemesisCorePlugin,
                    FreezedAtomsObjectDesignerID,DESIGNER_CAT,
                    FreezedAtomsObjectDesignerCB);

// -----------------------------------------------------------------------------

QObject* FreezedAtomsObjectDesignerCB(void* p_data)
{
    CGraphicsObject* p_bo = static_cast<CGraphicsObject*>(p_data);
    QObject* p_object = new CFreezedAtomsObjectDesigner(p_bo);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFreezedAtomsObjectDesigner::CFreezedAtomsObjectDesigner(CGraphicsObject* p_bo)
    : CProObjectDesigner(&FreezedAtomsObjectDesignerObject, p_bo)
{
    Object = p_bo;

    // graphics layout ---------------------------
    WidgetUI.setupUi(this);
    // it must be here becuase the title is set in WidgetUI.setupUi(this)
    setWindowTitle(GetType().GetName());

    // attached common setup part ----------------
    General = new CGODesignerGeneral(WidgetUI.generalGB,Object,this);
    Flags = new CGODesignerFlags(WidgetUI.flagsGB,Object,this);
    Objects = new CGODesignerObjects(WidgetUI.objectsTab,Object,this);
    Setup = new CGODesignerSetup(WidgetUI.setupTab,Object,this);
    RefBy = new CGODesignerRefBy(WidgetUI.refByTab,Object,this);

    // connect slots -----------------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            this,SLOT(ButtonBoxClicked(QAbstractButton *)));

    // init all values ---------------------------
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsObjectDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();
    Flags->InitValues();
    Objects->InitValues();
    Setup->InitValues();
    RefBy->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFreezedAtomsObjectDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_DESCRIPTION,"background change") == false ) return;

    Changing = true;
        General->ApplyValues();
        Flags->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CFreezedAtomsObjectDesigner::LoadObjectDesignerSetup(CXMLElement* p_ele)
{
    General->LoadDesignerSetup(p_ele);
    Flags->LoadDesignerSetup(p_ele);
    Objects->LoadDesignerSetup(p_ele);
    Setup->LoadDesignerSetup(p_ele);
    RefBy->LoadDesignerSetup(p_ele);
}

//------------------------------------------------------------------------------

void CFreezedAtomsObjectDesigner::SaveObjectDesignerSetup(CXMLElement* p_ele)
{
    General->SaveDesignerSetup(p_ele);
    Flags->SaveDesignerSetup(p_ele);
    Objects->SaveDesignerSetup(p_ele);
    Setup->SaveDesignerSetup(p_ele);
    RefBy->SaveDesignerSetup(p_ele);
}

//------------------------------------------------------------------------------

void CFreezedAtomsObjectDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
        ApplyAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Close ) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


