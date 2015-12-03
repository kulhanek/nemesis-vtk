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

#include <RenderWindow.hpp>
#include "RenderWindow.moc"

#include <ProjectList.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsViewList.hpp>
#include <QClipboard>
#include <GlobalSetup.hpp>
#include <QMessageBox>
#include <QFileDialog>

//------------------------------------------------------------------------------

CExtUUID        SaveImageModuleID(
                    "{SAVE_IMAGE_MODULE:ab179c0f-f7e9-48cf-8ff0-67c6c051d923}",
                    "Image");

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CRenderWindow::CRenderWindow(CProject* p_project)
    : QDialog(NULL)
{
    Project = p_project;

    // init ui part
    WidgetUI.setupUi(this);

    // fix layout
    LabelPicture = new QLabel(this);
    WidgetUI.scrollArea->setWidget(LabelPicture);

    // initial setup
    setWindowTitle(tr("Render project - ") + Project->GetName());

    Width = Project->GetGraphics()->GetViews()->GetPrimaryView()->GetWidth();
    Height = Project->GetGraphics()->GetViews()->GetPrimaryView()->GetHeight();
    WidgetUI.widthSB->setValue(Width);
    WidgetUI.heightSB->setValue(Height);
    WidgetUI.unitSizeCB->addItem("points");

    XDPI = 150;
    YDPI = 150;
    WidgetUI.xResSB->setValue(XDPI);
    WidgetUI.yResSB->setValue(YDPI);
    WidgetUI.unitResolutionCB->addItem("points/inch");

    ZoomFactor = 1.0;

    RenderProject();

    // signals
    connect(WidgetUI.widthSB,SIGNAL(valueChanged(int)),
            this,SLOT(WidthChanged(int)));
    connect(WidgetUI.heightSB,SIGNAL(valueChanged(int)),
            this,SLOT(HeightChanged(int)));

    connect(WidgetUI.xResSB,SIGNAL(valueChanged(int)),
            this,SLOT(XResChanged(int)));
    connect(WidgetUI.yResSB,SIGNAL(valueChanged(int)),
            this,SLOT(YResChanged(int)));

    connect(WidgetUI.zoomOutTB,SIGNAL(clicked(bool)),
            this,SLOT(ZoomOut(void)));
    connect(WidgetUI.zoomInTB,SIGNAL(clicked(bool)),
            this,SLOT(ZoomIn(void)));
    connect(WidgetUI.zoom1t1TB,SIGNAL(clicked(bool)),
            this,SLOT(Zoom1t1(void)));
    connect(WidgetUI.zoomFitTB,SIGNAL(clicked(bool)),
            this,SLOT(ZoomFit(void)));
    connect(WidgetUI.copyTB,SIGNAL(clicked(bool)),
            this,SLOT(CopyToClipboard(void)));

    connect(WidgetUI.buttonBox,SIGNAL(clicked(QAbstractButton*)),
            this,SLOT(ButtonBoxClicked(QAbstractButton*)));
}

//------------------------------------------------------------------------------

CRenderWindow::~CRenderWindow(void)
{

}

//------------------------------------------------------------------------------

void CRenderWindow::setVisible(bool visible)
{
    QDialog::setVisible(visible);
    // scroll area dimmension is known when dialog appears for first time
    ZoomFit();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRenderWindow::RenderProject(void)
{
    QString tmp = tr("Zoom: %1%    Size: %2 x %3 cm");
    tmp = tmp.arg(ZoomFactor*100.0,0,'f',0);
    tmp = tmp.arg(Width/(double)XDPI*2.54,0,'f',2);
    tmp = tmp.arg(Height/(double)YDPI*2.54,0,'f',2);

    WidgetUI.notifyLabel->setText(tmp);

    SceneImage = Project->GetGraphics()->GetViews()->GetPrimaryView()->Render(Width,Height);
    QPixmap pixmap = QPixmap::fromImage(SceneImage).scaled(QSize(Width*ZoomFactor,Height*ZoomFactor));

    LabelPicture->setPixmap(pixmap);
    LabelPicture->adjustSize();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRenderWindow::WidthChanged(int w)
{
    if( WidgetUI.sizeLockTB->isChecked() ) {
        WidgetUI.heightSB->blockSignals(true);
        Height = w / Width * Height;
        WidgetUI.heightSB->setValue(Height);
        WidgetUI.heightSB->blockSignals(false);
    }

    Width = w;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::HeightChanged(int h)
{  
    if( WidgetUI.sizeLockTB->isChecked() ) {
        WidgetUI.widthSB->blockSignals(true);
        Width = h / Height * Width;
        WidgetUI.widthSB->setValue(Width);
        WidgetUI.widthSB->blockSignals(false);
    }
    Height = h;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::XResChanged(int xres)
{
    // int diff = xres - XDPI;
    if( WidgetUI.resLockTB->isChecked() ) {
        WidgetUI.yResSB->blockSignals(true);
        // Difference deforms picture, use ratio
        // YDPI += diff;
        YDPI = xres / XDPI * YDPI;
        WidgetUI.yResSB->setValue(YDPI);
        WidgetUI.yResSB->blockSignals(false);
    }
    XDPI = xres;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::YResChanged(int yres)
{
    //int diff = yres - YDPI;
    if( WidgetUI.resLockTB->isChecked() ) {
        WidgetUI.xResSB->blockSignals(true);
        // Difference deforms picture, use ratio
        XDPI = yres / YDPI * XDPI;
        // XDPI += diff;
        WidgetUI.xResSB->setValue(XDPI);
        WidgetUI.xResSB->blockSignals(false);
    }
    YDPI = yres;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::ZoomOut(void)
{
    ZoomFactor *= 0.75;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::ZoomIn(void)
{
    ZoomFactor *= 1.25;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::Zoom1t1(void)
{
    ZoomFactor = 1.0;
    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::ZoomFit(void)
{
    double z1 = (WidgetUI.scrollArea->width() - 20) / (double)Width;
    double z2 = (WidgetUI.scrollArea->height() - 20) / (double)Height;

    ZoomFactor = z2;
    if( z2 > z1 ) ZoomFactor = z1;

    RenderProject();
}

//------------------------------------------------------------------------------

void CRenderWindow::CopyToClipboard(void)
{
    QClipboard* p_clipboard = QApplication::clipboard();

    QPixmap pixmap = QPixmap::fromImage(SceneImage);
    p_clipboard->setPixmap(pixmap);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CRenderWindow::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Cancel ) {
        close();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Save ) {
        SaveImage();
        return;
    }
}

//------------------------------------------------------------------------------

void CRenderWindow::SaveImage(void)
{
    QString outname = QFileDialog::getSaveFileName(this,
                      tr("Save project image"),
                      QString(GlobalSetup->GetLastOpenFilePath(SaveImageModuleID)),
                      "image files - (*.bmp *.jpg *.jpeg *.png)");
    if( outname == NULL ) return;

    // update last open path
    GlobalSetup->SetLastOpenFilePathFromFile(outname,SaveImageModuleID);

    if( ! ( outname.endsWith("bmp") ||
            outname.endsWith("jpg") ||
            outname.endsWith("jpeg") ||
            outname.endsWith("png") ) ) {
        outname += ".png";  // set default extension png
    }

    SceneImage.setDotsPerMeterX(XDPI*100.0/2.54);
    SceneImage.setDotsPerMeterY(YDPI*100.0/2.54);

    if( SceneImage.save(outname) == false ) {
        QMessageBox::critical(NULL, tr("Error"),
                              tr("Unable to save specified image!"),
                              QMessageBox::Ok,
                              QMessageBox::Ok);
        return;
    }

    close();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
