// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <SelectionRequest.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <ProObject.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelectionRequest::CSelectionRequest(QObject* p_parent,const QString& requestor_name)
    : QObject(p_parent)
{
    List = NULL;
    Request = NULL;
    RequestorName = requestor_name;
}

//------------------------------------------------------------------------------

CSelectionRequest::~CSelectionRequest(void)
{
    // detach it from selection list
    SetRequest(NULL);
}

//------------------------------------------------------------------------------

void CSelectionRequest::SetRequest(CSelectionList* p_list,CSelectionHandler* p_request,
                                   const QString& reason)
{
    if( (List != p_list) && (List != NULL) ) {
        List->SetRequest(NULL);
    }

    emit OnRequestChanging();

    List = p_list;
    Request = p_request;
    Reason = reason;

    if( List != NULL ) {
        List->SetRequest(this);
    }

}

//------------------------------------------------------------------------------

CSelectionList* CSelectionRequest::GetSelectionList(void)
{
    return(List);
}

//------------------------------------------------------------------------------

CSelectionHandler* CSelectionRequest::GetRequestType(void)
{
    return(Request);
}

//------------------------------------------------------------------------------

const QString CSelectionRequest::GetRequestReason(void)
{
    return(Reason);
}

//------------------------------------------------------------------------------

const QString CSelectionRequest::GetRequestorName(void)
{
    return(RequestorName);
}

//------------------------------------------------------------------------------

bool CSelectionRequest::IsCompleted(void)
{
    if( List == NULL ) return(false);
    return( List->GetStatus() == ESS_SELECTED_OBJECTS_END );
}

//------------------------------------------------------------------------------

bool CSelectionRequest::IsAttached(void)
{
    return(List != NULL);
}

//------------------------------------------------------------------------------

void CSelectionRequest::SelectionListStatusChanged(ESelStatus change)
{
    switch(change) {
        // -----------------------------
        case ESS_REQUEST_DETACHED:
            emit OnDetached();
            List = NULL;
            break;
        // -----------------------------
        case ESS_LIST_INITIALIZED:
            emit OnSelectionInitialized();
            break;
        // -----------------------------
        case ESS_SELECTED_OBJECTS_CHANGED:
            emit OnSelectionChanged();
            break;
        // -----------------------------
        case ESS_SELECTED_OBJECTS_END:
            emit OnSelectionCompleted();
            break;
        // -----------------------------
        default:
            break;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



