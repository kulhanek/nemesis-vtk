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

#include <MouseDriver.hpp>
#include <MouseHandler.hpp>
#include <SelectionList.hpp>
#include <GraphicsView.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMouseDriver::CMouseDriver(CMouseHandler* p_handler)
{
    Handler = p_handler;
    ActiveView = NULL;
    ManipLevel = EML_YZ;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMouseDriver::MousePressEvent(QMouseEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::MouseMoveEvent(QMouseEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::MouseReleaseEvent(QMouseEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::WheelEvent(QWheelEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::KeyPressEvent(QKeyEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::KeyReleaseEvent(QKeyEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::EnterEvent(QEvent* p_event)
{
    // nothing to be here
}

//------------------------------------------------------------------------------

void CMouseDriver::LeaveEvent(QEvent* p_event)
{
    // nothing to be here
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CMouseDriver::SelectObject(int x,int y)
{
    if( GetSelection() == NULL ) return;
    if( ActiveView == NULL ) return;

    // do selection
    CSelObject obj = ActiveView->SelectObject(x,y);
    GetSelection()->RegisterObject(obj);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMouseHandler* CMouseDriver::GetHandler(void)
{
    return(Handler);
}

//------------------------------------------------------------------------------

CSelectionList* CMouseDriver::GetSelection(void)
{
    if( Handler == NULL ) return(NULL);
    return( Handler->GetSelection() );
}

//------------------------------------------------------------------------------

CHistoryList* CMouseDriver::GetHistory(void)
{
    if( Handler == NULL ) return(NULL);
    return( Handler->GetHistory() );
}

//------------------------------------------------------------------------------

CGraphicsViewList* CMouseDriver::GetViews(void)
{
    if( Handler == NULL ) return(NULL);
    return(Handler->GetViews());
}

//------------------------------------------------------------------------------

CGraphicsView* CMouseDriver::GetActiveView(void)
{
    return(ActiveView);
}

//------------------------------------------------------------------------------

EManipLevel CMouseDriver::GetManipLevel(void)
{
    return(ManipLevel);
}

//------------------------------------------------------------------------------

void CMouseDriver::SetActiveView(CGraphicsView* p_view)
{
    ActiveView = p_view;
}

//------------------------------------------------------------------------------

void CMouseDriver::SetCursor(const QCursor& cursor)
{
    if( ActiveView ){
        ActiveView->SetCursor(cursor);
    }
}

//------------------------------------------------------------------------------

CManipulator* CMouseDriver::GetManipulator(void)
{
    return(ActiveView);
}

//------------------------------------------------------------------------------

void CMouseDriver::ResetManipulation(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


