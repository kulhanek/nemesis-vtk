#ifndef RenderWindowH
#define RenderWindowH
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

#include <QDialog>
#include "ui_RenderWindow.h"

//------------------------------------------------------------------------------

class CProject;

//------------------------------------------------------------------------------

class CRenderWindow : public QDialog {
    Q_OBJECT
public:
// constructor and destructors ------------------------------------------------
    CRenderWindow(CProject* p_project);
    virtual ~CRenderWindow(void);

// section of private data ----------------------------------------------------
private:
    Ui::RenderWindow    WidgetUI;
    CProject*           Project;
    QLabel*             LabelPicture;
    QImage              SceneImage;
    double              Width;
    double              Height;
    double              XDPI;
    double              YDPI;
    double              ZoomFactor;

    void SaveImage(void);
    void RenderProject(void);

    virtual void setVisible(bool visible);

private slots:
    void WidthChanged(int w);
    void HeightChanged(int h);
    void XResChanged(int xres);
    void YResChanged(int yres);
    void ZoomOut(void);
    void ZoomIn(void);
    void Zoom1t1(void);
    void ZoomFit(void);
    void CopyToClipboard(void);
    void ButtonBoxClicked(QAbstractButton* p_button);
};

//------------------------------------------------------------------------------

#endif
