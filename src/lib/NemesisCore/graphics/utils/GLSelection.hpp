#ifndef GLSelectionH
#define GLSelectionH
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

#include <NemesisCoreMainHeader.hpp>

// this is here because Windows has defined a macro for GetObject->GetObjectA
// substitution, we need to aply it on all GetObject occurences
#if defined _WIN32 || defined __CYGWIN__
#include <windows.h>
#endif

#include <SimpleList.hpp>
#include <SimpleIterator.hpp>

// -----------------------------------------------------------------------------

class CProObject;

// -----------------------------------------------------------------------------

// special meanings of SubID
#define SEL_MASTER_OBJ                  -1  // master object
#define SEL_INVERT                      -2  // try to invert selection buffer
#define SEL_ENTER_KEY                   -3  // "Enter" key was pressed
#define SEL_ESC_KEY                     -4  // "Esc" key was pressed
#define SEL_COMPLETE_BONDS              -5  // complete bonds (add bonds between selected atoms)
#define SEL_COMPLETE_VALENCES           -6  // complete valences
#define SEL_COMPLETE_MOLECULES          -7  // complete fragments (select entire fragments with selected atoms or bonds)
#define SEL_COMPLETE_ACTIVE_STRUCTURE   -8  // complete active structure
#define SEL_COMPLETE_ALL                -9  // complete all structures or other containers

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CSelObject {
// constructors and destructors ------------------------------------------------
public:
    CSelObject(void);   // construct NULL object
    CSelObject(CProObject* p_obj,int subid);
    CSelObject(const CSelObject& src);

// information methods ---------------------------------------------------------
    /// get selected object
    CProObject* GetObject(void) const;

    /// get its sub id
    int GetSubID(void) const;

    /// compare objects
    bool operator == (const CSelObject& left) const;

// section of private data -----------------------------------------------------
private:
    CProObject*     Object;     // selected object
    int             SubID;      // and its sub ID

    friend class CGLLoadedObjects;
};

// -----------------------------------------------------------------------------

/// load name into selection stack
void   NEMESIS_CORE_PACKAGE GLLoadObject(CProObject* p_object,int subid = 0);

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGLLoadedObjects {
// constructors and destructors ------------------------------------------------
public:
    CGLLoadedObjects(void);
    ~CGLLoadedObjects(void);

    /// register master object (it has index 0)
    bool RegisterMasterObject(CProObject* p_object);

    /// register object
    int RegisterObject(CProObject* p_object,int subid = 0);

    /// release allocated memory blocks and prepare for new selection
    const CSelObject FindObject(unsigned int index) const;

    /// clear load stack
    void Clear(void);

// section of private data ----------------------------------------------------
private:
    unsigned int                MaxBlockSize;
    unsigned int                TopSize;
    unsigned int                CurrSize;
    CSelObject*                 ActiveBlock;
    unsigned int                BlockPosition;

    CSimpleList<CSelObject>     Blocks;
    CSimpleIterator<CSelObject> Iterator;

    void AllocateNewBlock(void);
};

// -----------------------------------------------------------------------------

extern NEMESIS_CORE_PACKAGE CGLLoadedObjects LoadedObjects;

// -----------------------------------------------------------------------------

#endif

