#ifndef ASLMaskH
#define ASLMaskH
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

#include <NemesisCoreMainHeader.hpp>
#include <QList>
#include <QString>
#include <QObject>

//---------------------------------------------------------------------------

class CASLSelection;
class CStructure;
class CAtom;
class CBond;
class CResidue;

//---------------------------------------------------------------------------

/// mask support class
/*!
 mask support is now fully compatible with AMBER 9.0
*/

class NEMESIS_CORE_PACKAGE CASLMask : public QObject {
public:
    CASLMask(CStructure* p_mol);
    ~CASLMask(void);

// mask setup ------------------------------------------------------------------
    /// set mask from string specification
    bool SetMask(const QString& mask);

    /// return current mask specification
    const QString& GetMask(void) const;

// results ---------------------------------------------------------------------
    /// return selected atoms
    const QList<CAtom*>     GetSelectedAtoms(void);

    /// return selected bonds
    const QList<CBond*>     GetSelectedBonds(void);

    /// return selected residues
    const QList<CResidue*>  GetSelectedResidues(void);

// section of private data ----------------------------------------------------
private:
    CStructure*      Structure;
    QString         Mask;
    CASLSelection*  Selection;

    /// return list of atoms
    const QList<QObject*> GetAtoms(void);

    /// return list of residues
    const QList<QObject*> GetResidues(void);

    friend class CASLSelection;
};

//---------------------------------------------------------------------------

#endif
