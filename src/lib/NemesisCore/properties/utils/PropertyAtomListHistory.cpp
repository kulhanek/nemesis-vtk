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

#include <PropertyAtomListHistory.hpp>
#include <NemesisCoreModule.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <PropertyAtomList.hpp>
#include <ProObjectHistory.hpp>
#include <ErrorSystem.hpp>
#include <Atom.hpp>
#include <XMLElement.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,PropertyAtomListAddAtomHI,
                        "{PATM_L_ADD:9932c33c-ad00-42bd-a6df-9401a34819ab}")

// remove AddAtom macro defined on Windows
#if defined AddAtom
#undef AddAtom
#endif

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPropertyAtomListAddAtomHI::CPropertyAtomListAddAtomHI(CPropertyAtomList* p_go,CAtom* p_at,
                                                           EHistoryItemDirection change)
    : CHistoryItem(&PropertyAtomListAddAtomHIObject,p_go->GetProject(),change)
{
    PropertyAtomListIndex = p_go->GetIndex();
    AtomIndex = p_at->GetIndex();
}

//------------------------------------------------------------------------------

void CPropertyAtomListAddAtomHI::Forward(void)
{
    CPropertyAtomList* p_go = dynamic_cast<CPropertyAtomList*>(GetProject()->FindObject(PropertyAtomListIndex));
    if(p_go == NULL) return;
    CAtom* p_at = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_at == NULL) return;
    p_go->AddAtom(p_at);
}

//------------------------------------------------------------------------------

void CPropertyAtomListAddAtomHI::Backward(void)
{
    CPropertyAtomList* p_go = dynamic_cast<CPropertyAtomList*>(GetProject()->FindObject(PropertyAtomListIndex));
    if(p_go == NULL) return;
    CAtom* p_at = dynamic_cast<CAtom*>(GetProject()->FindObject(AtomIndex));
    if(p_at == NULL) return;
    p_go->RemoveAtom(p_at);
}

//------------------------------------------------------------------------------

void CPropertyAtomListAddAtomHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("pi",PropertyAtomListIndex);
    p_ele->GetAttribute("ai",AtomIndex);
}

//------------------------------------------------------------------------------

void CPropertyAtomListAddAtomHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("pi",PropertyAtomListIndex);
    p_ele->SetAttribute("ai",AtomIndex);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
