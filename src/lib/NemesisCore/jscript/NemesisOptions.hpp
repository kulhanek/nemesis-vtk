#ifndef NemesisOptionsH
#define NemesisOptionsH
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

#include <SimpleOptions.hpp>
#include <NemesisCoreMainHeader.hpp>

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CNemesisOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CNemesisOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "nemesis"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "NEMESIS is molecular modelling package."
    CSO_PROG_DESC_END

    CSO_PROG_ARGS_SHORT_DESC_BEGIN
    "[ARGS]"
    CSO_PROG_ARGS_SHORT_DESC_END

    CSO_PROG_ARGS_LONG_DESC_BEGIN
    "<b>Arguments:</b>\n"
    "   -npr file       open a nemesis project file\n"
    "   -build          create a build project\n"
    "   -traj           create a trajectory project\n"
    "   -sketch         create a sketch project\n"
    "   -format file    load a structure from the file in the format to the current project"
    CSO_PROG_ARGS_LONG_DESC_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // options ------------------------------
    CSO_OPT(bool,PrintIFormats)
    CSO_OPT(bool,PrintOBExtensions)
    CSO_OPT(bool,GLDebug)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of options -----------------------------------------------------
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                PrintIFormats,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'i',                           /* short option name */
                "printiformats",                      /* long option name */
                NULL,                           /* parametr name */
                "print import formats")   /* option description */

    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                PrintOBExtensions,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'o',                           /* short option name */
                "printobextensions",                      /* long option name */
                NULL,                           /* parametr name */
                "print OpenBabel format extensions")   /* option description */

    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                GLDebug,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "gldebug",                      /* long option name */
                NULL,                           /* parametr name */
                "enable OpenGL debugging")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Verbose,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'v',                           /* short option name */
                "verbose",                      /* long option name */
                NULL,                           /* parametr name */
                "increase output verbosity")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Version,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "version",                      /* long option name */
                NULL,                           /* parametr name */
                "output version information and exit")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Help,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'h',                           /* short option name */
                "help",                      /* long option name */
                NULL,                           /* parametr name */
                "display this help and exit")   /* option description */
    CSO_MAP_END

// final operation with options ------------------------------------------------
private:
    virtual int CheckOptions(void);
    virtual int FinalizeOptions(void);
    virtual int CheckArguments(void);
};

//------------------------------------------------------------------------------

extern  NEMESIS_CORE_PACKAGE CNemesisOptions  NemesisOptions;    // program options

//------------------------------------------------------------------------------

#endif
