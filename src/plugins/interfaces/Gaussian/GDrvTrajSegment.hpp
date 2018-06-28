#ifndef GDrvTrajSegmentH
#define GDrvTrajSegmentH
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

#include <NemesisCoreMainHeader.hpp>
#include <TrajectorySegment.hpp>
#include <Snapshot.hpp>
#include <QStandardItemModel>
#include <istream>

// -----------------------------------------------------------------------------

class CTrajectory;
class QCPGraph;
class CPhysicalQuantity;

// -----------------------------------------------------------------------------

///  Gaussian optimization trajectory segment

class CGDrvTrajSegment : public CTrajectorySegment {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    CGDrvTrajSegment(CTrajectory* p_traj);
    ~CGDrvTrajSegment(void);

// information methods ---------------------------------------------------------
    /// get number of snaphots in the file
    virtual long int GetNumberOfSnapshots(void);

    /// get current segment
    virtual CSnapshot* GetCurrentSnapshot(void);

    /// get energy model
    QAbstractItemModel* GetEnergyModel(void);

    /// get method
    const QString& GetMethod(void) const;

    /// get CV definition
    const QString& GetCVDef(void) const;

    CPhysicalQuantity* GetCVUnit(void) const;

    /// get data
    void PopulateGraphData(QCPGraph* p_graph);

// input/output methods --------------------------------------------------------
    /// load trajectory data
    virtual void LoadTrajectoryData(void);

// section of private data -----------------------------------------------------
private:
    QList<CSnapshot*>   Snapshots;
    int                 CVKey;
    int                 EnergyKey;
    CPhysicalQuantity*  PQ_CV;
    QStandardItemModel* Model;
    QString             Method; // QM method
    QString             SCDDef; // SCD definition
    double              CVScaleFac;

    // read snapshot
    bool ReadSnapshot(std::istream& sin,CSnapshot* p_snap,int& lineno,const QString& mod);

    // create property
    bool CreateProperty(void);

private slots:
    // create model
    void CreateModel(void);
};

// -----------------------------------------------------------------------------

#endif

