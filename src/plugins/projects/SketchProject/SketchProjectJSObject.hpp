#ifndef SketchProjectJSObjectH
#define SketchProjectJSObjectH
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

#include <QObject>

//------------------------------------------------------------------------------

class CSketchProjectWindow;

//------------------------------------------------------------------------------

class CSketchProjectJSObject : public QObject {
Q_OBJECT
public:
// constructor and destructor --------------------------------------------------
    CSketchProjectJSObject(CSketchProjectWindow* p_owner);

public:
    /// get initial structure from project
    void getInitialStructure(void);

    /// update smiles data - synchronously
    void updateSMILESData(void);

    /// update SVG data - synchronously
    void updateSVGData(void);

// properties ------------------------------------------------------------------
public:
    Q_PROPERTY(QString smilesData READ getSMILESData WRITE setSMILESData NOTIFY onSetSMILEData)
    Q_PROPERTY(QString svgData READ getSVGData WRITE setSVGData NOTIFY onSetSVGData)

// environment exposed to ketcher javascript -----------------------------------
public slots:
    /// open new project dialog
    void newProject(void);

    /// show open project dialog
    void openProject(void);

    /// show save project dialog
    void saveProject(void);

    /// clean project structure
    const QString cleanProject(const QString& inmol);

    /// update menu
    void updateMenu(const QString& changed);

    /// save data
    const QString saveData(void);

    /// set/get data
    void setSMILESData(const QString& data);
    const QString& getSMILESData(void);

    /// set svg data
    void setSVGData(const QString& data);
    const QString& getSVGData(void);

    /// update ui
    void setUndoEnabled(bool set);
    void setRedoEnabled(bool set);
    void setCutEnabled(bool set);
    void setCopyEnabled(bool set);
    void setPasteEnabled(bool set);
    void setZoomInEnabled(bool set);
    void setZoomOutEnabled(bool set); 

// signals ---------------------------------------------------------------------
signals:
    void onUpdateSMILESData(void);
    void onUpdateSVGData(void);
    void onMenuUpdate(void);
    void onSetSMILEData(void);
    void onSetSVGData(void);

// section of private data -----------------------------------------------------
private:
    CSketchProjectWindow*   ProjectWindow;
    QString                 SMILESData;
    QString                 SVGData;
    int                     DataVersion;    // reflect asynchronous update
};

//------------------------------------------------------------------------------

#endif
