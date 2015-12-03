#ifndef ImportJobH
#define ImportJobH
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
#include <Job.hpp>

//------------------------------------------------------------------------------

class CStructure;
class CProject;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CImportJob : public CJob {
public:
// constructor and destructor --------------------------------------------------
    CImportJob(CPluginObject* p_objectinfo,CProject* p_project);

    /// set imported data
    virtual bool SetImportedStructure(CStructure* p_mol,const QString& name);

    /// push to recent files stack - only if EJS_FINISHED
    void PushToRecentFiles(void);

    /// adjust graphics for large systems
    void AdjustGraphics(void);

    /// add standard graphic model which will contain structure and optional focus on him
    void AddGraphicModel(bool adjust_graphic);


// set of protected data -------------------------------------------------------
protected:
    CStructure*             Structure;
    QString                 FileName;
};

//------------------------------------------------------------------------------

#endif
