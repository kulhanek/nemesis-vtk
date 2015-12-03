#ifndef TerminalWidgetH
#define TerminalWidgetH
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
#include <QWidget>

// -----------------------------------------------------------------------------

class CTerminalEdit;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CTerminalWidget : public QWidget {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CTerminalWidget(QWidget* p_parent,const QString& prompt);
    ~CTerminalWidget(void);

// informative methods --------------------------------------------------------

// setup methods ---------------------------------------------------------------
    /// set background color
    void SetBackgroundColor(const QColor& color);

    /// set foreground color
    void SetForegroundColor(const QColor& color);

    /// set prompt text
    void SetPrompt(const QString& text);

    /// set terminal text font
    void SetFont();

// public signals -------------------------------------------------------------
signals:
    void OnNewLineEntered(const QString& line);

// section of private data ----------------------------------------------------
private:
    CTerminalEdit*      EditArea;
    QColor              BackgroundColor;
    QColor              ForegroundColor;
    QString             Prompt;
    int                 PromptPos;
    QStringList         History;
    int                 HistoryPos;

    // print prompt
    void PrintPrompt(void);

    // key event redirected from CTerminalEdit
    bool KeyPressEvent(QKeyEvent* p_event);

    // mouse event redirected from CTerminalEdit
    bool MouseReleaseEvent(QMouseEvent* p_event);

    // drag or drop events redirected from CTerminalEdit
    bool DragDropEvent(QDropEvent* p_event);


    //update style sheet
    void UpdateStyleSheet(void);

private slots:
    void CheckCursorPosition(void);
    void CursorPositionChanged(void);


    friend class CTerminalEdit;
    friend class CTerminalOStream;
};

// -----------------------------------------------------------------------------

#endif
