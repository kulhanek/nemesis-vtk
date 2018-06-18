#ifndef AdminWorkPanelsMainHeaderH
#define AdminWorkPanelsMainHeaderH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

//------------------------------------------------------------------------------

#if defined _WIN32 || defined __CYGWIN__
#ifdef ADMIN_WORK_PANELS_BUILDING_DLL
#ifdef __GNUC__
#define ADMIN_WORK_PANELS_DLL_PUBLIC __attribute__((dllexport))
#else
#define ADMIN_WORK_PANELS_DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define ADMIN_WORK_PANELS_DLL_PUBLIC __attribute__((dllimport))
#else
#define ADMIN_WORK_PANELS_DLL_PUBLIC __declspec(dllimport)
#endif
#define ADMIN_WORK_PANELS_DLL_LOCAL
#endif
#else
#if __GNUC__ >= 4
#define ADMIN_WORK_PANELS_DLL_PUBLIC __attribute__ ((visibility("default")))
#define ADMIN_WORK_PANELS_DLL_LOCAL  __attribute__ ((visibility("hidden")))
#else
#define ADMIN_WORK_PANELS_DLL_PUBLIC
#define ADMIN_WORK_PANELS_DLL_LOCAL
#endif
#endif

#define ADMIN_WORK_PANELS_PACKAGE ADMIN_WORK_PANELS_DLL_PUBLIC

//------------------------------------------------------------------------------

#endif
