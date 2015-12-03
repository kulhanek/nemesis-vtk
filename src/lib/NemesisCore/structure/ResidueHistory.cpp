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

#include <ResidueHistory.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <Residue.hpp>
#include <Atom.hpp>
#include <ErrorSystem.hpp>
#include <ResidueList.hpp>
#include <ProObjectHistory.hpp>
#include <XMLElement.hpp>
#include <NemesisCoreModule.hpp>

// remove AddAtom macro defined on Windows
#if defined AddAtom
#undef AddAtom
#endif

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ResidueHI,
                        "{RES:0a5c9813-022a-4bea-a3df-b9c05aa51121}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ResidueAddAtomHI,
                        "{RES_ADD:dd19b3dc-641d-49bc-8d9d-5b1c66d8b444}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ResidueChainHI,
                        "{RES_CH:47955f7c-43f9-4a0b-8bd5-19125b62db82}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ResidueTypeHI,
                        "{RES_TY:ecefe3e0-65c0-4345-83d3-1b24f3c2e847}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,ResidueSeqIndexHI,
                        "{RES_SI:96850be2-9121-48f3-b629-8324945e461f}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueHI::CResidueHI(CResidue* p_res,EHistoryItemDirection change)
    : CHistoryItem(&ResidueHIObject,p_res->GetProject(),change)
{
    MoleculeIndex = p_res->GetStructure()->GetIndex();
    CXMLElement* p_ele = ResidueData.CreateChildElement("residue");
    p_res->SaveData(p_ele);
}

//------------------------------------------------------------------------------

void CResidueHI::Forward(void)
{
    CStructure* p_str = dynamic_cast<CStructure*>(GetProject()->FindObject(MoleculeIndex));
    if(p_str == NULL) return;

    CXMLElement* p_ele = ResidueData.GetFirstChildElement("residue");
    p_str->GetResidues()->CreateResidue(p_ele);
}

//------------------------------------------------------------------------------

void CResidueHI::Backward(void)
{
    CXMLElement* p_ele = ResidueData.GetFirstChildElement("residue");
    if( p_ele == NULL ) return;
    int resid = -1;
    p_ele->GetAttribute("id",resid);

    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(resid));
    if(p_res == NULL) return;

    // save designer setup
    CXMLElement* p_sdele = p_res->GetDesignerData();
    if( p_sdele ){
        CXMLElement* p_tdele = ResidueData.GetChildElementByPath("residue/designer",true);
        p_tdele->RemoveAllChildNodes();
        p_tdele->RemoveAllAttributes();
        p_tdele->CopyContentsFrom(p_sdele);
    }

    p_res->RemoveFromBaseList();
}

//------------------------------------------------------------------------------

void CResidueHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = p_ele->GetFirstChildElement("residue");
    if( p_sele == NULL ) return;
    ResidueData.RemoveAllChildNodes();
    CXMLElement* p_dele = ResidueData.CreateChildElement("residue");
    p_dele->CopyContentsFrom(p_sele);
}

//------------------------------------------------------------------------------

void CResidueHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("mi",MoleculeIndex);
    CXMLElement* p_sele = ResidueData.GetFirstChildElement("residue");
    if( p_sele == NULL ) return;
    CXMLElement* p_dele = p_ele->CreateChildElement("residue");
    p_dele->CopyContentsFrom(p_sele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueAddAtomHI::CResidueAddAtomHI(CResidue* p_res,CAtom* p_atom,
                                        EHistoryItemDirection change)
    : CHistoryItem(&ResidueAddAtomHIObject,p_res->GetProject(),change)
{
    NewResidueIndex = p_res->GetIndex();
    if( p_atom->GetResidue() ){
        OldResidueIndex = p_atom->GetResidue()->GetIndex();
    } else {
        OldResidueIndex = -1;
    }
    AtomIndex = p_atom->GetIndex();
}

//------------------------------------------------------------------------------

void CResidueAddAtomHI::Forward(void)
{
    CAtom* p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom == NULL) return;

    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(NewResidueIndex));
    if(p_res != NULL) {
        p_res->AddAtom(p_atom);
    } else {
        CResidue* p_ores = p_atom->GetResidue();
        if( p_ores ) p_ores->RemoveAtom(p_atom);
    }
}

//------------------------------------------------------------------------------

void CResidueAddAtomHI::Backward(void)
{
    CAtom* p_atom = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_atom == NULL) return;

    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(OldResidueIndex));
    if(p_res != NULL){
        p_res->AddAtom(p_atom);
    } else {
        CResidue* p_ores = p_atom->GetResidue();
        if( p_ores ) p_ores->RemoveAtom(p_atom);
    }
}

//------------------------------------------------------------------------------

void CResidueAddAtomHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("or",OldResidueIndex);
    p_ele->GetAttribute("nr",NewResidueIndex);
    p_ele->GetAttribute("ai",AtomIndex);
}

//------------------------------------------------------------------------------

void CResidueAddAtomHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("or",OldResidueIndex);
    p_ele->SetAttribute("nr",NewResidueIndex);
    p_ele->SetAttribute("ai",AtomIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueChainHI::CResidueChainHI(CResidue* p_res,const QString& newchain)
    : CHistoryItem(&ResidueChainHIObject,p_res->GetProject(),EHID_FORWARD)
{
    ResidueIndex = p_res->GetIndex();
    OldChain = p_res->GetChain();
    NewChain = newchain;
}

//------------------------------------------------------------------------------

void CResidueChainHI::Forward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetChain(NewChain);
}

//------------------------------------------------------------------------------

void CResidueChainHI::Backward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetChain(OldChain);
}


//------------------------------------------------------------------------------

void CResidueChainHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ri",ResidueIndex);
    p_ele->GetAttribute("nc",NewChain);
    p_ele->GetAttribute("oc",OldChain);
}

//------------------------------------------------------------------------------

void CResidueChainHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ri",ResidueIndex);
    p_ele->SetAttribute("nc",NewChain);
    p_ele->SetAttribute("oc",OldChain);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueTypeHI::CResidueTypeHI(CResidue* p_res,const QString& newType)
    : CHistoryItem(&ResidueTypeHIObject,p_res->GetProject(),EHID_FORWARD)
{
    ResidueIndex = p_res->GetIndex();
    OldType = p_res->GetType();
    NewType = newType;
}

//------------------------------------------------------------------------------

void CResidueTypeHI::Forward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetType(NewType);
}

//------------------------------------------------------------------------------

void CResidueTypeHI::Backward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetType(OldType);
}


//------------------------------------------------------------------------------

void CResidueTypeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ri",ResidueIndex);
    p_ele->GetAttribute("nt",NewType);
    p_ele->GetAttribute("ot",OldType);
}

//------------------------------------------------------------------------------

void CResidueTypeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ri",ResidueIndex);
    p_ele->SetAttribute("nt",NewType);
    p_ele->SetAttribute("ot",OldType);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResidueSeqIndexHI::CResidueSeqIndexHI(CResidue* p_res,int newseqidx)
    : CHistoryItem(&ResidueSeqIndexHIObject,p_res->GetProject(),EHID_FORWARD)
{
    ResidueIndex = p_res->GetIndex();
    OldSeqIndex = p_res->GetSeqIndex();
    NewSeqIndex = newseqidx;
}

//------------------------------------------------------------------------------

void CResidueSeqIndexHI::Forward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetSeqIndex(NewSeqIndex);
}

//------------------------------------------------------------------------------

void CResidueSeqIndexHI::Backward(void)
{
    CResidue* p_res = dynamic_cast<CResidue*>(GetProject()->FindObject(ResidueIndex));
    if(p_res == NULL) return;

    p_res->SetSeqIndex(OldSeqIndex);
}

//------------------------------------------------------------------------------

void CResidueSeqIndexHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ri",ResidueIndex);
    p_ele->GetAttribute("ns",NewSeqIndex);
    p_ele->GetAttribute("os",OldSeqIndex);
}

//------------------------------------------------------------------------------

void CResidueSeqIndexHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ri",ResidueIndex);
    p_ele->SetAttribute("ns",NewSeqIndex);
    p_ele->SetAttribute("os",OldSeqIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


