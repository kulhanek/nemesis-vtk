#ifndef AnglePropertyH
#define AnglePropertyH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <GeoProperty.hpp>

// -----------------------------------------------------------------------------

class CPropertyList;
class CPropertyAtomList;
class CAnglePropertySetup;

// -----------------------------------------------------------------------------

extern CExtUUID NEMESIS_CORE_PACKAGE AnglePropertyID;

// -----------------------------------------------------------------------------

///  Property definition class

class NEMESIS_CORE_PACKAGE CAngleProperty : public CGeoProperty {
Q_OBJECT
public:
// constructors and destructors ------------------------------------------------
    /// constructor.
    CAngleProperty(CPropertyList *p_bl);
    
// informational methods -------------------------------------------------------
    /// is property completed?
    virtual bool IsReady(void);

    /// is property from given structure?
    virtual bool IsFromStructure(CStructure* p_str);

    /// composed from single atom groups?
    virtual bool ComposedBySingleAtomGroups(void);

    /// get property value - scalar value
    virtual double  GetScalarValue(void);

    /// get property cartesian gradient
    virtual double GetGradient(QVector<CAtomGrad>& grads);

    /// get point A
    CPropertyAtomList* GetPointA(void);

    /// get point B
    CPropertyAtomList* GetPointB(void);

    /// get point C
    CPropertyAtomList* GetPointC(void);

    /// has cartesian gradient for given structure
    virtual bool HasGradient(CStructure* p_structure);

// input/output methods --------------------------------------------------------
    /// load atom data
    virtual void LoadData(CXMLElement* p_ele);

    /// save atom data
    virtual void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
protected:
    CPropertyAtomList*      PointA;
    CPropertyAtomList*      PointB;
    CPropertyAtomList*      PointC;

    // graphics
    virtual void Draw(void);

    void LabelAngle(void);

private slots:
    void PropertyAtomListChanged(void);
};

// -----------------------------------------------------------------------------

#endif

