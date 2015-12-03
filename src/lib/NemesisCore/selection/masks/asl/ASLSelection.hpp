#ifndef ASLSelectionH
#define ASLSelectionH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//                  This code is based on ASL mask implementation in
//                  Amber Support Library
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

#include "maskparser/ASLParser.hpp"
#include <QMap>
#include <QObject>

//---------------------------------------------------------------------------

class CASLMask;
class CAtom;

//------------------------------------------------------------------------------

/// selection of atoms

class CASLSelection : public QObject {
public:
    CASLSelection(CASLMask* p_mask,QObject* p_owner);

// executive methods  ----------------------------------------------------------
    bool ExpandAndReduceTree(struct SExpression* p_expr);

// section of private data -----------------------------------------------------
private:
    CASLMask*           Mask;
    QMap<CAtom*,bool>   Selection;

    // individual selections
    bool Select(struct SSelection* p_sel);

    void SelectAtomByIndex(int index,int length);
    void SelectAtomByName(const char* p_name);
    void SelectAtomByType(const char* p_name);

    void SelectResidueByIndex(int index,int length);
    void SelectResidueByName(const char* p_name);

    bool SelectAtomByDistanceFromOrigin(SOperator dist_oper,double dist);
    bool SelectAtomByDistanceFromCentreOfBox(SOperator dist_oper,double dist);
    bool SelectAtomByDistanceFromList(CASLSelection* p_left,
            SOperator dist_oper,double dist);
    bool SelectAtomByDistanceFromCOM(CASLSelection* p_left,
            SOperator dist_oper,double dist);
    bool SelectAtomByDistanceFromPlane(CASLSelection* p_left,
            SOperator dist_oper,double dist);

    bool SelectResidueByDistanceFromOrigin(SOperator dist_oper,double dist);
    bool SelectResidueByDistanceFromCentreOfBox(SOperator dist_oper,double dist);
    bool SelectResidueByDistanceFromList(CASLSelection* p_left,
            SOperator dist_oper,double dist);
    bool SelectResidueByDistanceFromCOM(CASLSelection* p_left,
            SOperator dist_oper,double dist);
    bool SelectResidueByDistanceFromPlane(CASLSelection* p_left,
            SOperator dist_oper,double dist);

    int   strnlen(const char* p_s1,int len);
    bool  firstmatch(const char* p_s1,const char* p_s2,int len);

    friend class CASLMask;
};

//---------------------------------------------------------------------------

#endif
