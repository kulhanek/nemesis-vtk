// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <AtomHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Atom.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <AtomList.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomHI,
                        "{ATOM:6dbe32fc-557c-4e08-99e3-dccd8b983c39}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomSerIndexHI,
                        "{ATOM_SI:de8a3775-b532-49c0-8aa6-22ec5ed1191d}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomLocIndexHI,
                        "{ATOM_LI:e3a57320-de47-4413-8bba-ebafb822639c}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomTypeHI,
                        "{ATOM_TY:bb4b1596-0349-46e4-9cf2-1fbf90ab08c1}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomZHI,
                        "{ATOM_Z:4e33cb10-a204-4964-80bd-b20423672b3e}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomChargeHI,
                        "{ATOM_CH:f0953fa4-ec28-4116-971d-087887ac14bd}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomPosHI,
                        "{ATOM_POS:3260c3e5-d826-4f9c-93af-5709d09d4a00}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomVelHI,
                        "{ATOM_VEL:757c7583-0ff8-49fe-88bf-69bb05c9393c}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,AtomTrajIndexHI,
                        "{ATOM_TI:39ea118a-95ac-4873-8a88-85389dd89267}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomHI::CAtomHI(CAtom* p_atom,EHistoryItemDirection change)
    : CHistoryItem(&AtomHIObject,p_atom->GetProject(),change)
{
    MoleculeIndex = p_atom->GetStructure()->GetIndex();
    CXMLElement* p_ele = AtomData.CreateChildElement("atom");
    p_atom->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CAtomHI::Forward(void)
{
    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(MoleculeIndex));
    if(p_str == NULL) return;

    CXMLElement* p_ele = AtomData.GetFirstChildElement("atom");
    p_str->GetAtoms()->CreateAtom(p_ele);
}

//------------------------------------------------------------------------------

void CAtomHI::Backward(void)
{
    CXMLElement* p_ele = AtomData.GetFirstChildElement("atom");
    if( p_ele == NULL ) return;
    int atom_index = -1;
    p_ele->GetAttribute("id",atom_index);

    CAtom* p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(atom_index));
    if(p_atom == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_atom->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = AtomData.GetChildElementByPath("atom/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_atom->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CAtomHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = p_ele->GetFirstChildElement("atom");
    if( p_sele == NULL ) return;
    AtomData.RemoveAllChildNodes();
    CXMLElement* p_dele = AtomData.CreateChildElement("atom");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CAtomHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = AtomData.GetFirstChildElement("atom");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("atom");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomSerIndexHI::CAtomSerIndexHI(CAtom* p_atom,int ser_idx)
    : CHistoryItem(&AtomSerIndexHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewSerIndex = ser_idx;
    OldSerIndex = p_atom->GetSerIndex();
}

//------------------------------------------------------------------------------

void CAtomSerIndexHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetSerIndex(NewSerIndex);
}

//------------------------------------------------------------------------------

void CAtomSerIndexHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetSerIndex(OldSerIndex);
}

//------------------------------------------------------------------------------

void CAtomSerIndexHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("ns",NewSerIndex);
    p_ele->GetAttribute("os",OldSerIndex);
}

//------------------------------------------------------------------------------

void CAtomSerIndexHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("ns",NewSerIndex);
    p_ele->SetAttribute("os",OldSerIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomLocIndexHI::CAtomLocIndexHI(CAtom* p_atom,int loc_idx)
    : CHistoryItem(&AtomLocIndexHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewLocIndex = loc_idx;
    OldLocIndex = p_atom->GetLocIndex();
}

//------------------------------------------------------------------------------

void CAtomLocIndexHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetLocIndex(NewLocIndex);
}

//------------------------------------------------------------------------------

void CAtomLocIndexHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetLocIndex(OldLocIndex);
}

//------------------------------------------------------------------------------

void CAtomLocIndexHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("nl",NewLocIndex);
    p_ele->GetAttribute("ol",OldLocIndex);
}

//------------------------------------------------------------------------------

void CAtomLocIndexHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("nl",NewLocIndex);
    p_ele->SetAttribute("ol",OldLocIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomTypeHI::CAtomTypeHI(CAtom* p_atom,const QString& newtype)
    : CHistoryItem(&AtomTypeHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewAtomType = newtype;
    OldAtomType = p_atom->GetType();
}

//------------------------------------------------------------------------------

void CAtomTypeHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetType(NewAtomType);
}

//------------------------------------------------------------------------------

void CAtomTypeHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetType(OldAtomType);
}

//------------------------------------------------------------------------------

void CAtomTypeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("nt",NewAtomType);
    p_ele->GetAttribute("ot",OldAtomType);
}

//------------------------------------------------------------------------------

void CAtomTypeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("nt",NewAtomType);
    p_ele->SetAttribute("ot",OldAtomType);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomZHI::CAtomZHI(CAtom* p_atom,int newz)
    : CHistoryItem(&AtomZHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewZ = newz;
    OldZ = p_atom->GetZ();
}

//------------------------------------------------------------------------------

void CAtomZHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetZ(NewZ);
}

//------------------------------------------------------------------------------

void CAtomZHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetZ(OldZ);
}


//------------------------------------------------------------------------------

void CAtomZHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("nz",NewZ);
    p_ele->GetAttribute("oz",OldZ);
}

//------------------------------------------------------------------------------

void CAtomZHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("nz",NewZ);
    p_ele->SetAttribute("oz",OldZ);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomChargeHI::CAtomChargeHI(CAtom* p_atom,double newcharge)
    : CHistoryItem(&AtomChargeHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewCharge = newcharge;
    OldCharge = p_atom->GetCharge();
}

//------------------------------------------------------------------------------

void CAtomChargeHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetCharge(NewCharge);
}

//------------------------------------------------------------------------------

void CAtomChargeHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetCharge(OldCharge);
}

//------------------------------------------------------------------------------

void CAtomChargeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("nc",NewCharge);
    p_ele->GetAttribute("oc",OldCharge);
}

//------------------------------------------------------------------------------

void CAtomChargeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("nc",NewCharge);
    p_ele->SetAttribute("oc",OldCharge);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomPosHI::CAtomPosHI(CAtom* p_atom,const CPoint& newpos)
    : CHistoryItem(&AtomPosHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();

    NewPos = newpos;
    OldPos = p_atom->GetPos();
}

//------------------------------------------------------------------------------

void CAtomPosHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetPos(NewPos);
}

//------------------------------------------------------------------------------

void CAtomPosHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetPos(OldPos);
}

//------------------------------------------------------------------------------

void CAtomPosHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("npx",NewPos.x);
    p_ele->GetAttribute("npy",NewPos.y);
    p_ele->GetAttribute("npz",NewPos.z);
    p_ele->GetAttribute("opx",OldPos.x);
    p_ele->GetAttribute("opy",OldPos.y);
    p_ele->GetAttribute("opz",OldPos.z);
}

//------------------------------------------------------------------------------

void CAtomPosHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("npx",NewPos.x);
    p_ele->SetAttribute("npy",NewPos.y);
    p_ele->SetAttribute("npz",NewPos.z);
    p_ele->SetAttribute("opx",OldPos.x);
    p_ele->SetAttribute("opy",OldPos.y);
    p_ele->SetAttribute("opz",OldPos.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomVelHI::CAtomVelHI(CAtom* p_atom,const CPoint& newvel)
    : CHistoryItem(&AtomVelHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();

    NewVel = newvel;
    OldVel = p_atom->GetVel();
}

//------------------------------------------------------------------------------

void CAtomVelHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetVel(NewVel);
}

//------------------------------------------------------------------------------

void CAtomVelHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetVel(OldVel);
}

//------------------------------------------------------------------------------

void CAtomVelHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ai",AtomIndex);
    p_ele->GetAttribute("nvx",NewVel.x);
    p_ele->GetAttribute("nvy",NewVel.y);
    p_ele->GetAttribute("nvz",NewVel.z);
    p_ele->GetAttribute("ovx",OldVel.x);
    p_ele->GetAttribute("ovy",OldVel.y);
    p_ele->GetAttribute("ovz",OldVel.z);
}

//------------------------------------------------------------------------------

void CAtomVelHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ai",AtomIndex);
    p_ele->SetAttribute("nvx",NewVel.x);
    p_ele->SetAttribute("nvy",NewVel.y);
    p_ele->SetAttribute("nvz",NewVel.z);
    p_ele->SetAttribute("ovx",OldVel.x);
    p_ele->SetAttribute("ovy",OldVel.y);
    p_ele->SetAttribute("ovz",OldVel.z);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAtomTrajIndexHI::CAtomTrajIndexHI(CAtom* p_atom,int traj_idx)
    : CHistoryItem(&AtomTrajIndexHIObject,p_atom->GetProject(),EHID_FORWARD)
{
    AtomIndex = p_atom->GetIndex();
    NewTrajIndex = traj_idx;
    OldTrajIndex = p_atom->GetTrajIndex();
}

//------------------------------------------------------------------------------

void CAtomTrajIndexHI::Forward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetTrajIndex(NewTrajIndex);
}

//------------------------------------------------------------------------------

void CAtomTrajIndexHI::Backward(void)
{
    CAtom *p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom != NULL) p_atom->SetTrajIndex(OldTrajIndex);
}

//------------------------------------------------------------------------------

void CAtomTrajIndexHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("at",AtomIndex);
    p_ele->GetAttribute("nt",NewTrajIndex);
    p_ele->GetAttribute("ot",OldTrajIndex);
}

//------------------------------------------------------------------------------

void CAtomTrajIndexHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("at",AtomIndex);
    p_ele->SetAttribute("nt",NewTrajIndex);
    p_ele->SetAttribute("ot",OldTrajIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
