// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <SelectionList.hpp>
#include <SelectionRequest.hpp>
#include <Project.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <SelectionHandler.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <ResidueList.hpp>
#include <BondList.hpp>
#include <Bond.hpp>
#include <Residue.hpp>
#include <ASLMask.hpp>
#include <Graphics.hpp>
#include <GraphicsObjectList.hpp>
#include <StructureList.hpp>
#include <Restraint.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        SelectionListID(
                    "{SELECTION_LIST:f3f06587-a9f4-4e81-ac09-14035ae1544d}",
                    "Selection");

CPluginObject   SelectionListObject(&NemesisCorePlugin,
                    SelectionListID,STRUCTURE_CAT,
                    ":/images/NemesisCore/selection/SelectionList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelectionList::CSelectionList(CProject* p_project,bool no_index)
    : CProObject(&SelectionListObject,p_project,p_project,no_index)
{    
    Request = NULL;
    Status = ESS_LIST_INITIALIZED;
    StrSelMode = ESOSM_CONSIDER_ALL_STRUCTURES;
}

//------------------------------------------------------------------------------

CSelectionList::~CSelectionList(void)
{
    if( Request ){
        Request->SelectionListStatusChanged(ESS_REQUEST_DETACHED);
        Request = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SetRequest(CSelectionRequest* p_request)
{
    if( (Request != p_request) && (Request != NULL) ) {
        if( p_request != NULL ) Request->SelectionListStatusChanged(ESS_REQUEST_DETACHED);
    }

    Request = p_request;
    ResetSelection();

    emit OnSelectionRequestChanged();
    emit OnSelectionChanged();
}

//------------------------------------------------------------------------------

void CSelectionList::RegisterObject(const CSelObject& obj)
{
    if( Request == NULL ) return;
    if( Request->GetRequestType() == NULL ) return;

    if( Status == ESS_SELECTED_OBJECTS_END ) {
        ResetSelection();
    }

    ESelResult selres = Request->GetRequestType()->RegisterObject(this,obj);

    switch(selres) {
        case ESR_NONE_SELECTED:
            Status = ESS_NONE_SELECTED;
            break;
        case ESR_SELECTED_OBJECTS_CHANGED:
            Status = ESS_SELECTED_OBJECTS_CHANGED;
            Request->SelectionListStatusChanged(ESS_SELECTED_OBJECTS_CHANGED);
            break;
        case ESR_SELECTED_OBJECTS_END:
            Status = ESS_SELECTED_OBJECTS_END;
            Request->SelectionListStatusChanged(ESS_SELECTED_OBJECTS_END);
            emit OnSelectionCompleted();
            emit OnSelectionChanged();
            break;
    }
}

//------------------------------------------------------------------------------

void CSelectionList::ResetSelection(void)
{
    while( ! SelectedObjects.isEmpty() ) {
        CSelObject obj = SelectedObjects.first();
        RemoveObject(obj);
    }

    Status = ESS_LIST_INITIALIZED;
    if( Request != NULL ) Request->SelectionListStatusChanged(ESS_LIST_INITIALIZED);
    emit OnSelectionInitialized();
    emit OnSelectionChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SelectAtomsByZ(int z)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectAtomsByZ(z,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }
        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectAtomsByZ(z,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByZ(int z,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) continue;
        if( (p_atom->GetZ() == z) || ( p_atom->IsVirtual() && (z == 1) ) ) {
            CSelObject selobj(p_atom,0);
            RegisterObject(selobj);
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByName(const QRegExp& regexp)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectAtomsByName(regexp,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectAtomsByName(regexp,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByName(const QRegExp& regexp,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) continue;
        if( regexp.exactMatch(p_atom->GetName()) ) {
            CSelObject selobj(p_atom,0);
            RegisterObject(selobj);
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByType(const QRegExp& regexp)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectAtomsByType(regexp,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectAtomsByType(regexp,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByType(const QRegExp& regexp,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) continue;
        if( regexp.exactMatch(p_atom->GetType()) ) {
            CSelObject selobj(p_atom,0);
            RegisterObject(selobj);
        }
    }
}

//------------------------------------------------------------------------------

bool CSelectionList::SelectAtomsByMask(const QString& mask,EAtomMaskType type)
{
    bool result = true;

    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                result &= SelectAtomsByMask(mask,type,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                result &= SelectAtomsByMask(mask,type,p_str);
            }
            break;
        }
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CSelectionList::SelectAtomsByMask(const QString& mask,EAtomMaskType type,CStructure* p_mol)
{
    // TODO type is only ASL

    CASLMask aslmask(p_mol);
    if( aslmask.SetMask(mask) == false ){
        CSmallString error;
        error << "unable to set mask '" << mask << "'";
        ES_ERROR(error);
        return(false);
    }

    foreach(CAtom* p_atom, aslmask.GetSelectedAtoms()){
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) continue;
        CSelObject selobj(p_atom,0);
        RegisterObject(selobj);
    }

    return(true);
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByPlane(double a,double b,double c, double d,bool front)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectAtomsByPlane(a,b,c,d,front,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectAtomsByPlane(a,b,c,d,front,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectAtomsByPlane(double a,double b,double c, double d,bool front,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetAtoms()->children()) {
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        if( p_atom->IsFlagSet(EPOF_SELECTED) ) continue;
        CPoint pos = p_atom->GetPos();
        if( front ){
            if( pos.x*a + pos.y*b + pos.z*c + d > 0 ) {
                CSelObject selobj(p_atom,0);
                RegisterObject(selobj);
            }
        } else {
            if( pos.x*a + pos.y*b + pos.z*c + d < 0 ) {
                CSelObject selobj(p_atom,0);
                RegisterObject(selobj);
            }
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SelectBondsByOrder(EBondOrder order,EComparisonOperator oper)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectBondsByOrder(order,oper,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectBondsByOrder(order,oper,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectBondsByOrder(EBondOrder order,EComparisonOperator oper,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetBonds()->children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsFlagSet(EPOF_SELECTED) ) continue;

        bool select = false;
        switch(oper) {
        case ECO_LT:
            select = p_bond->GetBondOrder() < order;
            break;
        case ECO_LE:
            select = p_bond->GetBondOrder() <= order;
            break;
        case ECO_EQ:
            select = p_bond->GetBondOrder() == order;
            break;
        case ECO_NE:
            select = p_bond->GetBondOrder() != order;
            break;
        case ECO_GE:
            select = p_bond->GetBondOrder() >= order;
            break;
        case ECO_GT:
            select = p_bond->GetBondOrder() > order;
            break;
        }
        if( select ) {
            CSelObject selobj(p_bond,0);
            RegisterObject(selobj);
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectBondsByName(const QRegExp& regexp)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectBondsByName(regexp,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectBondsByName(regexp,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectBondsByName(const QRegExp& regexp,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetBonds()->children()) {
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsFlagSet(EPOF_SELECTED) ) continue;
        if( regexp.exactMatch(p_bond->GetName()) ) {
            CSelObject selobj(p_bond,0);
            RegisterObject(selobj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SelectResiduesByName(const QRegExp& regexp)
{
    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                SelectResiduesByName(regexp,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                SelectResiduesByName(regexp,p_str);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

void CSelectionList::SelectResiduesByName(const QRegExp& regexp,CStructure* p_mol)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    foreach(QObject* p_qobj,p_mol->GetResidues()->children()) {
        CResidue* p_res = static_cast<CResidue*>(p_qobj);
        if( p_res->IsFlagSet(EPOF_SELECTED) ) continue;
        if( regexp.exactMatch(p_res->GetName()) ) {
            CSelObject selobj(p_res,0);
            RegisterObject(selobj);
        }
    }
}

//------------------------------------------------------------------------------

bool CSelectionList::SelectResiduesByMask(const QString& mask,EAtomMaskType type)
{
    bool result = true;

    switch( GetStrObjectSelMode() ){
        case ESOSM_ACTIVE_STRUCTURE_ONLY:
        case ESOSM_STRUCTURES_FROM_OBJECTS: {
            if( GetProject()->GetStructures()->GetActiveStructure() ){
                result &= SelectResiduesByMask(mask,type,GetProject()->GetStructures()->GetActiveStructure());
            }
            break;
        }

        case ESOSM_CONSIDER_ALL_STRUCTURES:{
            foreach(QObject* p_qobj, GetProject()->GetStructures()->children()){
                CStructure* p_str = static_cast<CStructure*>(p_qobj);
                result &= SelectResiduesByMask(mask,type,p_str);
            }
            break;
        }
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CSelectionList::SelectResiduesByMask(const QString& mask,EAtomMaskType type,CStructure* p_mol)
{
    // TODO type is only ASL

    CASLMask aslmask(p_mol);
    if( aslmask.SetMask(mask) == false ){
        CSmallString error;
        error << "unable to set mask '" << mask << "'";
        ES_ERROR(error);
        return(false);
    }

    foreach(CResidue* p_res, aslmask.GetSelectedResidues()){
        if( p_res->IsFlagSet(EPOF_SELECTED) ) continue;
        CSelObject selobj(p_res,0);
        RegisterObject(selobj);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SelectGraphicsObjectsByType(const CUUID& type)
{
    foreach(QObject* p_qobj,GetProject()->GetGraphics()->GetObjects()->children()) {
        CGraphicsObject* p_gobj = static_cast<CGraphicsObject*>(p_qobj);
        if( p_gobj->IsFlagSet(EPOF_SELECTED) ) continue;
        if( p_gobj->GetType() == type ) {
            CSelObject selobj(p_gobj,0);
            RegisterObject(selobj);
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::SetStrObjectSelMode(EStrObjectSelMode mode)
{
    StrSelMode = mode;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

EStrObjectSelMode CSelectionList::GetStrObjectSelMode(void) const
{
    return(StrSelMode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelectionHandler* CSelectionList::GetRequestType(void)
{
    if( Request == NULL ) return(NULL);
    return(Request->GetRequestType());
}

//------------------------------------------------------------------------------

const QString CSelectionList::GetRequestTypeString(void)
{
    if( Request == NULL ) return(NULL);
    return(Request->GetRequestType()->GetHandlerDescription());
}

//------------------------------------------------------------------------------

const QString CSelectionList::GetRequestReason(void)
{
    if( Request == NULL ) return(NULL);
    return(Request->GetRequestReason());
}

//------------------------------------------------------------------------------

const QString CSelectionList::GetRequestorName(void)
{
    if( Request == NULL ) return(NULL);
    return(Request->GetRequestorName());
}

//------------------------------------------------------------------------------

ESelStatus CSelectionList::GetStatus(void)
{
    return(Status);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CSelectionList::NumOfSelectedObjects(void)
{
    return(SelectedObjects.count());
}

//------------------------------------------------------------------------------

int CSelectionList::NumOfSelectedObjects(const CUUID& type)
{
    int count = 0;
    foreach(CSelObject obj,SelectedObjects){
        if( obj.GetObject() == NULL ) continue;
        if( obj.GetObject()->GetType() == type ) count++;
        if( obj.GetObject()->GetPluginObject()->GetCategoryUUID() == type ) count++;
    }
    return(count);
}

//------------------------------------------------------------------------------

CSelObject* CSelectionList::GetSelectedSelObject(int index)
{
    if( index < 0 || index >= SelectedObjects.count() ) return(NULL);
    return(&SelectedObjects[index]);
}

//------------------------------------------------------------------------------

CSelObject CSelectionList::PopSelectedSelObject(void)
{
    if( SelectedObjects.isEmpty() ) return(CSelObject());
    CSelObject obj = SelectedObjects.takeFirst();
    if( obj.GetObject() != NULL ) {
        obj.GetObject()->disconnect(this);
        obj.GetObject()->SetFlag(EPOF_SELECTED,false);
    }
    return(obj);
}

//------------------------------------------------------------------------------

CProObject* CSelectionList::GetSelectedObject(int index)
{
    if( index < 0 || index >= SelectedObjects.count() ) return(NULL);
    return(SelectedObjects[index].GetObject());
}

//------------------------------------------------------------------------------

CProObject* CSelectionList::PopSelectedObject(void)
{
    if( SelectedObjects.isEmpty() ) return(NULL);
    CSelObject obj = SelectedObjects.takeFirst();
    RemoveObject(obj);
    return(obj.GetObject());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelectionList::AddObject(const CSelObject& obj)
{
    SelectedObjects.append(obj);
    if( obj.GetObject() != NULL ) {
        obj.GetObject()->disconnect(this); // try to avoid duplicit signals
        connect(obj.GetObject(),SIGNAL(destroyed(QObject *)),this,SLOT(ObjectDestroyed(QObject *)));
        obj.GetObject()->SetFlag(EPOF_SELECTED,true);
    }
    emit OnObjectAdded(obj);
    emit OnSelectionChanged();
}

//------------------------------------------------------------------------------

void CSelectionList::RemoveObject(const CSelObject& obj)
{
    SelectedObjects.removeAll(obj);
    if( obj.GetObject() != NULL ) {
        obj.GetObject()->disconnect(this);
        obj.GetObject()->SetFlag(EPOF_SELECTED,false);
    }
    emit OnObjectRemoved(obj);
    emit OnSelectionChanged();
}

//------------------------------------------------------------------------------

bool CSelectionList::IsInList(const CSelObject& obj)
{
    return( SelectedObjects.contains(obj) );
}

//------------------------------------------------------------------------------

bool CSelectionList::IsInList(CProObject* p_obj)
{
    QList<CSelObject>::iterator it = SelectedObjects.begin();
    QList<CSelObject>::iterator ie = SelectedObjects.end();

    while( it != ie ){
        if((*it).GetObject() == p_obj) return(true);
        it++;
    }

    return( false );
}

//------------------------------------------------------------------------------

void CSelectionList::ObjectDestroyed(QObject* p_obj)
{
    foreach(CSelObject obj,SelectedObjects) {
        if( obj.GetObject() == p_obj ) {
            SelectedObjects.removeOne(obj);
            //TODO: this is probably dangerous since object could be partially destroyed
            emit OnObjectRemoved(obj);
            emit OnSelectionChanged();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
