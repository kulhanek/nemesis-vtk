#ifndef TerminalOStreamH
#define TerminalOStreamH
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

#include <NemesisCoreMainHeader.hpp>
#include <QTextEdit>
#include <fstream>
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------

class CTerminalWidget;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CTerminalOBuffer : public std::streambuf {
public:
// public methods --------------------------------------------------------------
    CTerminalOBuffer(void);

// setup methods ---------------------------------------------------------------
    /// set output "device" to CTerminalWidget
    void SetDevice(QTextEdit* p_edit);

    /// set output "device" to other stream
    void SetDevice(std::ostream* p_stream);

// section of private data -----------------------------------------------------
private:
    QTextEdit*          TextEdit;
    std::ostream*       Stream;
    std::stringstream   Command;
    int                 CommandLevel;

    // This verbose buffer has no buffer. So every character "overflows"
    // and can be put directly into the redirected buffer.
    virtual int overflow(int c);

    // Sync buffer.
    virtual int sync(void);

    friend class CTerminalOStream;
};

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CTerminalOStream : public std::ostream {
public:
// constructor and destructors ------------------------------------------------
    CTerminalOStream(void);
    ~CTerminalOStream(void);

// setup methods ---------------------------------------------------------------
    /// set output "device" to CTerminalWidget
    void SetDevice(CTerminalWidget* p_terminal);

    /// set output "device" to other stream
    void SetDevice(std::ostream* p_stream);

    /// is cout?
    bool IsStdOutput(void);

// section of private data ----------------------------------------------------
private:
    CTerminalOBuffer        Buffer;
};

// -----------------------------------------------------------------------------

#endif
