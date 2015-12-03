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

#include <TerminalOStream.hpp>
#include <TerminalWidget.hpp>
#include <TerminalEdit.hpp>
#include <fstream>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTerminalOBuffer::CTerminalOBuffer(void)
{
    TextEdit = NULL;
    Stream = NULL;
    CommandLevel = 0;
}

//------------------------------------------------------------------------------

void CTerminalOBuffer::SetDevice(QTextEdit* p_edit)
{
    TextEdit = p_edit;
    Stream = NULL;
    CommandLevel = 0;
    Command.str("");
    Command.clear();
}

//------------------------------------------------------------------------------

void CTerminalOBuffer::SetDevice(std::ostream* p_stream)
{
    TextEdit = NULL;
    Stream = p_stream;
    CommandLevel = 0;
    Command.str("");
    Command.clear();
}

//------------------------------------------------------------------------------

int CTerminalOBuffer::overflow(int c)
{
    if( TextEdit != NULL ) {
        // levels
        // 12
        // <c>
        // </c>
        // 134

        if( c == '<' ) {
            if( CommandLevel == 1 ) {
                TextEdit->insertPlainText("<");
                Command.str("");
                Command.clear();
                CommandLevel = 0;
                return(0);
            }
            if( CommandLevel != 0 ) {
                TextEdit->insertPlainText("!formating error!");
                Command.str("");
                Command.clear();
                CommandLevel = 0;
                return(0);
            }
            CommandLevel = 1;
            return(0);
        }
        if( c == '/' ) {
            if( CommandLevel == 0 ) {
                TextEdit->insertPlainText(QString((char)c));
                return(0);
            }
            if( CommandLevel != 1 ) {
                TextEdit->insertPlainText("!formating error1!");
                Command.str("");
                Command.clear();
                CommandLevel = 0;
                return(0);
            }
            CommandLevel = 3;
            return(0);
        }
        if( c == '>' ) {
            if( CommandLevel == 0 ) {
                TextEdit->insertPlainText(QString((char)c));
                return(0);
            }
            if( (CommandLevel != 2) && (CommandLevel != 4) ) {
                TextEdit->insertPlainText("!formating error2!");
                Command.str("");
                Command.clear();
                CommandLevel = 0;
                return(0);
            }

            std::string cmd = Command.str();

            // apply command
            if( CommandLevel == 2 ) {
                if( cmd == "b" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontWeight(QFont::Bold);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "u" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontUnderline(true);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "i" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontItalic(true);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "red" ) {
                    TextEdit->setTextColor(QColor("red"));
                } else if ( cmd == "blue" ) {
                    TextEdit->setTextColor(QColor("blue"));
                }
            }

            // revers command
            if( CommandLevel == 4 ) {
                if( cmd == "b" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontWeight(QFont::Normal);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "u" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontUnderline(false);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "i" ) {
                    QTextCharFormat fmt = TextEdit->currentCharFormat();
                    fmt.setFontItalic(false);
                    TextEdit->setCurrentCharFormat(fmt);
                } else if ( cmd == "red" ) {
                    TextEdit->setTextColor(QColor("white"));
                } else if ( cmd == "blue" ) {
                    TextEdit->setTextColor(QColor("white"));
                }
            }

            Command.str("");
            Command.clear();
            CommandLevel = 0;
            return(0);
        }
        if( (CommandLevel == 1) || (CommandLevel == 2) ) {
            Command << (char)c;
            CommandLevel = 2;
            return(0);
        }
        if( (CommandLevel == 3) || (CommandLevel == 4) ) {
            Command << (char)c;
            CommandLevel = 4;
            return(0);
        }
        if( CommandLevel != 0 ) {
            TextEdit->insertPlainText("!formating error4!");
            Command.str("");
            Command.clear();
            CommandLevel = 0;
            return(0);
        }
        TextEdit->insertPlainText(QString((char)c));
        return(0);
    }
    if( Stream != NULL ) {
        *Stream << (char)c;
        return(0);
    }

    return(0);  // anihilate output
}

//------------------------------------------------------------------------------

int CTerminalOBuffer::sync(void)
{
    return(0);  // always in sync
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTerminalOStream::CTerminalOStream(void)
    : std::ostream(&Buffer)
{

}

//------------------------------------------------------------------------------

CTerminalOStream::~CTerminalOStream(void)
{

}

//------------------------------------------------------------------------------

void CTerminalOStream::SetDevice(CTerminalWidget* p_terminal)
{
    if( p_terminal != NULL ) {
        Buffer.SetDevice(p_terminal->EditArea);
    }
}

//------------------------------------------------------------------------------

void CTerminalOStream::SetDevice(std::ostream* p_stream)
{
    if( p_stream != NULL ) {
        Buffer.SetDevice(p_stream);
    }
}

//------------------------------------------------------------------------------

bool CTerminalOStream::IsStdOutput(void)
{
    if( Buffer.Stream == &std::cout ) return(true);
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
