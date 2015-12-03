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
#include <XMLElement.hpp>

#include <GSDesignerRefBy.hpp>
#include "GSDesignerRefBy.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGSDesignerRefBy::CGSDesignerRefBy(QWidget* p_owner,CGraphicsSetup* p_setup,
                                   CProObjectDesigner* p_designer)
    : QWidget(p_owner)
{
    Setup = p_setup;
    if( Setup == NULL ){
        INVALID_ARGUMENT("p_object is NULL");
    }
    Designer = p_designer;

    // insert to layout --------------------------
    if( p_owner->layout() ) delete p_owner->layout();
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0,0,0,0);
    p_layout->addWidget(this);
    p_owner->setLayout(p_layout);

    // load graphics -----------------------------
    WidgetUI.setupUi(this);

    // init model --------------------------------
    ObjectsModel = Setup->GetRegObjectsContainerModel(this);
    WidgetUI.objectsTV->setModel(ObjectsModel);
    if( ObjectsModel != NULL ){
        for(int i=0; i < ObjectsModel->columnCount(); i++){
            WidgetUI.objectsTV->resizeColumnToContents(i);
        }
    }

    // connect signals ---------------------------
    connect(WidgetUI.objectsTV,SIGNAL(doubleClicked(const QModelIndex&)),
            this,SLOT(ObjectTVDblClicked(const QModelIndex&)));

    // do not init values here, the owner of this componet is
    // responsible to call InitValues explicitly
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGSDesignerRefBy::InitValues(void)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CGSDesignerRefBy::ApplyValues(void)
{
    // nothing to do here
}

//------------------------------------------------------------------------------

void CGSDesignerRefBy::LoadDesignerSetup(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    QByteArray view_state;
    p_ele->GetAttribute("refby_header_state",view_state);
    QHeaderView* p_header = WidgetUI.objectsTV->header();
    p_header->restoreState(view_state);
}

//------------------------------------------------------------------------------

void CGSDesignerRefBy::SaveDesignerSetup(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele == NULL");
    }

    QHeaderView* p_header = WidgetUI.objectsTV->header();
    p_ele->SetAttribute("refby_header_state",p_header->saveState());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGSDesignerRefBy::ObjectTVDblClicked(const QModelIndex& index)
{
    CProObject* p_obj = dynamic_cast<CProObject*>(ObjectsModel->GetItem(index));
    if( p_obj == NULL ) return;
    p_obj->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

