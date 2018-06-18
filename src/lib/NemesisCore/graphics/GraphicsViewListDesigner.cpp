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

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ExtUUID.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsViewList.hpp>
#include <MainWindow.hpp>

#include <PODesignerGeneral.hpp>

#include <GraphicsViewListDesigner.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* GraphicsViewListDesignerCB(void* p_data);

CExtUUID        GraphicsViewListDesignerID(
                    "{GRAPHICS_VIEW_LIST_DESIGNER:eeb53172-b286-4f95-82f3-6b118a491d82}",
                    "Views");

CPluginObject   GraphicsViewListDesignerObject(&NemesisCorePlugin,
                    GraphicsViewListDesignerID,WORK_PANEL_CAT,
                    ":/images/NemesisCore/graphics/ViewList.svg",
                    GraphicsViewListDesignerCB);

// -----------------------------------------------------------------------------

QObject* GraphicsViewListDesignerCB(void* p_data)
{
    CGraphicsViewList* p_gp = static_cast<CGraphicsViewList*>(p_data);
    QObject* p_object = new CGraphicsViewListDesigner(p_gp);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphicsViewListDesigner::CGraphicsViewListDesigner(CGraphicsViewList* p_gp)
    : CProObjectDesigner(&GraphicsViewListDesignerObject,p_gp)
{
    Object = p_gp;
    WidgetUI.setupUi(this);

    // attached common setup part ----------------
    General = new CPODesignerGeneral(WidgetUI.generalGB,Object,this);

    // profile objects ---------------------------
    ViewModel = p_gp->GetContainerModel(this);
    WidgetUI.objectTV->setModel(ViewModel);

    if( ViewModel != NULL ){
        connect(ViewModel,SIGNAL(modelReset(void)),
                this,SLOT(ViewTVReset(void)));
        for(int i=0; i < ViewModel->columnCount(); i++){
            WidgetUI.objectTV->resizeColumnToContents(i);
        }
    }

    // connect slots -----------------------------
    connect(WidgetUI.objectTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(ViewTVClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.objectTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ViewTVDblClicked(const QModelIndex&)));
    //--------------
    connect(WidgetUI.newViewPB,SIGNAL(clicked(bool)),
            this,SLOT(NewView(void)));
    //--------------
    connect(WidgetUI.deleteViewPB,SIGNAL(clicked(bool)),
            this,SLOT(DeleteView(void)));
    //--------------
    connect(WidgetUI.viewInfoPB,SIGNAL(clicked(bool)),
            this,SLOT(ViewInfo(void)));

    //------------------
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // init all values ---------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    InitAllValues();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewListDesigner::InitAllValues(void)
{
    if( IsItChangingContent() == true ) return;

    General->InitValues();

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::ApplyAllValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    if( Object->BeginChangeWH(EHCL_COMPOSITE,Object->GetType().GetName()) == false ) return;

    Changing = true;
        General->ApplyValues();
    Changing = false;

    Object->EndChangeWH(); // this also repaint the project

    // some changes can be prohibited - reinit visualization
    InitAllValues();

    // do not repaint project here, it is done in EndChangeWH
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Reset ) {
        InitAllValues();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Apply  ) {
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

void CGraphicsViewListDesigner::ViewTVClicked(const QModelIndex& index)
{
    CGraphicsView* p_obj = NULL;
    p_obj = dynamic_cast<CGraphicsView*>(ViewModel->GetItem(index));
    bool obj_enabled = p_obj != NULL;

    WidgetUI.deleteViewPB->setEnabled(obj_enabled);
    WidgetUI.viewInfoPB->setEnabled(obj_enabled);
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::ViewTVDblClicked(const QModelIndex& index)
{
    CGraphicsView* p_go = NULL;
    p_go = dynamic_cast<CGraphicsView*>(ViewModel->GetItem(index));
    if( p_go == NULL ) return;
    p_go->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::ViewTVReset(void)
{
    WidgetUI.deleteViewPB->setEnabled(false);
    WidgetUI.viewInfoPB->setEnabled(false);
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::NewView(void)
{
    Object->CreateView();
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::DeleteView(void)
{
    const QModelIndex& index = WidgetUI.objectTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsView* p_go = NULL;
    p_go = dynamic_cast<CGraphicsView*>(ViewModel->GetItem(index));
    if( p_go == NULL ) return;

    p_go->Delete();
}

//------------------------------------------------------------------------------

void CGraphicsViewListDesigner::ViewInfo(void)
{
    const QModelIndex& index = WidgetUI.objectTV->currentIndex();
    if( index.isValid() == false ) return;

    CGraphicsView* p_go = NULL;
    p_go = dynamic_cast<CGraphicsView*>(ViewModel->GetItem(index));
    if( p_go == NULL ) return;
    p_go->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphicsViewListDesigner::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    WidgetUI.controlW->setEnabled(! Object->GetProject()->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
