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

// context commands ============================================================

#include <context/Alias.hpp>
#include <context/Help.hpp>
#include <context/History.hpp>
#include <context/LoadState.hpp>
#include <context/LogFile.hpp>
#include <context/NLeap.hpp>
#include <context/Redo.hpp>
#include <context/SaveState.hpp>
#include <context/Source.hpp>
#include <context/Undo.hpp>

nleapcmds::CAliasCommand            g_alias_command( "alias" );
nleapcmds::CHelpCommand             g_help_command( "help" );
nleapcmds::CHistoryCommand          g_history_command( "history" );
nleapcmds::CLoadStateCommand        g_loadstate_command( "loadState" );
nleapcmds::CLogFileCommand          g_logfile_command( "logFile" );
nleapcmds::CNLeapCommand            g_nleap_command( "nleap" );
nleapcmds::CRedoCommand             g_redo_command( "redo" );
nleapcmds::CSaveStateCommand        g_savestate_command( "saveState" );
nleapcmds::CSourceCommand           g_source_command( "source" );
nleapcmds::CUndoCommand             g_undo_command( "undo" );

// control commands ============================================================

#include <control/Add.hpp>
#include <control/AddAtomTypes.hpp>
#include <control/AddPDBAtomMap.hpp>
#include <control/AddPDBResMap.hpp>
#include <control/AmberFF.hpp>
#include <control/Assign.hpp>
#include <control/BondByDistance.hpp>
#include <control/Bond.hpp>
#include <control/Check.hpp>
#include <control/Clear.hpp>
#include <control/Combine.hpp>
#include <control/Copy.hpp>
#include <control/CreateAtom.hpp>
#include <control/CreateResidue.hpp>
#include <control/CreateUnit.hpp>
#include <control/DeleteBond.hpp>
#include <control/Desc.hpp>
#include <control/Release.hpp>
#include <control/List.hpp>
#include <control/Remove.hpp>
#include <control/Sequence.hpp>
#include <control/Set.hpp>

nleapcmds::CAddCommand                  g_add_command( "add" );
nleapcmds::CAddAtomTypesCommand         g_addatomtypes_command( "addAtomTypes" );
nleapcmds::CAddPDBAtomMapCommand        g_addpdbatommap_command( "addPdbAtomMap" );
nleapcmds::CAddPDBResMapCommand         g_addpdbresmap_command( "addPdbResMap" );
nleapcmds::CAmberFFCommand              g_amberff_command( "amberFF" );
nleapcmds::CAssignCommand               g_assign_command( "assign" );
nleapcmds::CBondCommand                 g_bond_command( "bond" );
nleapcmds::CBondByDistanceCommand       g_bond_by_distance_command( "bondByDistance" );
nleapcmds::CCheckCommand                g_check_command( "check" );
nleapcmds::CClearCommand                g_clear_command( "clear" );
nleapcmds::CCombineCommand              g_combine_command( "combine" );
nleapcmds::CCopyCommand                 g_copy_command( "copy" );
nleapcmds::CCreateAtomCommand           g_createAtom_command( "createAtom" );
nleapcmds::CCreateResidueCommand        g_createResidue_command(  "createResidue" );
nleapcmds::CCreateUnitCommand           g_createUnit_command( "createUnit" );
nleapcmds::CDeleteBondCommand           g_delete_bond_command( "deleteBond" );
nleapcmds::CDescCommand                 g_desc_command( "desc" );
nleapcmds::CReleaseCommand              g_destroy_command( "release" );
nleapcmds::CListCommand                 g_list_command( "list" );
nleapcmds::CRemoveCommand               g_remove_command( "remove" );
nleapcmds::CSequenceCommand             g_sequence_command( "sequence" );
nleapcmds::CSetCommand                  g_set_command( "set" );

// input commands ==============================================================
#include <input/LoadAmberParams.hpp>
#include <input/LoadAmberPrep.hpp>
#include <input/LoadMol2.hpp>
#include <input/LoadOFF.hpp>
#include <input/LoadPDB.hpp>

nleapcmds::CLoadAmberParamsCommand  g_loadamberparams_command( "loadAmberParams" );
nleapcmds::CLoadAmberPrepCommand    g_loadamberprep_command( "loadAmberPrep" );
nleapcmds::CLoadMol2Command         g_loadmol2_command( "loadMol2" );

nleapcmds::CLoadOFFCommand          g_loadoff_command( "loadOff" );
nleapcmds::CLoadPDBCommand          g_loadpdb_command( "loadPdb" );

// openbabel commands ==========================================================
#include <openbabel/ListOBFormats.hpp>
#include <openbabel/LoadOB.hpp>
#include <openbabel/SaveOB.hpp>

nleapcmds::CListOBFormatsCommand    g_listobformats_command( "listOBFormats" );
nleapcmds::CLoadOBCommand           g_loadob_command( "loadOB" );
nleapcmds::CSaveOBCommand           g_saveob_command( "saveOB" );

// output commands =============================================================

#include <output/SaveAmberParams.hpp>
#include <output/SaveAmberParm.hpp>
#include <output/SaveGaussian.hpp>
#include <output/SaveGromacs.hpp>
#include <output/SaveGromos.hpp>
#include <output/SaveMol2.hpp>
#include <output/SavePDB.hpp>

nleapcmds::CSaveAmberParamsCommand      g_saveamberparams_command( "saveAmberParams" );
nleapcmds::CSaveAmberParmCommand        g_saveamberparm_command( "saveAmberParm" );
nleapcmds::CSaveGaussianCommand         g_savegaussian_command( "saveGaussian" );
nleapcmds::CSaveGromacsCommand          g_savegromacs_command( "saveGromacs" );
nleapcmds::CSaveGromosCommand           g_savegromos_command( "saveGromos" );
nleapcmds::CSaveMol2Command             g_savemol2_command( "saveMol2" );
nleapcmds::CSavePDBCommand              g_savepdb_command( "savePdb" );

// geometry commands ===========================================================
#include <geometry/AlignAxes.hpp>
#include <geometry/Center.hpp>
#include <geometry/MeasureGeom.hpp>
#include <geometry/Translate.hpp>

nleapcmds::CAlignAxesCommand        g_alignaxes_command( "alignAxes" );
nleapcmds::CCenterCommand           g_center_command( "center" );
nleapcmds::CMeasureGeomCommand      g_measuregeom_command( "measureGeom" );
nleapcmds::CTranslateCommand        g_translate_command( "translate" );

// solvent commands ============================================================
//#include <shell.hpp>
//#include <solvate.hpp>
//#include <addions.hpp>

////amber::shell_command        g_shell_command;
////amber::solvate_command      g_solvatebox_command( "box" );
////amber::solvate_command      g_solvateoct_command( "oct" );
////amber::solvate_command      g_solvatecap_command( "cap" );
////amber::solvate_command      g_solvateshl_command( "shell" );
////amber::addions_command      g_addions_command;

// property commands ===========================================================

#include <properties/Charge.hpp>
#include <properties/Energy.hpp>

nleapcmds::CEnergyCommand       g_energy_command("energy");
nleapcmds::CChargeCommand       g_charge_command("charge");

// antechamber commands ========================================================

//#include <moloper.hpp>
////amber::moloper_command      g_fixbond_command( "fixbond" );
////amber::moloper_command      g_addhydr_command( "addhydr" );
////amber::moloper_command      g_setpchg_command( "setpchg" );
////amber::moloper_command      g_parmchk_command( "parmchk" );

// openbabel commands ==========================================================































