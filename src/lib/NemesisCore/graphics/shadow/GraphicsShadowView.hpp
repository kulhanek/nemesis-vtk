#ifndef GraphicsShadowViewH
#define GraphicsShadowViewH
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

#include <GraphicsCommonView.hpp>
#include <ComObject.hpp>

// -----------------------------------------------------------------------------

class CXMLElement;
class CGraphicsShadowViewDockWidget;
class CProject;
class CGraphicsShadowControls;
class CGraphicsShadowFSNotice;
class CGraphicsShadowCtrlNotice;

// -----------------------------------------------------------------------------

/// shadow view widget

class CGraphicsShadowView : public CGraphicsCommonView, public CComObject {
Q_OBJECT
public:
// constructor and destructors -------------------------------------------------
    CGraphicsShadowView(CGraphicsView* p_view);
    ~CGraphicsShadowView();

// input/output methods --------------------------------------------------------
    /// show full screen
    void ShowFullScreen(void);

    /// show normal
    void ShowNormal(void);

    /// close shadow view
    void CloseShadowView(void);

// section of private data -----------------------------------------------------
private:
    CGraphicsShadowViewDockWidget*  DockWidget;
    bool                            FullScreen;
    // controls
    CGraphicsShadowControls*        ControlsW;
    bool                            MoveControlsUp;
    int                             ControlsOffset;
    CGraphicsShadowFSNotice*        FSNoticeW;
    CGraphicsShadowCtrlNotice*      CtrlNoticeW;

    /// put view into DockWidget container
    bool SetDockWidget(bool set);

    /// show view
    void Show(void);

    /// data management
    void LoadShadowSetup(void);
    void SaveShadowSetup(void);
    void LoadShadowSetupMain(CXMLElement* p_ele);
    void SaveShadowSetupMain(CXMLElement* p_ele);

    /// handle close event
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* p_event);
    virtual void keyPressEvent(QKeyEvent* p_event);

private slots:
    void ShowControls(void);
    void HideControls(void);
    void ShallHideControls(void);
    void ControlsViewStateUpdate(void);

    void ShowFSNotice(void);
    void FSNoticeStateUpdate(void);

    void ShowCtrlNotice(void);
    void CtrlNoticeStateUpdate(void);

    void AutoFit(void);
    void OpenObjectDesigner(void);
    void SyncToPrimary(void);
    void ViewChanged(void);
    void ViewDestroyed(void);
    void ToggleFullScreen(void);
    void SavePositionAsDefault(void);

    friend class CGraphicsShadowViewDockWidget;
};

// -----------------------------------------------------------------------------

#endif
