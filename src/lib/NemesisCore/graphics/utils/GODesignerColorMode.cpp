// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ErrorSystem.hpp>
#include <NemesisCoreModule.hpp>
#include <GOColorMode.hpp>
#include <ProObjectDesigner.hpp>
#include <Project.hpp>

#include <GODesignerColorMode.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGODesignerColorMode::CGODesignerColorMode(QWidget* p_owner,CGOColorMode* p_object,
                                   CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    ChangingContents = false;
    Object = p_object;
    if( Object == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Designer = p_designer;

    // set layout --------------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // -----------------------
    WidgetUI.modesCB->clear();
    for(int i = 0; i < Object->GetNumberOfColorModes(); i++){
        WidgetUI.modesCB->addItem(Object->GetColorModeName(i));
    }

    // connect signals ---------------------------
    connect(WidgetUI.modesCB,SIGNAL(currentIndexChanged(int)),
            Designer,SLOT(SetChangedFlagTrue()));
    // -----------------------
    connect(WidgetUI.setupTB,SIGNAL(clicked(bool)),
            Object,SLOT(OpenObjectDesigner()));
    // -----------------------
    connect(Object,SIGNAL(OnStatusChanged(EStatusChanged)),
            this,SLOT(InitValues()));
    // -----------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGODesignerColorMode::InitValues(void)
{
    if( Designer->IsItChangingContent() ) return;
    if( ChangingContents ) return;

    ChangingContents = true;
        WidgetUI.modesCB->setCurrentIndex(Object->GetColorMode());
    ChangingContents = false;
}

//------------------------------------------------------------------------------

void CGODesignerColorMode::ApplyValues(void)
{
    ChangingContents = true;
        Object->SetColorModeWH(WidgetUI.modesCB->currentIndex());
    ChangingContents = false;
}

//------------------------------------------------------------------------------

void CGODesignerColorMode::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


