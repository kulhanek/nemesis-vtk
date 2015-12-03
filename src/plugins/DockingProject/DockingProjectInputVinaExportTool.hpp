#ifndef DockingProjectInputVinaExportTool_H
#define DockingProjectInputVinaExportTool_H
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

#include <ExtComObject.hpp>
#include <ProObject.hpp>
#include <WorkPanel.hpp>
#include "DockingProject.hpp"

#include <Atom.hpp>

//------------------------------------------------------------------------------

class CStructure;
class CBoxObject;

//------------------------------------------------------------------------------

extern CExtUUID DockingProjectInputVinaExportToolID;

//------------------------------------------------------------------------------
class CDockingProjectInputVinaExportTool: public CProObject { //: public CExtComObject {
Q_OBJECT
public:
    CDockingProjectInputVinaExportTool(CWorkPanel *p_parent);
    ~CDockingProjectInputVinaExportTool(void);

    // information methods ---------------------------------------------------------
    QString     GetPath(void);
    bool        GetHadPath(void);
    QString     GetConfig(void);
    QString     GetLigandInFormat(void);
    QString     GetReceptorInFormat(void);

    CFileName   GetLigandFileName(void);
    CFileName   GetReceptorFileName(void);
    CFileName   GetConfigFileName(void);
    CFileName   GetOutputFileName(void);

    bool        GetBoxState(void);
    CPoint      GetBoxPosition(void);
    CPoint      GetBoxDimension(void);

    int         GetNumberOfModes(void);
    long int    GetSeed(void);
    long int    GetEnergyRange(void);
    int         GetNumberOfProcessors(void);
    long int    GetExhaustiveness(void);

    bool        GetFlexibleLigand(void);
    bool        GetCombineLigand(void);
    bool        GetRenumberLigand(void);
    bool        GetFlexibleReceptor(void);
    bool        GetCombineReceptor(void);
    bool        GetRenumberReceptor(void);

    bool        GetSaveLogOutput(void);
    bool        GetSaveConfigFile(void);

    // methods execute changes --------------------------
    /// Set path where will be compute docking
    void        SetPath(QString p_str);
    /// Set information about having path for dock
    void        SetHadPath(bool have_path);

    /// Name of config file for docking algorithm
    void        SetConfig(QString c_str);
    /// Ligand structure in right format for docking
    void        SetLigandInFormat(QString l_str);
    /// Receptor structure in right format for docking
    void        SetReceptorInFormat(QString r_str);

    /// Ligand file name which will be save for docking
    void        SetLigandFileName(CFileName file);
    /// Receptor file name which will be save for docking
    void        SetReceptorFileName(CFileName file);
    /// Config file name for docking algorithm which will be save for docking
    void        SetConfigFileName(CFileName file);
    /// Output file name in which will be saved dock result
    void        SetOutputFileName(CFileName file);

    /// Box position
    void        SetBoxPosition(CPoint box_pos);
    /// Box dimension
    void        SetBoxDimension(CPoint box_dim);

    /// Parameters of docking
    void        SetNumberOfModes(int n_modes);
    void        SetSeed(long int seed);
    void        SetEnergyRange(long int e_range);
    void        SetNumberOfProcessors(int n_proc);
    void        SetExhaustiveness(long int exhaus);

    /// Parameters of conversion to th format dor docking
    void        SetFlexibleLigand(bool flex);
    void        SetCombineLigand(bool comb);
    void        SetRenumberLigand(bool renum);
    void        SetFlexibleReceptor(bool flex);
    void        SetCombineReceptor(bool comb);
    void        SetRenumberReceptor(bool renum);

    /// Information if has to be saved log of docking
    void        SetSaveLogOutput(bool log);
    /// Information if config file may stay after docking
    void        SetSaveConfigFile(bool config);

    /// Information about which files will be saved
    void        SetWriteLigand(bool write);
    void        SetWriteReceptor(bool write);
    void        SetWriteConfig(bool write);


//------------------------------------------------------------------------------
private:
    QString     Path;
    bool        HavePath;

    QString     Config;
    QString     LigandInFormat;
    QString     ReceptorInFormat;

    CFileName   LigandFileName;
    CFileName   ReceptorFileName;
    CFileName   ConfigFileName;
    CFileName   OutputFileName;

    bool        HaveBox;
    CPoint      BoxPosition;
    CPoint      BoxDimension;

    int         NumberOfModes;
    long int    Seed;
    long int    EnergyRange;
    int         NumberOfProcessors;
    long int    Exhaustiveness;

    bool        FlexibleLigand;
    bool        CombineLigand;
    bool        RenumberLigand;

    bool        FlexibleReceptor;
    bool        CombineReceptor;
    bool        RenumberReceptor;

    bool        SaveLogOutput;
    bool        SaveConfigFile;

    bool        WriteLigand;
    bool        WriteReceptor;
    bool        WriteConfig;

public:
    /// Generate config file for Autodock Vina
    QString     GenerateConfigText(void);
    /// Generate right PDBQT format for Autodock Vina from structure
    QString     GenerateCoordinatesInPdbqt(CStructure* structure, bool is_ligand);

    /// Information about change in values
    bool        AreLigandCheckBoxesDefaults(void);
    bool        AreReceptorCheckBoxesDefaults(void);

    /// Method handle with unwanted lines from OpenBabel
    void        RemoveUnwantedLines(QString* p_receptor);

    /// Find and set Box
    CBoxObject* GetBox(void);
    /// Save files for run dock
    bool        SaveInputFiles(void);

    friend class CDockingProjectDockPanel;
};

//------------------------------------------------------------------------------

#endif
