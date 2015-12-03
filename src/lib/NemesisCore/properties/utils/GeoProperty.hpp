#ifndef GeoPropertyH
#define GeoPropertyH
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

#include <NemesisCoreMainHeader.hpp>
#include <GraphicsProperty.hpp>
#include <QList>

#if defined _WIN32 || defined __CYGWIN__
#undef DrawText
#endif

//------------------------------------------------------------------------------

enum EGeoPropertyObjectFlag {
    // user flags               = 0x00010000    // first user flag
    EGPOF_SHOW_LABEL            = 0x00010000,
    EGPOF_RELATIVE_LABEL_POS    = 0x00020000,
    EGPOF_SHOW_LABEL_QUOTATION  = 0x00040000,
    EGPOF_SHOW_COM              = 0x00080000,
    EGPOF_SHOW_COM_QUOTATION    = 0x00100000
};

//------------------------------------------------------------------------------

class CGeoPropertySetup;
class CAtom;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CGeoProperty : public CGraphicsProperty {
public:
// constructors and destructors ------------------------------------------------
    CGeoProperty(CPluginObject* p_po,CPropertyList *p_bl);
    ~CGeoProperty(void);

// setup methods ---------------------------------------------------------------
    /// set label position
    bool SetLabelPositionWH(const CPoint& pos);

// setup methods ---------------------------------------------------------------
    /// set label position
    void SetLabelPosition(const CPoint& pos,CHistoryNode* p_history=NULL);

// information methods ---------------------------------------------------------
    /// get label position
    const CPoint& GetLabelPosition(void);

// input/output methods --------------------------------------------------------
    virtual void LoadData(CXMLElement* p_ele);
    virtual void SaveData(CXMLElement* p_ele);

// section of private data -----------------------------------------------------
protected:
    CPoint              LabelPos;
    CGeoPropertySetup*  Setup;

    void DrawText(const CSimplePoint<float>& pos,const QString& label);
    void DrawLabelQuotationLine(CSimplePoint<float>& p1,CSimplePoint<float>& p2);
    void DrawCOMPosition(CSimplePoint<float>& com);
    void DrawCOMQuotation(CSimplePoint<float>& com,const QList<CAtom*>& atoms);
};

//------------------------------------------------------------------------------

#endif
