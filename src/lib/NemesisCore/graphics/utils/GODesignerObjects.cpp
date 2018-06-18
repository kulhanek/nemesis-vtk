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

#include <ErrorSystem.hpp>
#include <ContainerModel.hpp>
#include <GraphicsObject.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <ProObjectDesigner.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <CategoryUUID.hpp>
#include <SelectionList.hpp>

#include <GODesignerObjects.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGODesignerObjects::CGODesignerObjects(QWidget* p_owner,CGraphicsObject* p_object,
                                       CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
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

    // init model --------------------------------
    ObjectsModel = Object->GetContainerModel(this);
    WidgetUI.objectsTV->setModel(ObjectsModel);

    if( ObjectsModel != NULL ){
        for(int i=0; i < ObjectsModel->columnCount(); i++){
            WidgetUI.objectsTV->resizeColumnToContents(i);
        }
        // connect signals ---------------------------
        connect(ObjectsModel,SIGNAL(modelReset()),
                this,SLOT(ObjectTVReset()));
    }

    if(   Object->IsAllowedObjectType(InvalidObjectID)
       || Object->IsAllowedObjectType(PrivateObjectID)
       || (ObjectsModel == NULL) ){
        WidgetUI.toolsW->hide();
    } else {
        // update list of allowed objects
        std::vector<CExtUUID>::iterator it = Object->BeginAllowedObjectTypes();
        std::vector<CExtUUID>::iterator ie = Object->EndAllowedObjectTypes();

        while( it != ie ){
            WidgetUI.allowedTypesCB->addItem( (*it).GetName() );
            it++;
        }

        // can we add structure?
        if( ! Object->IsAllowedObjectType(StructureID) ){
            WidgetUI.addStructurePB->hide();
        }
        if( ! Object->IsAllowedObjectType(StructureListID) ){
            WidgetUI.addAllStructuresPB->hide();
        }
    }

    // update list status
    ObjectTVReset();

    //------------------
    connect(WidgetUI.objectsTV,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(ObjectTVClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.objectsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectTVDblClicked(const QModelIndex&)));
    //------------------
    connect(WidgetUI.addStructurePB,SIGNAL(clicked(bool)),
            this,SLOT(AddStructureObject(void)));
    //------------------
    connect(WidgetUI.addAllStructuresPB,SIGNAL(clicked(bool)),
            this,SLOT(AddAllStructures(void)));
    //------------------
    connect(WidgetUI.addSelectedObjectsPB,SIGNAL(clicked(bool)),
            this,SLOT(AddSelectedObjects(void)));
    //------------------
    connect(WidgetUI.removeSelectedObjectsPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveSelectedObjects(void)));
    //------------------
    connect(WidgetUI.removeObjectPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveObject(void)));
    //------------------
    connect(WidgetUI.removeAllObjectsPB,SIGNAL(clicked(bool)),
            this,SLOT(RemoveAllObjects(void)));
    //------------------
    connect(Object->GetProject()->GetSelection(),SIGNAL(OnSelectionChanged(void)),
            this,SLOT(SelectionChanged(void)));
    //------------------
    connect(Object->GetProject()->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
            this,SLOT(ProjectLockChanged(EHistoryChangeMessage)));

    // set initial values ------------------------
    ProjectLockChanged(EHCM_LOCK_LEVEL);
    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly

    // update selections
    SelectionChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGODesignerObjects::InitValues(void)
{
    // nothing to do here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGODesignerObjects::ObjectTVReset(void)
{
    WidgetUI.removeObjectPB->setEnabled(false);

    int num = 0;
    if( ObjectsModel != NULL ) {
        num = ObjectsModel->rowCount();
    }
    QString text("%1");
    text = text.arg(num);
    WidgetUI.numOfObjectsLE->setText(text);
}

//------------------------------------------------------------------------------

void CGODesignerObjects::ObjectTVClicked(const QModelIndex& index)
{
    WidgetUI.removeObjectPB->setEnabled(true);
}

//------------------------------------------------------------------------------

void CGODesignerObjects::ObjectTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CGODesignerObjects::AddStructureObject(void)
{
    if( Object->GetProject()->GetActiveStructure() ){
        Object->AddObjectWH(Object->GetProject()->GetActiveStructure());
    }
}

//------------------------------------------------------------------------------

void CGODesignerObjects::AddAllStructures(void)
{
    if( Object->GetProject()->GetStructures() ){
        Object->AddObjectWH(Object->GetProject()->GetStructures());
    }
}

//------------------------------------------------------------------------------

void CGODesignerObjects::AddSelectedObjects(void)
{
    Object->AddSelectedObjectsWH();
}

//------------------------------------------------------------------------------

void CGODesignerObjects::RemoveSelectedObjects(void)
{
    Object->RemoveSelectedObjectsWH();
}

//------------------------------------------------------------------------------

void CGODesignerObjects::RemoveObject(void)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(WidgetUI.objectsTV->currentIndex()));
    if( p_obj == NULL ) return;
    Object->RemoveObjectWH(p_obj);
}

//------------------------------------------------------------------------------

void CGODesignerObjects::RemoveAllObjects(void)
{
    Object->RemoveAllObjectsWH();
}

//------------------------------------------------------------------------------

void CGODesignerObjects::SelectionChanged(void)
{
    bool set = false;

    std::vector<CExtUUID>::iterator it = Object->BeginAllowedObjectTypes();
    std::vector<CExtUUID>::iterator ie = Object->EndAllowedObjectTypes();

    while( it != ie ){
        set |= Object->GetProject()->GetSelection()->NumOfSelectedObjects(*it);
        it++;
    }

    WidgetUI.addSelectedObjectsPB->setEnabled(set);
    WidgetUI.removeSelectedObjectsPB->setEnabled(set);
}

//------------------------------------------------------------------------------

void CGODesignerObjects::ProjectLockChanged(EHistoryChangeMessage message)
{
    if( message != EHCM_LOCK_LEVEL ) return;
    parentWidget()->setEnabled(! Object->IsHistoryLocked(EHCL_GRAPHICS));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

