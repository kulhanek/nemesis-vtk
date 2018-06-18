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
#include <GraphicsObject.hpp>
#include <ElementColorsList.hpp>
#include <Project.hpp>
#include <ErrorSystem.hpp>
#include <Atom.hpp>

#include <GOColorMode.hpp>
#include <GOColorModeHistory.hpp>

// -----------------------------------------------------------------------------

CExtUUID        GOColorModeID(
                    "{GO_COLOR_MODE:359c8445-f9dc-4cc1-9d21-8b288634cccd}",
                    "Model Color Mode");

CPluginObject   GOColorModeObject(&NemesisCorePlugin,
                    GOColorModeID,GENERIC_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOColorMode::CGOColorMode(CGraphicsObject* p_object,const QString& name)
    : CProObject(&GOColorModeObject,p_object,p_object->GetProject())
{
    SetName(name);
    SetFlag(EPOF_RO_NAME,true);

    // set list all color modes
    Modes[EGOCM_ATOM_SYMBOL] = tr("by symbol");
    Modes[EGOCM_USER_COLOR] = tr("by user color");

    // default mode
    Mode = EGOCM_ATOM_SYMBOL;

    // default user color
    UserColor.Color.SetRGB(1,0,0);
    UserColor.Diffuse.SetRGB(1,0,0);
}

//------------------------------------------------------------------------------

CGOColorMode::CGOColorMode(CGraphicsObject* p_object,const QString& name,bool no_index)
    : CProObject(&GOColorModeObject,p_object,p_object->GetProject(),no_index)
{
    SetName(name);
    SetFlag(EPOF_RO_NAME,true);

    // set list all color modes
    Modes[EGOCM_ATOM_SYMBOL] = tr("by symbol");
    Modes[EGOCM_USER_COLOR] = tr("by user color");

    // default mode
    Mode = EGOCM_ATOM_SYMBOL;

    // default user color
    UserColor.Color.SetRGB(1,0,0);
    UserColor.Diffuse.SetRGB(1,0,0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGOColorMode::SetColorModeWH(int index)
{
    if( Mode == GetColorMode(index) ) return(true);
    if( GetColorMode(index) == EGOCM_NOT_FOUND ) return(false);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change color mode"));
    if( p_history == NULL ) return (false);

    SetColorMode(index,p_history);

    EndChangeWH();
    return(true);
}

//------------------------------------------------------------------------------

bool CGOColorMode::SetUserColorWH(const CElementColors& color)
{
    if( UserColor == color ) return(true);

    CHistoryNode* p_history = BeginChangeWH(EHCL_GRAPHICS,tr("change user color"));
    if( p_history == NULL ) return (false);

    SetUserColor(color,p_history);

    EndChangeWH();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOColorMode::SetColorMode(int index,CHistoryNode* p_history)
{
    if( GetColorMode(index) == EGOCM_NOT_FOUND ) return;
    SetColorMode(GetColorMode(index),p_history);
}

//------------------------------------------------------------------------------

void CGOColorMode::SetColorMode(EGraphicsObjectColorMode mode,CHistoryNode* p_history)
{
    if( Mode == mode ) return;
    if( p_history != NULL ){
        CHistoryItem* p_item = new CGOColorModeChangeModeHI(this,mode);
        p_history->Register(p_item);
    }
    Mode = mode;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGOColorMode::SetUserColor(const CElementColors& color,CHistoryNode* p_history)
{
    if( color == UserColor ) return;
    if( p_history != NULL ){
        CHistoryItem* p_item = new CGOColorModeUserColorHI(this,color);
        p_history->Register(p_item);
    }
    UserColor = color;
    emit OnStatusChanged(ESC_OTHER);
}

//------------------------------------------------------------------------------

void CGOColorMode::DisableColorMode(EGraphicsObjectColorMode mode)
{
    Modes.remove(mode);
}

//------------------------------------------------------------------------------

EGraphicsObjectColorMode CGOColorMode::GetColorMode(int index)
{
    QMap<EGraphicsObjectColorMode,QString>::iterator it = Modes.begin();
    QMap<EGraphicsObjectColorMode,QString>::iterator ie = Modes.end();
    EGraphicsObjectColorMode mode = EGOCM_NOT_FOUND;
    int i = 0;
    while( it != ie ){
        if( index == i ){
            mode = it.key();
            break;
        }
        i++;
        it++;
    }
    return(mode);
}

//------------------------------------------------------------------------------

int CGOColorMode::GetColorMode(void)
{
    QMap<EGraphicsObjectColorMode,QString>::iterator it = Modes.begin();
    QMap<EGraphicsObjectColorMode,QString>::iterator ie = Modes.end();
    int i = 0;
    while( it != ie ){
        if( it.key() == Mode ){
            return(i);
        }
        i++;
        it++;
    }
    return(-1);
}

//------------------------------------------------------------------------------

EGraphicsObjectColorMode CGOColorMode::GetCurrentColorMode(void)
{
    return(Mode);
}

//------------------------------------------------------------------------------

bool CGOColorMode::IsColorModeEnabled(EGraphicsObjectColorMode mode)
{
    return( Modes.count(mode) > 0 );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CGOColorMode::GetNumberOfColorModes(void)
{
    return(Modes.count());
}

//------------------------------------------------------------------------------

const QString CGOColorMode::GetColorModeName(int index)
{
    QMap<EGraphicsObjectColorMode,QString>::iterator it = Modes.begin();
    QMap<EGraphicsObjectColorMode,QString>::iterator ie = Modes.end();
    int i = 0;
    while( it != ie ){
        if( index == i ){
            return(it.value());
        }
        i++;
        it++;
    }
    return(QString());
}

//------------------------------------------------------------------------------

const CElementColors& CGOColorMode::GetUserColor(void)
{
    return(UserColor);
}

//------------------------------------------------------------------------------

CColor* CGOColorMode::GetColor(CAtom* p_atom)
{
    if( p_atom == NULL ){
        return(&(UserColor.Color));
    }
    switch(Mode){
        // ---------------------------------------
        case EGOCM_USER_COLOR:
            return(&(UserColor.Color));
        // ---------------------------------------
        case EGOCM_ATOM_SYMBOL:
        default:
            return(&(ColorsList.GetElementColorPointer(p_atom->GetZ())->Color));
    }
    return(&(UserColor.Color));
}

//------------------------------------------------------------------------------

CElementColors* CGOColorMode::GetElementColor(CAtom* p_atom)
{
    if( p_atom == NULL ){
        return(&UserColor);
    }
    switch(Mode){
        // ---------------------------------------
        case EGOCM_USER_COLOR:
            return(&UserColor);
        // ---------------------------------------
        case EGOCM_ATOM_SYMBOL:
        default:
            return(ColorsList.GetElementColorPointer(p_atom->GetZ()));
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOColorMode::LoadData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // allow user specific element
    //    if( p_ele->GetName() != "cmode" ) {
    //        LOGIC_ERROR("p_ele is not cmode");
    //    }

    // load object info -----------------------------
    CProObject::LoadData(p_ele);

    // load color modes -----------------------------
    p_ele->GetAttribute<EGraphicsObjectColorMode>("mode",Mode);

    // user color mode
    CXMLElement* p_cele = p_ele->GetFirstChildElement("user");
    if( p_cele ){
        UserColor.Load(p_cele);
    }
}

//------------------------------------------------------------------------------

void CGOColorMode::SaveData(CXMLElement* p_ele)
{
    // check input data -----------------------------
    if( p_ele == NULL ) {
        INVALID_ARGUMENT("p_ele is NULL");
    }

    // allow user specific element
    //    if( p_ele->GetName() != "cmode" ) {
    //        LOGIC_ERROR("p_ele is not object");
    //    }

    // save object info -----------------------------
    CProObject::SaveData(p_ele);

    // save color modes -----------------------------
    p_ele->SetAttribute("mode",Mode);

    // user color mode
    CXMLElement* p_cele = p_ele->CreateChildElement("user");
    UserColor.Save(p_cele);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


