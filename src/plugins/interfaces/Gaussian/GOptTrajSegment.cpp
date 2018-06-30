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
#include <fstream>
#include <Structure.hpp>
#include <AtomList.hpp>
#include <PhysicalQuantity.hpp>
#include <PhysicalQuantities.hpp>
#include <qcustomplot.h>

#include "GaussianModule.hpp"
#include "GaussianUtils.hpp"
#include "GOptTrajSegment.hpp"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

QObject* GOptTrajSegmentCB(void* p_data);

CExtUUID        GOptTrajSegmentID(
                    "{GOPT_TRAJ_SEGMENT:d40ec150-1757-4dff-8f74-a053460bf9fa}",
                    "Gaussian Geometry Optimization");

CPluginObject   GOptTrajSegmentObject(&GaussianPlugin,
                    GOptTrajSegmentID,TRAJECTORY_SEGMENT_CAT,
                    ":/images/Gaussian/GOptTrajSegment.svg",
                    GOptTrajSegmentCB);

// -----------------------------------------------------------------------------

QObject* GOptTrajSegmentCB(void* p_data)
{
    CTrajectory* p_traj = static_cast<CTrajectory*>(p_data);
    if( p_traj == NULL ){
        ES_ERROR("CGOptTrajSegment requires active trajectory");
        return(NULL);
    }

    QObject* p_build_wp = new CGOptTrajSegment(p_traj);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGOptTrajSegment::CGOptTrajSegment(CTrajectory* p_traj)
    : CTrajectorySegment(&GOptTrajSegmentObject,p_traj,true)
{
    EnergyKey = GetProject()->GetFreeObjectIndex();
    Model = NULL;

    // response to energy unit changes
    connect(PQ_ENERGY,SIGNAL(OnUnitChanged(void)),
            this,SLOT(CreateModel(void)));

    connect(PQ_ABSOLUTE_ENERGY,SIGNAL(OnUnitChanged(void)),
            this,SLOT(CreateModel(void)));
}

//------------------------------------------------------------------------------

CGOptTrajSegment::~CGOptTrajSegment(void)
{
    foreach(CSnapshot* p_snap, Snapshots){
        delete p_snap;
    }
    Snapshots.clear();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

long int CGOptTrajSegment::GetNumberOfSnapshots(void)
{
    return(Snapshots.count());
}

//------------------------------------------------------------------------------

CSnapshot* CGOptTrajSegment::GetCurrentSnapshot(void)
{
    if( (SnapshotIndex < 1) || (SnapshotIndex > Snapshots.count()) ) return(NULL);
    return(Snapshots.at(SnapshotIndex-1));
}

//------------------------------------------------------------------------------

QAbstractItemModel* CGOptTrajSegment::GetEnergyModel(void)
{
    if( Model == NULL ){
        CreateModel();
    }
    return(Model);
}

//------------------------------------------------------------------------------

const QString& CGOptTrajSegment::GetMethod(void) const
{
    return(Method);
}

//------------------------------------------------------------------------------

double CGOptTrajSegment::GetFinalEnergy(void) const
{
    if( Snapshots.count() == 0 ) return(0.0);
    CSnapshot* p_snap = Snapshots.last();
    return(p_snap->GetProperty(EnergyKey));
}

//------------------------------------------------------------------------------

void CGOptTrajSegment::CreateModel(void)
{
    if( Model == NULL ){
        Model = new QStandardItemModel(this);
    } else {
        Model->clear();
    }

    QStringList header;
    QString rel_energy_hdr = QString(tr("Relative Energy [%1]")).arg(PQ_ENERGY->GetUnitName());

    header << tr("ID") << tr("Energy [a.u.]") << rel_energy_hdr;
    Model->setHorizontalHeaderLabels(header);

    // get first energy
    double first_energy = 0;
    if( Snapshots.count() > 0 ){
        first_energy = Snapshots.first()->GetProperty(EnergyKey);
    }

    // load items
    int i = 0;
    foreach(CSnapshot* p_snap,Snapshots){

        double energy = p_snap->GetProperty(EnergyKey);
        double rel_energy = energy - first_energy;
        // convert to kcal/mol
        rel_energy *= 627.509608;

        QStandardItem*          p_item;
        QList<QStandardItem*>   items;

        // snapshot ID
        p_item = new QStandardItem(QString("%1").arg(i+1));
        p_item->setTextAlignment(Qt::AlignLeft);
        items << p_item;

        // absolute energy
        p_item = new QStandardItem(PQ_ABSOLUTE_ENERGY->GetRealValueText(energy));
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        p_item = new QStandardItem(PQ_ENERGY->GetRealValueText(rel_energy));
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        Model->appendRow(items);

        i++;
    }
}

//------------------------------------------------------------------------------

void CGOptTrajSegment::PopulateGraphData(QCPGraph* p_graph)
{
    // get first energy
    double first_energy = 0;
    if( Snapshots.count() > 0 ){
        first_energy = Snapshots.first()->GetProperty(EnergyKey);
    }

    p_graph->data()->clear();

    // load items
    int i = 1;
    foreach(CSnapshot* p_snap,Snapshots){

        double energy = p_snap->GetProperty(EnergyKey);
        double rel_energy = energy - first_energy;
        // convert to kcal/mol
        rel_energy *= 627.509608;

        p_graph->addData(i,PQ_ENERGY->GetRealValue(rel_energy));
        i++;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGOptTrajSegment::LoadTrajectoryData(void)
{
    std::ifstream   sin;

    sin.open(FileName.toLatin1());
    if( !sin ) {
        CSmallString error;
        error << tr("unable open trajectory segment") << " '" << FileName << "'";
        GetProject()->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return;
    }

    int lineno = 0;

    // read method
    if( CGaussianUtils::ReadMethod(sin,lineno,Method) == false ) return;

    // read snapshots
    while( sin.eof() == false ){
        CSnapshot* p_snap = new CSnapshot(this);
        p_snap->InitSnapshot();
        if( ReadSnapshot(sin,p_snap,lineno) == true ) {
            Snapshots.append(p_snap);
        } else {
            delete p_snap;
            break;
        }
    }

    // notify about changes
    GetTrajectory()->EmitOnTrajectorySegmentsChanged();
}

//------------------------------------------------------------------------------

bool CGOptTrajSegment::ReadSnapshot(std::istream& sin,CSnapshot* p_snap,int& lineno)
{
    // find geometry
    if( CGaussianUtils::FindGeometry(sin,lineno) == false ){
        CSmallString error;
        error << "unable to find new snapshot";
        ES_ERROR(error);
        return(false);
    }

    // read geometry
    vector<CGAtom> atoms;
    if( CGaussianUtils::ReadGeometry(sin,lineno,atoms) == false ) {
        CSmallString error;
        error << "unable to read snapshot geometry at line " << lineno;
        ES_ERROR(error);
        return(false);
    }

    // read energy
    double energy;
    string type;
    if( CGaussianUtils::ReadEnergy(sin,lineno,energy,type) == false ) {
        if( sin ){
            CSmallString error;
            error << "unable to read snapshot energy at line " << lineno;
            ES_ERROR(error);
        }
        return(false);
    }

    // update snapshot
    std::vector<CGAtom>::iterator   it = atoms.begin();
    std::vector<CGAtom>::iterator   ie = atoms.end();

    int i = 0;
    while( it != ie ){
        if( i >= GetNumberOfAtoms() ){
            ES_ERROR("more atoms than in the structure");
            return(false);
        }
        CGAtom atom = *it;
        p_snap->SetPos(i,atom.Pos);
        it++;
        i++;
    }

    // set energy
    p_snap->SetProperty(EnergyKey,energy);

    return( true );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

