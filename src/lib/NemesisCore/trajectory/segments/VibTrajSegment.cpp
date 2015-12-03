// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <Trajectory.hpp>
#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <Trajectory.hpp>
#include <NormalVibMode.hpp>
#include <Snapshot.hpp>
#include <Atom.hpp>

#include <VibTrajSegment.hpp>
#include "VibTrajSegment.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVibTrajSegment::CVibTrajSegment(CPluginObject* p_po,CTrajectory* p_traj,bool no_index)
    : CTrajectorySegment(p_po,p_traj,no_index)
{
    NumberOfSnapshots = 180;
    ModeAmplitude = 1.0;
    NumberOfModes = 0;
    Snapshot = new CSnapshot(this);
    Snapshot->InitSnapshot(false);

    // copy base positions
    int natoms = GetNumberOfAtoms();
    if( natoms > 0 ){
        Coordinates.CreateVector(natoms);
        foreach(QObject* p_qatom, GetStructure()->GetAtoms()->children() ){
            CAtom* p_atom = static_cast<CAtom*>(p_qatom);
            int idx = p_atom->GetTrajIndex();
            if( idx < 0 || idx >= natoms ) continue;
            Coordinates[idx] = p_atom->GetBasePos();
        }
    }
}

//------------------------------------------------------------------------------

CVibTrajSegment::~CVibTrajSegment(void)
{
    // destroy modes
    foreach(CNormalVibMode* p_mode, NormalModes){
        delete p_mode;
    }
    NormalModes.clear();

    // and snapshot
    if( Snapshot ){
        delete Snapshot;
        Snapshot = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

long int CVibTrajSegment::GetNumberOfSnapshots(void)
{
    return(NumberOfSnapshots);
}

//------------------------------------------------------------------------------

CSnapshot* CVibTrajSegment::GetCurrentSnapshot(void)
{
    return(Snapshot);
}

//------------------------------------------------------------------------------

void CVibTrajSegment::SetAmplitude(double amplitude)
{
    ModeAmplitude = amplitude;
}

//------------------------------------------------------------------------------

double CVibTrajSegment::GetAmplitude(void)
{
    return(ModeAmplitude);
}

//------------------------------------------------------------------------------

int CVibTrajSegment::GetNumberOfModes(void)
{
    return(NumberOfModes);
}

//------------------------------------------------------------------------------

void CVibTrajSegment::SnapshotChanged(void)
{
    if( Snapshot == NULL ) return;

    for(int i=0; i < GetNumberOfAtoms(); i++){
        Snapshot->Coordinates[i] = Coordinates[i];
    }

    if( SnapshotIndex == 0 ) return;
    if( GetNumberOfSnapshots() < 2 ) return;
    int nmodes = GetNumberOfModes();
    if( nmodes == 0 ) return;

    double angle = 2.0 * M_PI * (double)(SnapshotIndex-1) / (double)(GetNumberOfSnapshots()-1);

    // count number of active modes
    int active = 0;
    for(int i=0; i < nmodes; i++){
        CNormalVibMode* p_mode = NormalModes[i];
        if( p_mode->IsActive == true ) active++;
    }
    if( active == 0 ) return;   // no active vibrations

    // current amplitude
    double amplitude = ModeAmplitude*sin(angle);

    // add modes
    for(int i=0; i < nmodes; i++){
        CNormalVibMode* p_mode = NormalModes[i];
        if( p_mode->IsActive == false ) continue;
        for(int j=0; j < GetNumberOfAtoms(); j++){
            Snapshot->Coordinates[j] += p_mode->Mode[j]*amplitude;
        }
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVibTrajSegment::AllocateModes(int natoms)
{
    if( natoms != GetNumberOfAtoms() ){
        CSmallString error;
        error << "inconsistent number of atoms in the file '" << natoms
              << "' and in the trajectory '" << GetNumberOfAtoms() << "'";
        RUNTIME_ERROR(error);
    }

    NumberOfModes = 3*natoms;

    NormalModes.resize(NumberOfModes);
    for(int i=0; i < NumberOfModes; i++){
        CNormalVibMode* p_mode = new CNormalVibMode(natoms);
        NormalModes[i] = p_mode;
    }
}

//------------------------------------------------------------------------------

void CVibTrajSegment::UpdateNumberOfModes(int nmodes)
{
    if( nmodes == NumberOfModes ) return;
    if( nmodes > NumberOfModes ){
        NormalModes.resize(nmodes);
        for(int i=NumberOfModes; i < nmodes; i++){
            CNormalVibMode* p_mode = new CNormalVibMode(GetNumberOfAtoms());
            NormalModes[i] = p_mode;
        }
        NumberOfModes = nmodes;
        return;
    }

    for(int i=NumberOfModes-1; i >= nmodes; i--){
        delete NormalModes[i];
    }
    NormalModes.resize(nmodes);
    NumberOfModes = nmodes;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

