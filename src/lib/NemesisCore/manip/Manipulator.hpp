#ifndef ManipulatorH
#define ManipulatorH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Point.hpp>

//------------------------------------------------------------------------------

class CHistoryNode;
class CGraphicsView;
class CXMLElement;

//------------------------------------------------------------------------------

/// level of manipulation

enum EDrawLevel {
    EDL_SYSTEM_DRAW,        // system request draw
    EDL_USER_DRAW,          // user request draw
    EDL_MANIP_DRAW,         // scene is drawn during manipulation
    EDL_SELECTION_DRAW      // selection is requested
};

//------------------------------------------------------------------------------

/// base class for all object that can be manipulated

class NEMESIS_CORE_PACKAGE CManipulator {
public:
// constructors and destructors ------------------------------------------------
    CManipulator(void);
    virtual   ~CManipulator(void);

// executive methods -----------------------------------------------------------
    /// begin manipulation - return false to disable manipulation
    virtual bool StartManipulation(void);

    /// begin manipulation
    virtual void StartManipulation(CHistoryNode* p_history);

    /// move by object/scene
    virtual void Move(const CPoint& dmov,CGraphicsView* p_view);

    /// move by object/scene rotation centre
    virtual void MoveCentrum(const CPoint& dmov,CGraphicsView* p_view);

    /// rotate with object/scene
    virtual void Rotate(const CPoint& drot,CGraphicsView* p_view);

    /// scale scene
    virtual void ChangeScale(double increment,CGraphicsView* p_view);

    /// end manipulation
    virtual void EndManipulation(void);

// input/output methods --------------------------------------------------------
    /// load manipulator data
    virtual void LoadManip(CXMLElement* p_ele);

    /// save manipulator data
    virtual void SaveManip(CXMLElement* p_ele);
};

//------------------------------------------------------------------------------

#endif

