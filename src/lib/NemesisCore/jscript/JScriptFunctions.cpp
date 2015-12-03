// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <JScriptFunctions.hpp>
#include <NemesisJScript.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue NemesisQuit(QScriptContext* p_ctx,QScriptEngine* p_eng)
{
    NemesisJScript->PostCloseEvent();
    return(QScriptValue());
}

//------------------------------------------------------------------------------

QScriptValue NemesisPrint(QScriptContext* p_ctx,QScriptEngine* p_eng)
{
    for(int i=0; i < p_ctx->argumentCount(); i++) {
        QScriptValue arg = p_ctx->argument(i);
        if( arg.isNumber() ) {
            tout << arg.toNumber();
        }
    }
    tout << endl;

    return(QScriptValue());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

