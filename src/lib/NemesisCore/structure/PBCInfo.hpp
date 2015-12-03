#ifndef PBCInfoH
#define PBCInfoH
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
#include <Point.hpp>
#include <stdlib.h>

//------------------------------------------------------------------------------

class CStructure;
class CHistoryNode;
class CXMLElement;

//------------------------------------------------------------------------------

/// implementation of PBC information

class NEMESIS_CORE_PACKAGE CPBCInfo {
public:
// constructors and destructors -----------------------------------------------
    CPBCInfo(void);
    CPBCInfo(const CPBCInfo& src);

// PBC support ---------------------------------------------------------------
    /// is PBC enabled, e.g. data are valid and PBC is enabled at least in one direction
    bool    IsPBCEnabled(void) const;

    /// is 3D PBC enabled, e.g. data are valid and PBC is enabled in all directions
    bool    Is3DPBCEnabled(void) const;

    /// is setup valid?
    bool    IsValid(void) const;

    /// is periodic along A vector
    bool    IsPeriodicAlongA(void) const;

    /// is periodic along B vector
    bool    IsPeriodicAlongB(void) const;

    /// is periodic along C vector
    bool    IsPeriodicAlongC(void) const;

    /// is rectangular box
    bool    IsRectangularBox(void) const;

    /// get sizes
    CPoint  GetSizes(void) const;

    /// get a vector size
    double  GetAVectorSize(void) const;

    /// get b vector size
    double  GetBVectorSize(void) const;

    /// get c vector size
    double  GetCVectorSize(void) const;

    /// get angles
    CPoint  GetAngles(void) const;

    /// get alpha angle
    double  GetAlpha(void) const;

    /// get beta angle
    double  GetBeta(void) const;

    /// get gamma angle
    double  GetGamma(void) const;

    /// get cell volume
    double  GetVolume(void) const;

    /// get largest inscribe sphere radius
    double  GetLargestSphereRadius(void) const;

    /// get box center
    const CPoint GetBoxCenter(void) const;

    /// get A vector
    const CPoint GetAVector(void) const;

    /// get B vector
    const CPoint GetBVector(void) const;

    /// get C vector
    const CPoint GetCVector(void) const;

    //--------------------------------------------------------------------------
    /// set box dimmensions
    void SetDimmensions(double a, double b, double c, double alpha, double beta, double gamma);

    /// set box dimmensions
    void SetDimmensions(const CPoint& sizes, const CPoint& angles);

    /// set box periodicity
    void SetPeriodicity(bool pa, bool pb, bool pc);

    //--------------------------------------------------------------------------
    /// image point
    const CPoint ImagePoint(const CPoint& pos,int kx,int ky,int kz,
                            bool origin,bool familiar);

    /// image vector
    const CPoint ImageVector(const CPoint& vec);

// input/output methods -------------------------------------------------------
    /// load box setup
    void LoadData(CXMLElement* p_ele);

    /// save box setup
    void SaveData(CXMLElement* p_ele);

// section of private data ----------------------------------------------------
private:
    // PBC data
    bool            PeriodicA;
    bool            PeriodicB;
    bool            PeriodicC;
    double          A,B,C;
    double          Alpha,Beta,Gamma;
    double          UCELL[3][3];
    double          RECIP[3][3];
    bool            ValidData;

    double          Volume;     // volume of the cell
    double          Radius;     // largest inscribed sphere radius
};

//------------------------------------------------------------------------------

#endif
