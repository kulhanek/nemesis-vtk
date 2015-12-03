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

#include <list>
#include <sstream>

#include <QFont>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <XMLElement.hpp>
#include <GlobalSetup.hpp>
#include <Project.hpp>
#include <ProObject.hpp>
#include <PluginDatabase.hpp>

#include <Graphics.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsObjectList.hpp>
#include <BoxObject.hpp>

#include <Structure.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <PeriodicTable.hpp>

#include <OpenBabelUtils.hpp>
#include <openbabel/obiter.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/builder.h>

#include "DockingProjectModule.hpp"
#include "DockingProject.hpp"
#include "DockingProjectInputVinaExportTool.hpp"
#include "DockingProjectInputVinaExportTool.moc"

using namespace OpenBabel;
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectInputVinaExportToolCB(void* p_data);

CExtUUID        DockingProjectInputVinaExportToolID(
                    "{DOCKING_PROJECT_INPUT_VINA_EXPORT_TOOL:7cce0ccc-0259-4669-9023-46de1d537f60}",
                    "Docking Input for Autodock Vina");

CPluginObject   DockingProjectInputVinaExportToolObject(&DockingProjectPlugin,
                    DockingProjectInputVinaExportToolID,GENERIC_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectInputVinaExportTool::CDockingProjectInputVinaExportTool(CWorkPanel* p_parent)
    : CProObject(&DockingProjectInputVinaExportToolObject,p_parent->GetProject(),p_parent->GetProject())

// : CExtComObject(&DockingProjectInputVinaExportToolObject,p_parent)
{
    HaveBox = false;
}

//------------------------------------------------------------------------------

CDockingProjectInputVinaExportTool::~CDockingProjectInputVinaExportTool(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QString CDockingProjectInputVinaExportTool::GetPath(void)
{
    return (Path);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetHadPath(void)
{
   return (HavePath);
}

//------------------------------------------------------------------------------

QString CDockingProjectInputVinaExportTool::GetConfig(void)
{
     return (Config);
}

//------------------------------------------------------------------------------

QString CDockingProjectInputVinaExportTool::GetLigandInFormat(void)
{
     return (LigandInFormat);
}

//------------------------------------------------------------------------------

QString CDockingProjectInputVinaExportTool::GetReceptorInFormat(void)
{
     return (ReceptorInFormat);
}

//------------------------------------------------------------------------------

CFileName CDockingProjectInputVinaExportTool::GetLigandFileName(void)
{
     return (LigandFileName);
}

//------------------------------------------------------------------------------

CFileName CDockingProjectInputVinaExportTool::GetReceptorFileName(void)
{
     return (ReceptorFileName);
}

//------------------------------------------------------------------------------

CFileName CDockingProjectInputVinaExportTool::GetConfigFileName(void)
{
     return (ConfigFileName);
}

//------------------------------------------------------------------------------

CFileName CDockingProjectInputVinaExportTool::GetOutputFileName(void)
{
     return (OutputFileName);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetBoxState(void)
{
     return (HaveBox);
}

//------------------------------------------------------------------------------

CPoint CDockingProjectInputVinaExportTool::GetBoxPosition(void)
{
     return (BoxPosition);
}

//------------------------------------------------------------------------------

CPoint CDockingProjectInputVinaExportTool::GetBoxDimension(void)
{
     return (BoxDimension);
}

//------------------------------------------------------------------------------

int CDockingProjectInputVinaExportTool::GetNumberOfModes(void)
{
     return (NumberOfModes);
}

//------------------------------------------------------------------------------

long int CDockingProjectInputVinaExportTool::GetSeed(void)
{
     return (Seed);
}

//------------------------------------------------------------------------------

long int CDockingProjectInputVinaExportTool::GetEnergyRange(void)
{
     return (EnergyRange);
}

//------------------------------------------------------------------------------

int CDockingProjectInputVinaExportTool::GetNumberOfProcessors(void)
{
     return (NumberOfProcessors);
}

//------------------------------------------------------------------------------

long int CDockingProjectInputVinaExportTool::GetExhaustiveness(void)
{
     return (Exhaustiveness);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetFlexibleLigand(void)
{
     return (FlexibleLigand);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetCombineLigand(void)
{
     return (CombineLigand);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetRenumberLigand(void)
{
     return (RenumberLigand);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetFlexibleReceptor(void)
{
     return (FlexibleReceptor);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetCombineReceptor(void)
{
     return (CombineReceptor);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetRenumberReceptor(void)
{
     return (RenumberReceptor);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetSaveLogOutput(void)
{
     return (SaveLogOutput);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::GetSaveConfigFile(void)
{
     return (SaveConfigFile);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputVinaExportTool::SetPath(QString p_str)
{
    Path = p_str;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetHadPath(bool have_path)
{
   HavePath = have_path;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetConfig(QString c_str)
{
    Config = c_str;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetLigandInFormat(QString l_str)
{
    LigandInFormat = l_str;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetReceptorInFormat(QString r_str)
{
    ReceptorInFormat = r_str;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetLigandFileName(CFileName file)
{
    LigandFileName = file;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetReceptorFileName(CFileName file)
{
    ReceptorFileName = file;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetConfigFileName(CFileName file)
{
    ConfigFileName = file;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetOutputFileName(CFileName file)
{
    OutputFileName = file;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetBoxPosition(CPoint box_pos)
{
    BoxPosition = box_pos;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetBoxDimension(CPoint box_dim)
{
     BoxDimension = box_dim;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetNumberOfModes(int n_modes)
{
     NumberOfModes = n_modes;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetSeed(long int seed)
{
     Seed = seed;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetEnergyRange(long int e_range)
{
     EnergyRange = e_range;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetNumberOfProcessors(int n_proc)
{
     NumberOfProcessors = n_proc;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetExhaustiveness(long int exhaus)
{
     Exhaustiveness = exhaus;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetFlexibleLigand(bool flex)
{
     FlexibleLigand = flex;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetCombineLigand(bool comb)
{
     CombineLigand = comb;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetRenumberLigand(bool renum)
{
     RenumberLigand = renum;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetFlexibleReceptor(bool flex)
{
     FlexibleReceptor = flex;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetCombineReceptor(bool comb)
{
     CombineReceptor = comb;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetRenumberReceptor(bool renum)
{
     RenumberReceptor = renum;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetSaveLogOutput(bool log)
{
     SaveLogOutput = log;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetSaveConfigFile(bool config)
{
     SaveConfigFile = config;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetWriteLigand(bool write)
{
     WriteLigand = write;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetWriteReceptor(bool write)
{
    WriteReceptor = write;
}

//------------------------------------------------------------------------------

void CDockingProjectInputVinaExportTool::SetWriteConfig(bool write)
{
    WriteConfig = write;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QString CDockingProjectInputVinaExportTool::GenerateConfigText(void)
{
    QString buffer;
    QTextStream text(&buffer);

    // Generate config file from set in widget window
    text << "ligand = " << LigandFileName << endl
         << "receptor = " << ReceptorFileName << endl
         << endl
         << "out = " << OutputFileName << endl
         << endl
         << "center_x = " << BoxPosition.x << endl
         << "center_y = " << BoxPosition.y << endl
         << "center_z = " << BoxPosition.z << endl
         << endl
         << "size_x = " << BoxDimension.x << endl
         << "size_y = " << BoxDimension.y << endl
         << "size_z = " << BoxDimension.z << endl
         << endl
         << "num_modes = " << NumberOfModes << endl;

         if (Seed != 0)
         {
            text << "seed = " << Seed << endl;
         }

    text << "energy_range = " << EnergyRange << endl
         << "cpu = " << NumberOfProcessors << endl
         << "exhaustiveness = " << Exhaustiveness << endl;

    return(buffer);

}
//------------------------------------------------------------------------------

QString CDockingProjectInputVinaExportTool::GenerateCoordinatesInPdbqt(CStructure* structure, bool is_ligand)
{
    if( (structure == NULL) || (structure->IsEmpty()) ) return QString();

    QString buffer;
    QTextStream str(&buffer);

    OBMol mol;
    ostringstream converted;
    OpenBabel::OBConversion conv(NULL,&converted);

    // convert nemesis structure data into openbabel
    COpenBabelUtils::Nemesis2OpenBabel(structure,mol);

//    "AutoDock PDQBT format\n"
//    "Reads and writes AutoDock PDBQT (Protein Data Bank, Partial Charge (Q), & Atom Type (T)) format\n"
//    "Note that the torsion tree is by default. Use the ``r`` write option\n"
//    "to prevent this.\n\n"

//    "Read Options, e.g. -ab\n"
//    "  b  Disable automatic bonding\n"
//    "  d  Input file is in dlg (AutoDock docking log) format\n\n"

//    "Write Options, e.g. -xr\n"
//    "  b  Enable automatic bonding\n"
//    "  r  Output as a rigid molecule (i.e. no branches or torsion tree)\n"
//    "  c  Combine separate molecular pieces of input into a single rigid molecule (requires \"r\" option or will have no effect)\n"
//    "  s  Output as a flexible residue\n"
//    "  p  Preserve atom indices from input file (default is to renumber atoms sequentially)\n\n";

    // b - Disable automatic bonding
    conv.AddOption("b",OBConversion::INOPTIONS);
    bool flexible, combine, renumber;
    if (is_ligand)
    {
        flexible = FlexibleLigand;
        combine = CombineLigand;
        renumber = RenumberLigand;
    } else {
        flexible = FlexibleReceptor;
        combine = CombineReceptor;
        renumber = RenumberReceptor;
    }
    if (!flexible) {
        // r - Output as a rigid molecule (i.e. no branches or torsion tree)
        conv.AddOption("r",OBConversion::OUTOPTIONS);
    }

    if (combine && (!flexible) ) {
        // c  Combine separate molecular pieces of input into a single rigid molecule (requires "r" option or will have no effect)
        conv.AddOption("c",OBConversion::OUTOPTIONS);
    }

    if (!renumber) {
        // p - Preserve atom indices from input file (default is to renumber atoms sequentially)
        conv.AddOption("p",OBConversion::OUTOPTIONS);
    }

    // output format setting
    if (!conv.SetOutFormat("PDBQT")) {
        ES_ERROR("Cannot set format");
        return QString();
    }

    // write data
    conv.Write(&mol);

    string stroutput = converted.str();
    str << stroutput.c_str();

    str << '\n';

    if (!FlexibleReceptor && !is_ligand) {
        RemoveUnwantedLines( &buffer );
    }

    return buffer;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectInputVinaExportTool::SaveInputFiles(void)
{
    if (!HavePath) {
        // dialog for open the directory and set the path
        QFileDialog* p_dialog = new QFileDialog();
        p_dialog->setDirectory (QString(GlobalSetup->GetLastOpenFilePath(DockingProjectInputVinaExportToolID)));
        p_dialog->setFileMode(QFileDialog::DirectoryOnly);
        p_dialog->setAcceptMode(QFileDialog::AcceptOpen);

        // dialog window will be open
        if( p_dialog->exec() == QDialog::Rejected ){
            return (false);
        }

        Path = p_dialog->selectedFiles().at(0);
        // set the last path
        GlobalSetup->SetLastOpenFilePathFromFile(Path,DockingProjectInputVinaExportToolID);

        delete p_dialog;
    }

    // get whole path od file
    QString pathFileNameConfig = Path + "/" + ConfigFileName;
    QString pathFileNameLigand = Path  + "/" + LigandFileName;
    QString pathFileNameReceptor = Path + "/" + ReceptorFileName;

    if (WriteConfig) {
        // save of config file for Autodock vina
        QFile fileCfg(pathFileNameConfig);
        // open files
        if( ! fileCfg.open(QIODevice::WriteOnly | QIODevice::Text) ){
            QMessageBox::critical(NULL,"Error","Unable to open file!",QMessageBox::Ok,QMessageBox::Ok);
            return (false);
        }
        // write to files
        QTextStream outC(&fileCfg);
        outC << Config;
        fileCfg.close();
    }

    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(GetProject());

    // handle save of ligand file in PDBQT format
    if (!LigandFileName.IsEmpty() && WriteLigand) {
        QFile fileLgn(pathFileNameLigand);
        // open files
        if( ! fileLgn.open(QIODevice::WriteOnly | QIODevice::Text) ){
            QMessageBox::critical(NULL,"Error","Unable to open file!",QMessageBox::Ok,QMessageBox::Ok);
            return (false);
        }

        // if is settings changed we must regenerate with new settings
        if( !AreLigandCheckBoxesDefaults() ){
            if(p_docproj != NULL) {
                if ((p_docproj->GetLigandStructure() != NULL) && (!p_docproj->GetLigandStructure()->IsEmpty()) )
                    LigandInFormat = GenerateCoordinatesInPdbqt(p_docproj->GetLigandStructure(),true);
            }
        }
        // write to files
        QTextStream outL(&fileLgn);
        outL << LigandInFormat;
        fileLgn.close();
    }

    // handle save of receptor file in PDBQT format
    if (!ReceptorFileName.IsEmpty() && WriteReceptor) {
        QFile fileRcp(pathFileNameReceptor);
        // open files
        if( ! fileRcp.open(QIODevice::WriteOnly | QIODevice::Text) ){
            QMessageBox::critical(NULL,"Error","Unable to open file!",QMessageBox::Ok,QMessageBox::Ok);
            return (false);
        }
        // if was settings changed we must regenerate with new settings
        if ( !AreReceptorCheckBoxesDefaults() ) {
            if(p_docproj != NULL) {
                if( (p_docproj->GetReceptorStructure() != NULL) && (!p_docproj->GetReceptorStructure()->IsEmpty())) {
                    ReceptorInFormat = GenerateCoordinatesInPdbqt(p_docproj->GetReceptorStructure(),false);
                }
            }
        }
        // write to files
        QTextStream outR(&fileRcp);
        outR << ReceptorInFormat;
        fileRcp.close();
    }
    return (true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectInputVinaExportTool::AreLigandCheckBoxesDefaults(void)
{
    return (FlexibleLigand && !CombineLigand && RenumberLigand);
}

//------------------------------------------------------------------------------

bool CDockingProjectInputVinaExportTool::AreReceptorCheckBoxesDefaults(void)
{
    return (!FlexibleReceptor && CombineReceptor && !RenumberReceptor);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBoxObject* CDockingProjectInputVinaExportTool::GetBox(void)
{
    // Get graphics of current object
    CGraphics* p_grph = GetProject()->GetGraphics();
    if( p_grph == NULL ) return(NULL);

    // Search in graphics object and find box object
    ///TODO What if i have two boxes?
    foreach(QObject* p_qobj,p_grph->GetObjects()->children()) {
        CGraphicsObject* p_gobj = static_cast<CGraphicsObject*>(p_qobj);
        // Are you box?
        CBoxObject* p_box = dynamic_cast<CBoxObject*>(p_gobj);
        if( p_box != NULL ) {
            HaveBox = true;
            // If i have box and if i want only write config file i can
            return (p_box);
        }

    }
    return (NULL);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CDockingProjectInputVinaExportTool::RemoveUnwantedLines(QString* p_receptor)
{
    // method for remove unwanted symbols form OpenBabel which vina cant get

    // pdbqt for vina cannot contain ROOT flag, ENDROOT flag and (END)TORSDORF num flag.
    p_receptor->remove(QRegExp("ROOT."));
    p_receptor->remove(QRegExp("ENDROOT."));
    p_receptor->remove(QRegExp("TORSDORF.[0-9]*"));
    p_receptor->remove(QRegExp("ENDTORSDOF.[0-9]*"));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


