#ifndef AtomLabelObjectH
#define AtomLabelObjectH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GraphicsObject.hpp>

//------------------------------------------------------------------------------

class CGraphicsObjectList;
class CAtomLabelSetup;
class CAtom;
class CStructure;
class CStructureList;
class CResidue;

//------------------------------------------------------------------------------

extern CExtUUID AtomLabelObjectID;

//------------------------------------------------------------------------------

enum EAtomLabelObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EALOF_SHOW_HIDDEN           = 0x00010000,
    EALOF_SHOW_NAME             = 0x00020000,
    EALOF_SHOW_TYPE             = 0x00040000,
    EALOF_SHOW_CHARGE           = 0x00080000,
    EALOF_SHOW_RESNAME          = 0x00100000,
    EALOF_SHOW_CUSTOM           = 0x00200000,
    EALOF_SHOW_LINE             = 0x00400000,
    EALOF_SHOW_HYDROGENS        = 0x00800000,
    EALOF_SHOW_SERIALINDEX      = 0x01000000,
};

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CAtomLabelObject : public CGraphicsObject {
public:
// constructors and destructors ------------------------------------------------
    CAtomLabelObject(CGraphicsObjectList* p_gl);

// setup methods ---------------------------------------------------------------
    /// set custom format
    bool SetCustomFormatWH(const QString& format);

    /// set offset
    bool SetOffsetWH(const CPoint& offset);

    /// set custom format
    void SetCustomFormat(const QString& format,CHistoryNode* p_history=NULL);

    /// set offset
    void SetOffset(const CPoint& offset,CHistoryNode* p_history=NULL);

    /// get custom format
    const QString& GetCustomFormat(void) const;

    /// get custom format
    const CPoint& GetOffset(void) const;

// executive methods -----------------------------------------------------------
    /// draw object
    virtual void Draw(void);

    /// calculate visible object metrics
    virtual void GetObjectMetrics(CObjMetrics& metrics);

// manipulation ----------------------------------------------------------------
    /// begin manipulation
    virtual void StartManipulation(CHistoryNode* p_history);

    /// move by object/scene
    virtual void Move(const CPoint& dmov,CGraphicsView* p_view);

    /// end manipulation
    virtual void EndManipulation(void);

    /// respond to manipulation event
    virtual void RespondToEvent(void);

// input/outpu methods ---------------------------------------------------------
    /// load object specific data
    virtual void LoadData(CXMLElement* p_ele);

    /// save object specific data
    virtual void SaveData(CXMLElement* p_ele);

// section of public data ------------------------------------------------------
private:
    QString             Format;         // custom format
    CPoint              Offset;         // label offset
    CPoint              AutoOffsetDir;  // direction for auto offset - local variable

    /// label structure list
    void LabelStructureList(CStructureList* p_strlist);

    /// label structure
    void LabelStructure(CStructure* p_mol);

    /// label residue
    void LabelResidue(CResidue* p_res);

    /// label atom
    void LabelAtom(CAtom* p_atm);

    /// get structure list metrics
    void GetStructureListMetrics(CStructureList* p_strlist,CObjMetrics& metrics);

    /// get structure metrics
    void GetStructureMetrics(CStructure* p_str,CObjMetrics& metrics);

    /// get residue metrics
    void GetResidueMetrics(CResidue* p_res,CObjMetrics& metrics);

    /// get atom metrics
    void GetAtomMetrics(CAtom* p_atom,CObjMetrics& metrics);
};

//------------------------------------------------------------------------------

#endif
