#ifndef JobThreadH
#define JobThreadH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QThread>

//------------------------------------------------------------------------------

class CJob;

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CJobThread : private QThread {
public:
// constructor and destructor -------------------
    CJobThread(CJob* p_job);

// section of private data -----------------------------------------------------
private:
    CJob*   Job;

    /// job thread main execution point
    virtual void run(void);

    friend class CJob;
};

//------------------------------------------------------------------------------

#endif
