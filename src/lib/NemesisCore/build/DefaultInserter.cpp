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

#include <DefaultInserter.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Structure.hpp>
#include <Fragment.hpp>
#include <Project.hpp>
#include <PeriodicTable.hpp>
#include <Atom.hpp>
#include <AtomList.hpp>
#include <BondList.hpp>
#include <ResidueList.hpp>
#include <Residue.hpp>

// -----------------------------------------------------------------------------

QObject* DefaultInserterCB(void* p_data);

CExtUUID        DefaultInserterID(
                    "{DEFAULT_INSERTER:b23ea359-aab5-4972-8604-65adadeb69e4}",
                    "Default structure inserter");

CPluginObject   DefaultInserterObject(&NemesisCorePlugin,
                    DefaultInserterID,INSERTER_CAT,
                    DefaultInserterCB);

// -----------------------------------------------------------------------------

QObject* DefaultInserterCB(void* p_data)
{
    CExtComObject* p_parent = static_cast<CExtComObject*>(p_data);
    CDefaultInserter* p_object = new CDefaultInserter(p_parent);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDefaultInserter::CDefaultInserter(CExtComObject* p_parent)
    : CInserter(&DefaultInserterObject,p_parent)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDefaultInserter::InsertStructure(CFragment* p_str,
                                       CAtom* p_atom,
                                       CHistoryNode* p_history)
{
    if( p_str == NULL ) {
        INVALID_ARGUMENT("p_str is NULL");
    }

    if( p_atom == NULL ) {
        INVALID_ARGUMENT("p_atom is NULL");
    }

    CStructure* p_mol = p_atom->GetStructure();
    CResidue*  p_res = p_atom->GetResidue();
    CProject*  p_prj = p_atom->GetProject();

    // get connection bond
    CBond* p_cbond;
    if( (p_cbond = p_atom->GetConnectBond()) == NULL ) {
        ES_ERROR("no connect bond");
        // no such a bond - terminate
        return(false);
    }

    CAtom* p_catom = p_cbond->GetOppositeAtom(p_atom);
    EBondOrder order = p_cbond->GetBondOrder();

    // set best connector in the structure
    if( p_str->SetBestConnector(order) == false ) {
        ES_ERROR("unable to set best connector");
        // no suitable connector
        return(false);
    }

    int con_id = p_str->GetCurrentConnector();
    int satom_index;
    if( (satom_index = p_str->GetOppositeAtom(con_id)) == -1 ) {
        ES_ERROR("is not structure corrupted");
        // no connected atom in the structure
        return(false);
    }

    // backup coordinates
    CSimpleVector<CPoint>  positions;
    positions.CreateVector(p_str->GetNumOfAtoms());

    // move the structure in such a way that satom_index is in orgin
    for(int i=0; i<p_str->GetNumOfAtoms(); i++) {
        positions[i] = p_str->GetAtomData(i).Pos - p_str->GetAtomData(satom_index).Pos;
    }

    CPoint olddir,newdir;
    olddir = positions[p_str->GetAtomDataFromIndex(con_id)];
    newdir = p_catom->GetPos() - p_atom->GetPos();

    CTransformation trans;
    trans.ChangeDirection(olddir,newdir);

    for(int i=0; i < p_str->GetNumOfAtoms(); i++) {     // change bond dirrection
        trans.Apply(positions[i]);
    }

    double size;
    size = - Size(newdir);
    newdir = newdir /size ;  // norm and revert direction

    int z1,z2;
    z1 = p_catom->GetZ();
    z2 = p_str->GetAtomData(satom_index).Z;

    if( z1 == -1 ) z1 = p_str->GetMasterAtomZ();
    if( z2 == -1 ) z2 = p_str->GetMasterAtomZ();

    newdir = newdir * PeriodicTable.GetBondDistance(z1,z2);   // get length of a bond
    newdir = p_catom->GetPos() + newdir;               // move to final position

    for(int i=0; i < p_str->GetNumOfAtoms(); i++) {          // move the whole structure
        positions[i] = positions[i] + newdir;
    }

    // delete atom
    p_atom->RemoveFromBaseList(p_history);

    p_prj->BeginLinkProcedure();

    CAtom* p_nsatom = NULL;

    for(int i=0; i < p_str->GetNumOfAtoms(); i++) {
        CAtomData atomdata = p_str->GetAtomData(i);
        if( atomdata.Index == con_id ) continue;     // do not insert connector atom
        if( atomdata.Z == -1 ) atomdata.Z = p_str->GetMasterAtomZ();
        atomdata.Pos = positions[i];
        CAtom* p_natom = p_mol->GetAtoms()->CreateAtom(atomdata,p_history);
        if( p_res ){
            p_res->AddAtom(p_natom,p_history);
        }
        if( i == satom_index ) p_nsatom = p_natom;
    }

    // insert bonds
    for(int i=0; i<p_str->GetNumOfBonds(); i++) {
        CBondData bonddata = p_str->GetBondData(i);
        if( (bonddata.A1 == con_id) || (bonddata.A2 == con_id) ) continue; // do not insert connector bond
        p_mol->GetBonds()->CreateBond(bonddata,p_history);
    }

    p_prj->EndLinkProcedure();

    // this must be AFTER EndLinkProcedure()
    p_mol->GetBonds()->CreateBond(p_catom,p_nsatom,order,p_history);

    return(true);
}

//------------------------------------------------------------------------------

bool CDefaultInserter::InsertStructure(CFragment* p_str,
                                       CStructure* p_mol,const CPoint& pos,
                                       CHistoryNode* p_history)
{
    if( p_mol == NULL ) {
        INVALID_ARGUMENT("p_mol is NULL");
    }

    if( p_str == NULL ) {
        INVALID_ARGUMENT("p_str is NULL");
    }

    CProject*  p_prj = p_mol->GetProject();

    CAtomData  atomdata;
    CBondData  bonddata;

    CResidue* p_res = p_mol->GetResidues()->CreateResidue("UNK",p_history);

    // this MUST be after CreateResidue()
    p_prj->BeginLinkProcedure();

    for(int i=0; i < p_str->GetNumOfAtoms(); i++) {
        atomdata = p_str->GetAtomData(i);
        if( atomdata.Z == -1 ) atomdata.Z = p_str->GetMasterAtomZ();
        atomdata.Pos += pos;
        CAtom* p_atom = p_mol->GetAtoms()->CreateAtom(atomdata,p_history);
        p_res->AddAtom(p_atom,p_history);
    }

    for(int i=0; i < p_str->GetNumOfBonds(); i++) {
        bonddata = p_str->GetBondData(i);
        p_mol->GetBonds()->CreateBond(bonddata,p_history);
    }

    p_prj->EndLinkProcedure();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



