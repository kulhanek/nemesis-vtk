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

#include <GLSelection.hpp>
#include <Transformation.hpp>
#include <IndexCounter.hpp>
#include <ProObject.hpp>

//------------------------------------------------------------------------------

CGLLoadedObjects LoadedObjects;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelObject::CSelObject(void)
{
    Object = NULL;
    SubID = 0;
}

//------------------------------------------------------------------------------

CSelObject::CSelObject(CProObject* p_obj,int subid)
{
    Object = p_obj;
    SubID = subid;
}

//------------------------------------------------------------------------------

CSelObject::CSelObject(const CSelObject& src)
{
    Object = src.Object;
    SubID = src.SubID;
}

//------------------------------------------------------------------------------

CProObject* CSelObject::GetObject(void) const
{
    return(Object);
}

//------------------------------------------------------------------------------

int CSelObject::GetSubID(void) const
{
    return(SubID);
}

//------------------------------------------------------------------------------

bool CSelObject::operator == (const CSelObject& left) const
{
    if( Object != left.Object ) return(false);
    if( SubID != left.SubID ) return(false);
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

GLenum GLGetMode(void)
{
    GLint i[1];
    glGetIntegerv(GL_RENDER_MODE,i);
    //return(i[0]);
    // TODO - does not work under Windows
    int j = GL_SELECT;
    return(j);
}

//---------------------------------------------------------------------------

void GLLoadObject(CProObject* p_object,int subid)
{
    int index = LoadedObjects.RegisterObject(p_object,subid);
    glLoadName(index);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGLLoadedObjects::CGLLoadedObjects(void)
{
    MaxBlockSize = 4096;
    TopSize = 0;
    CurrSize = 0;
    ActiveBlock = NULL;
    BlockPosition = 0;
    Iterator.ChangeList(&Blocks);
    AllocateNewBlock();
}

//------------------------------------------------------------------------------

CGLLoadedObjects::~CGLLoadedObjects(void)
{
    // TODO - crash here - it should be fixed?
    CSelObject* p_obj;
    while( (p_obj = Blocks.RemoveFromBegin()) != NULL ){
        delete[] p_obj;
    }
}

//------------------------------------------------------------------------------

bool CGLLoadedObjects::RegisterMasterObject(CProObject* p_object)
{
    int index = RegisterObject(p_object,SEL_MASTER_OBJ);
    return( index != 0 );
}

//------------------------------------------------------------------------------

int CGLLoadedObjects::RegisterObject(CProObject* p_object,int subid)
{
    if( BlockPosition == MaxBlockSize ) {
        if( CurrSize >= TopSize ) {
            AllocateNewBlock();
        } else {
            Iterator++;
            ActiveBlock = Iterator.Current();
            BlockPosition = 0;
        }
    }
    ActiveBlock[BlockPosition].Object = p_object;
    ActiveBlock[BlockPosition].SubID = subid;
    BlockPosition++;

    return(CurrSize++);
}

//------------------------------------------------------------------------------

const CSelObject CGLLoadedObjects::FindObject(unsigned int index) const
{
    if( index >= CurrSize ) return(CSelObject());
    int majorid = index / MaxBlockSize;
    int minorid = index % MaxBlockSize;
    return(Blocks[majorid][minorid]);
}

//------------------------------------------------------------------------------

void CGLLoadedObjects::Clear(void)
{
    MaxBlockSize = 4096;
    TopSize = 0;
    CurrSize = 0;
    ActiveBlock = NULL;
    BlockPosition = 0;
    CSelObject* p_obj;
    while( (p_obj = Blocks.RemoveFromBegin()) != NULL ){
        delete[] p_obj;
    }
    Iterator.SetToBegin();
    AllocateNewBlock();
}

//------------------------------------------------------------------------------

void CGLLoadedObjects::AllocateNewBlock(void)
{
    ActiveBlock = new CSelObject[MaxBlockSize];
    Blocks.InsertToEnd(ActiveBlock);
    BlockPosition = 0;
    TopSize += MaxBlockSize;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
