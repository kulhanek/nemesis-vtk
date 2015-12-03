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

#include <NemesisCoreModule.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <GOColorModeHistory.hpp>

//------------------------------------------------------------------------------

REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GOColorModeChangeModeHI,
                        "{CM_CHM:1c3da811-d0fa-4957-b1be-33430f0a8112}")
REGISTER_HISTORY_OBJECT(NemesisCorePlugin,GOColorModeUserColorHI,
                        "{CM_UC:3afedc12-9a00-4aaf-ad7b-4ccd89ff5779}")

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOColorModeChangeModeHI::CGOColorModeChangeModeHI(CGOColorMode* p_cm,
                             EGraphicsObjectColorMode newcm)
    : CHistoryItem(&GOColorModeChangeModeHIObject,p_cm->GetProject(),EHID_FORWARD)
{
    ObjectID = p_cm->GetIndex();
    OldMode = p_cm->GetCurrentColorMode();
    NewMode = newcm;
}

//------------------------------------------------------------------------------

void CGOColorModeChangeModeHI::Forward(void)
{
    CGOColorMode* p_cm = dynamic_cast<CGOColorMode*>(GetProject()->FindObject(ObjectID));
    if(p_cm == NULL) return;
    p_cm->SetColorMode(NewMode);
}

//------------------------------------------------------------------------------

void CGOColorModeChangeModeHI::Backward(void)
{
    CGOColorMode* p_cm = dynamic_cast<CGOColorMode*>(GetProject()->FindObject(ObjectID));
    if(p_cm == NULL) return;
    p_cm->SetColorMode(OldMode);
}

//------------------------------------------------------------------------------

void CGOColorModeChangeModeHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    p_ele->GetAttribute("ci",ObjectID);
    p_ele->GetAttribute<EGraphicsObjectColorMode>("nm",NewMode);
    p_ele->GetAttribute<EGraphicsObjectColorMode>("om",OldMode);
}

//------------------------------------------------------------------------------

void CGOColorModeChangeModeHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    p_ele->SetAttribute("ci",ObjectID);
    p_ele->SetAttribute("nm",NewMode);
    p_ele->SetAttribute("om",OldMode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOColorModeUserColorHI::CGOColorModeUserColorHI(CGOColorMode* p_cm,
                                                 const CElementColors& newuc)
    : CHistoryItem(&GOColorModeUserColorHIObject,p_cm->GetProject(),EHID_FORWARD)
{
    ObjectID = p_cm->GetIndex();
    OldColor = p_cm->GetUserColor();
    NewColor = newuc;
}

//------------------------------------------------------------------------------

void CGOColorModeUserColorHI::Forward(void)
{
    CGOColorMode* p_cm = dynamic_cast<CGOColorMode*>(GetProject()->FindObject(ObjectID));
    if(p_cm == NULL) return;
    p_cm->SetUserColor(NewColor);
}

//------------------------------------------------------------------------------

void CGOColorModeUserColorHI::Backward(void)
{
    CGOColorMode* p_cm = dynamic_cast<CGOColorMode*>(GetProject()->FindObject(ObjectID));
    if(p_cm == NULL) return;
    p_cm->SetUserColor(OldColor);
}

//------------------------------------------------------------------------------

void CGOColorModeUserColorHI::LoadData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // load core data ----------------------------
    CHistoryItem::LoadData(p_ele);

    // load local data ---------------------------
    CXMLElement* p_cele;
    p_ele->GetAttribute("ci",ObjectID);

    p_cele = p_ele->GetFirstChildElement("new");
    if( p_cele ){
        NewColor.Load(p_cele);
    }
    p_cele = p_ele->GetFirstChildElement("old");
    if( p_cele ){
        OldColor.Load(p_cele);
    }
}

//------------------------------------------------------------------------------

void CGOColorModeUserColorHI::SaveData(CXMLElement* p_ele)
{
    if( p_ele == NULL ){
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // save core data ----------------------------
    CHistoryItem::SaveData(p_ele);

    // save local data ---------------------------
    CXMLElement* p_cele;
    p_ele->SetAttribute("ci",ObjectID);

    p_cele = p_ele->CreateChildElement("new");
    NewColor.Save(p_cele);
    p_cele = p_ele->CreateChildElement("old");
    OldColor.Save(p_cele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


