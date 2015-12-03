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

#include <QtGui>

#include <PluginObject.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>

#include <StructureList.hpp>
#include <Structure.hpp>
#include <Project.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>
#include <BondList.hpp>
#include <Bond.hpp>
#include <AtomList.hpp>
#include <Atom.hpp>
#include <StructureSelection.hpp>
#include <HistoryList.hpp>
#include <GeoMeasurement.hpp>
#include <QMessageBox>

#include "ExtraToolsModule.hpp"

#include "CompareInternalGeometry.hpp"
#include "CompareInternalGeometry.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* CompareInternalGeometryCB(void* p_data);

CExtUUID        CompareInternalGeometryID(
                    "{CMP_INTERNAL_GEO:2c451d0a-b794-4b38-a362-71417987c7a8}",
                    "Compare Internal Geometry");

CPluginObject   CompareInternalGeometryObject(&ExtraToolsPlugin,
                    CompareInternalGeometryID,WORK_PANEL_CAT,
                    ":/images/ExtraTools/CompareInternalGeometry.svg",
                    CompareInternalGeometryCB);

// -----------------------------------------------------------------------------

QObject* CompareInternalGeometryCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CCompareInternalGeometry requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CCompareInternalGeometry(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCompareInternalGeometry::CCompareInternalGeometry(CProject* p_project)
    : CWorkPanel(&CompareInternalGeometryObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // create and assign models
    BondsModel = new QStandardItemModel(this);
    BondsModel->setSortRole(Qt::UserRole + 3);
    WidgetUI.bondsTV->setModel(BondsModel);

    AnglesModel = new QStandardItemModel(this);
    AnglesModel->setSortRole(Qt::UserRole + 3);
    WidgetUI.anglesTV->setModel(AnglesModel);

    TorsionsModel = new QStandardItemModel(this);
    TorsionsModel->setSortRole(Qt::UserRole + 3);
    WidgetUI.torsionsTV->setModel(TorsionsModel);

    // set initial structure
    WidgetUI.structureW1->setProject(p_project);
    WidgetUI.structureW1->setObjectBaseMIMEType("structure.indexes");
    WidgetUI.structureW1->setSelectionHandler(&SH_Structure);
    WidgetUI.structureW2->setProject(p_project);
    WidgetUI.structureW2->setObjectBaseMIMEType("structure.indexes");
    WidgetUI.structureW2->setSelectionHandler(&SH_Structure);

    // units
    WidgetUI.aveBondsSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.mseBondsSB->setPhysicalQuantity(PQ_DISTANCE);
    WidgetUI.rmseBondsSB->setPhysicalQuantity(PQ_DISTANCE);

    WidgetUI.aveAnglesSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.mseAnglesSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.rmseAnglesSB->setPhysicalQuantity(PQ_ANGLE);

    WidgetUI.aveTorsionsSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.mseTorsionsSB->setPhysicalQuantity(PQ_ANGLE);
    WidgetUI.rmseTorsionsSB->setPhysicalQuantity(PQ_ANGLE);

    // signals
    connect(WidgetUI.comparePB,SIGNAL(clicked(bool)),
        this,SLOT(CompareInternalCoordinates(void)));
    // -------------
    connect(WidgetUI.excludeHAtomsCB,SIGNAL(toggled(bool)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(WidgetUI.highlightSelectedCB,SIGNAL(toggled(bool)),
        this,SLOT(HighlightingToggled(bool)));
    // -------------
    connect(WidgetUI.structureW1,SIGNAL(OnObjectChanged(void)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(WidgetUI.structureW2,SIGNAL(OnObjectChanged(void)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(WidgetUI.bondsTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(BondsSelectionChanged(void)));
    // -------------
    connect(WidgetUI.anglesTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(AnglesSelectionChanged(void)));
    // -------------
    connect(WidgetUI.torsionsTV->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(TorsionsSelectionChanged(void)));
    // -------------
    connect(WidgetUI.tabWidget,SIGNAL(currentChanged(int)),
        this,SLOT(TabChanged(void)));
    // -------------
    connect(PQ_DISTANCE,SIGNAL(OnUnitChanged(void)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(PQ_ANGLE,SIGNAL(OnUnitChanged(void)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(p_project->GetHistory(),SIGNAL(OnHistoryChanged(EHistoryChangeMessage)),
        this,SLOT(ClearAllCoordinates(void)));

    // load work panel setup
    LoadWorkPanelSetup();

    // initialize data
    ClearAllCoordinates();
}

//------------------------------------------------------------------------------

CCompareInternalGeometry::~CCompareInternalGeometry()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCompareInternalGeometry::CompareInternalCoordinates(void)
{
    // clear first everything
    ClearAllCoordinates();

    // do we have both structures
    if( WidgetUI.structureW1->getObject() == NULL ) return;
    if( WidgetUI.structureW2->getObject() == NULL ) return;

    if( CompareBonds() == false ) return;
    if( CompareAngles() == false ) return;
    if( CompareTorsions() == false ) return;
}

//------------------------------------------------------------------------------

bool CCompareInternalGeometry::CompareBonds(void)
{
    CStructure* p_str1 = WidgetUI.structureW1->getObject<CStructure*>();
    CStructure* p_str2 = WidgetUI.structureW2->getObject<CStructure*>();

    int count;
    double sum,sum2,ave,mse,rmse;
    // create list of bonds
    count = 0;
    sum = 0.0;
    sum2 = 0.0;
    ave = 0.0;
    mse = 0.0;
    rmse = 0.0;

    if( p_str1->GetAtoms()->GetNumberOfAtoms() != p_str2->GetAtoms()->GetNumberOfAtoms() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in atoms)"));
        ClearAllCoordinates();
        return(false);
    }

    if( p_str1->GetBonds()->GetNumberOfBonds() != p_str2->GetBonds()->GetNumberOfBonds() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in bonds)"));
        ClearAllCoordinates();
        return(false);
    }

    QList<QObject*>::const_iterator bit = p_str2->GetBonds()->children().constBegin();
    foreach(QObject* p_qobj, p_str1->GetBonds()->children() ){
        CBond* p_bond1 = static_cast<CBond*>(p_qobj);
        CBond* p_bond2 = static_cast<CBond*>(*bit);
        bit++;

        if( p_bond1->IsInvalidBond() ) continue;
        if( CompareBonds(p_bond1,p_bond2) == false ) return(false);

        CAtom* p_atom11 = p_bond1->GetFirstAtom();
        CAtom* p_atom12 = p_bond1->GetSecondAtom();
        if( WidgetUI.excludeHAtomsCB->isChecked() ){
            if( p_atom11->IsVirtual() ) continue;
            if( p_atom12->IsVirtual() ) continue;
        }
        CAtom* p_atom21 = p_bond2->GetFirstAtom();
        CAtom* p_atom22 = p_bond2->GetSecondAtom();

        if( CompareAtoms(p_atom11,p_atom21) == false ) return(false);
        if( CompareAtoms(p_atom12,p_atom22) == false ) return(false);


        QList<QStandardItem*> items;
        QStandardItem* p_item;
        p_item = new QStandardItem(p_atom11->GetName());
        p_item->setData(p_atom11->GetIndex(),Qt::UserRole + 1);
        p_item->setData(p_atom21->GetIndex(),Qt::UserRole + 2);
        p_item->setData(p_atom11->GetName(),Qt::UserRole + 3);

        items << p_item;
        p_item = new QStandardItem(p_atom12->GetName());
        p_item->setData(p_atom12->GetIndex(),Qt::UserRole + 1);
        p_item->setData(p_atom22->GetIndex(),Qt::UserRole + 2);
        p_item->setData(p_atom12->GetName(),Qt::UserRole + 3);
        items << p_item;

        double v1 = p_bond1->GetLength();
        p_item = new QStandardItem(PQ_DISTANCE->GetRealValueText(v1));
        p_item->setData(v1,Qt::UserRole + 3);
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        double v2 = p_bond2->GetLength();
        p_item = new QStandardItem(PQ_DISTANCE->GetRealValueText(v2));
        p_item->setData(v2,Qt::UserRole + 3);
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        double diff = v2 - v1;
        p_item = new QStandardItem(PQ_DISTANCE->GetRealValueText(diff));
        p_item->setData(diff,Qt::UserRole + 3);
        p_item->setTextAlignment(Qt::AlignRight);
        items << p_item;

        BondsModel->appendRow(items);
        count++;
        sum += diff;
        sum2 += diff*diff;
        if( fabs(mse) < fabs(diff)  ){
            mse = diff;
        }
    }
    WidgetUI.numOfBondsSB->setValue(count);
    if( count > 0 ){
        ave = sum / (double)count;
        rmse = sqrt(sum2 / (double)count);
    }
    WidgetUI.aveBondsSB->setInternalValue(ave);
    WidgetUI.mseBondsSB->setInternalValue(mse);
    WidgetUI.rmseBondsSB->setInternalValue(rmse);

    return(true);
}

//------------------------------------------------------------------------------

bool CCompareInternalGeometry::CompareAngles(void)
{
    CStructure* p_str1 = WidgetUI.structureW1->getObject<CStructure*>();
    CStructure* p_str2 = WidgetUI.structureW2->getObject<CStructure*>();

    int count;
    double sum,sum2,ave,mse,rmse;
    // create list of angles
    count = 0;
    sum = 0.0;
    sum2 = 0.0;
    ave = 0.0;
    mse = 0.0;
    rmse = 0.0;

    if( p_str1->GetAtoms()->GetNumberOfAtoms() != p_str2->GetAtoms()->GetNumberOfAtoms() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in atoms)"));
        ClearAllCoordinates();
        return(false);
    }

    if( p_str1->GetBonds()->GetNumberOfBonds() != p_str2->GetBonds()->GetNumberOfBonds() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in bonds)"));
        ClearAllCoordinates();
        return(false);
    }

    QList<QObject*>::const_iterator bit = p_str2->GetAtoms()->children().constBegin();
    foreach(QObject* p_qobj, p_str1->GetAtoms()->children() ){
        CAtom* p_atom12 = static_cast<CAtom*>(p_qobj);
        CAtom* p_atom22 = static_cast<CAtom*>(*bit);
        bit++;
        if( CompareAtoms(p_atom12,p_atom22) == false ) return(false);

        int nbonds = p_atom12->GetNumberOfBonds();
        if(  nbonds < 2 ) continue;

        for(int i=0; i < nbonds; i++){
            for(int j=i+1; j < nbonds; j++ ){
                CBond* p_bond11 = p_atom12->GetBonds().at(i);
                CBond* p_bond12 = p_atom12->GetBonds().at(j);
                CAtom* p_atom11 = p_bond11->GetOppositeAtom(p_atom12);
                CAtom* p_atom13 = p_bond12->GetOppositeAtom(p_atom12);
                if( (p_atom11 == NULL) || (p_atom13 == NULL) ) continue;
                if( WidgetUI.excludeHAtomsCB->isChecked() ){
                    if( p_atom11->IsVirtual() ) continue;
                    if( p_atom12->IsVirtual() ) continue;
                    if( p_atom13->IsVirtual() ) continue;
                }

                CBond* p_bond21 = p_atom22->GetBonds().at(i);
                CBond* p_bond22 = p_atom22->GetBonds().at(j);
                CAtom* p_atom21 = p_bond21->GetOppositeAtom(p_atom22);
                CAtom* p_atom23 = p_bond22->GetOppositeAtom(p_atom22);
                if( (p_atom21 == NULL) || (p_atom23 == NULL) ) continue;
                if( WidgetUI.excludeHAtomsCB->isChecked() ){
                    if( p_atom21->IsVirtual() ) continue;
                    if( p_atom22->IsVirtual() ) continue;
                    if( p_atom23->IsVirtual() ) continue;
                }

                if( CompareAtoms(p_atom11,p_atom21) == false ) return(false);
                if( CompareAtoms(p_atom13,p_atom23) == false ) return(false);

                QList<QStandardItem*> items;
                QStandardItem* p_item;
                p_item = new QStandardItem(p_atom11->GetName());
                p_item->setData(p_atom11->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom21->GetIndex(),Qt::UserRole + 2);
                p_item->setData(p_atom11->GetName(),Qt::UserRole + 3);
                items << p_item;
                p_item = new QStandardItem(p_atom12->GetName());
                p_item->setData(p_atom12->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom22->GetIndex(),Qt::UserRole + 2);
                p_item->setData(p_atom12->GetName(),Qt::UserRole + 3);
                items << p_item;
                p_item = new QStandardItem(p_atom13->GetName());
                p_item->setData(p_atom13->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom23->GetIndex(),Qt::UserRole + 2);
                p_item->setData(p_atom13->GetName(),Qt::UserRole + 3);
                items << p_item;

                double angle1 = CGeoMeasurement::GetAngle(p_atom11->GetPos(),p_atom12->GetPos(),p_atom13->GetPos());
                p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle1));
                p_item->setData(angle1,Qt::UserRole + 3);
                p_item->setTextAlignment(Qt::AlignRight);
                items << p_item;

                double angle2 = CGeoMeasurement::GetAngle(p_atom21->GetPos(),p_atom22->GetPos(),p_atom23->GetPos());
                p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle2));
                p_item->setData(angle2,Qt::UserRole + 3);
                p_item->setTextAlignment(Qt::AlignRight);
                items << p_item;

                double diff = angle2 - angle1;
                p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(diff));
                p_item->setData(diff,Qt::UserRole + 3);
                p_item->setTextAlignment(Qt::AlignRight);
                items << p_item;

                AnglesModel->appendRow(items);
                count++;
                sum += diff;
                sum2 += diff*diff;
                if( fabs(mse) < fabs(diff)  ){
                    mse = diff;
                }
            }
        }
    }
    WidgetUI.numOfAnglesSB->setValue(count);
    if( count > 0 ){
        ave = sum / (double)count;
        rmse = sqrt(sum2 / (double)count);
    }
    WidgetUI.aveAnglesSB->setInternalValue(ave);
    WidgetUI.mseAnglesSB->setInternalValue(mse);
    WidgetUI.rmseAnglesSB->setInternalValue(rmse);

    return(true);
}

//------------------------------------------------------------------------------

bool CCompareInternalGeometry::CompareTorsions(void)
{
    CStructure* p_str1 = WidgetUI.structureW1->getObject<CStructure*>();
    CStructure* p_str2 = WidgetUI.structureW2->getObject<CStructure*>();

    int count,lcount;
    double sum,sum2,ave,mse,rmse;
    // create list of torsions
    count = 0;
    lcount = 0;
    sum = 0.0;
    sum2 = 0.0;
    ave = 0.0;
    mse = 0.0;
    rmse = 0.0;

    if( p_str1->GetAtoms()->GetNumberOfAtoms() != p_str2->GetAtoms()->GetNumberOfAtoms() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in atoms)"));
        ClearAllCoordinates();
        return(false);
    }

    if( p_str1->GetBonds()->GetNumberOfBonds() != p_str2->GetBonds()->GetNumberOfBonds() ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in bonds)"));
        ClearAllCoordinates();
        return(false);
    }

    QList<QObject*>::const_iterator bit = p_str2->GetBonds()->children().constBegin();
    foreach(QObject* p_qobj, p_str1->GetBonds()->children() ){
        CBond* p_bond12 = static_cast<CBond*>(p_qobj);
        CBond* p_bond22 = static_cast<CBond*>(*bit);
        bit++;
        if( p_bond12->IsInvalidBond() ) continue;

        if( CompareBonds(p_bond12,p_bond22) == false ) return(false);

        CAtom* p_atom12 = p_bond12->GetFirstAtom();
        CAtom* p_atom13 = p_bond12->GetSecondAtom();

        CAtom* p_atom22 = p_bond22->GetFirstAtom();
        CAtom* p_atom23 = p_bond22->GetSecondAtom();

        if( CompareAtoms(p_atom12,p_atom22) == false ) return(false);
        if( CompareAtoms(p_atom13,p_atom23) == false ) return(false);

        for(int i=0; i < p_atom12->GetNumberOfBonds(); i++){
            CBond* p_bond11 = p_atom12->GetBonds().at(i);
            CBond* p_bond21 = p_atom22->GetBonds().at(i);
            if( p_bond11 == p_bond12 ) continue;
            for(int j=0; j < p_atom13->GetNumberOfBonds(); j++){
                CBond* p_bond13 = p_atom13->GetBonds().at(j);
                CBond* p_bond23 = p_atom23->GetBonds().at(j);
                if( p_bond13 == p_bond12 ) continue;
                if( p_bond13 == p_bond11 ) continue;
                CAtom* p_atom11 = p_bond11->GetOppositeAtom(p_atom12);
                CAtom* p_atom14 = p_bond13->GetOppositeAtom(p_atom13);
                if( (p_atom11 == NULL) || (p_atom14 == NULL) ) continue;

                CAtom* p_atom21 = p_bond21->GetOppositeAtom(p_atom22);
                CAtom* p_atom24 = p_bond23->GetOppositeAtom(p_atom23);

                if( CompareAtoms(p_atom11,p_atom21) == false ) return(false);
                if( CompareAtoms(p_atom14,p_atom24) == false ) return(false);

                if( WidgetUI.excludeHAtomsCB->isChecked() ){
                    if( p_atom11->IsVirtual() ) continue;
                    if( p_atom12->IsVirtual() ) continue;
                    if( p_atom13->IsVirtual() ) continue;
                    if( p_atom14->IsVirtual() ) continue;
                }

                bool nolindep = false;

                nolindep |= CGeoMeasurement::GetAngle(p_atom11->GetPos(),p_atom12->GetPos(),p_atom13->GetPos()) > M_PI*0.95;
                nolindep |= CGeoMeasurement::GetAngle(p_atom21->GetPos(),p_atom22->GetPos(),p_atom23->GetPos()) > M_PI*0.95;

                nolindep |= CGeoMeasurement::GetAngle(p_atom12->GetPos(),p_atom13->GetPos(),p_atom14->GetPos()) > M_PI*0.95;
                nolindep |= CGeoMeasurement::GetAngle(p_atom22->GetPos(),p_atom23->GetPos(),p_atom24->GetPos()) > M_PI*0.95;

                if( nolindep == false ) {

                    double angle1 = CGeoMeasurement::GetTorsion(p_atom11->GetPos(),p_atom12->GetPos(),
                                                               p_atom13->GetPos(),p_atom14->GetPos());

                    double angle2 = CGeoMeasurement::GetTorsion(p_atom21->GetPos(),p_atom22->GetPos(),
                                                               p_atom23->GetPos(),p_atom24->GetPos());

                    QList<QStandardItem*> items;
                    QStandardItem* p_item;
                    p_item = new QStandardItem(p_atom11->GetName());
                    p_item->setData(p_atom11->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom21->GetIndex(),Qt::UserRole + 2);
                    p_item->setData(p_atom11->GetName(),Qt::UserRole + 3);
                    items << p_item;

                    p_item = new QStandardItem(p_atom12->GetName());
                    p_item->setData(p_atom12->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom22->GetIndex(),Qt::UserRole + 2);
                    p_item->setData(p_atom12->GetName(),Qt::UserRole + 3);
                    items << p_item;

                    p_item = new QStandardItem(p_atom13->GetName());
                    p_item->setData(p_atom13->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom23->GetIndex(),Qt::UserRole + 2);
                    p_item->setData(p_atom13->GetName(),Qt::UserRole + 3);
                    items << p_item;

                    p_item = new QStandardItem(p_atom14->GetName());
                    p_item->setData(p_atom14->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom24->GetIndex(),Qt::UserRole + 2);
                    p_item->setData(p_atom14->GetName(),Qt::UserRole + 3);
                    items << p_item;


                    p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle1));
                    p_item->setData(angle1,Qt::UserRole + 3);
                    p_item->setTextAlignment(Qt::AlignRight);
                    items << p_item;


                    p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle2));
                    p_item->setData(angle2,Qt::UserRole + 3);
                    p_item->setTextAlignment(Qt::AlignRight);
                    items << p_item;

                    double diff = angle2 - angle1;
                    // image angle
                    diff = diff - 2.0*M_PI*floor((diff+M_PI)/(2.0*M_PI));
                    p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(diff));
                    p_item->setData(diff,Qt::UserRole + 3);
                    p_item->setTextAlignment(Qt::AlignRight);
                    items << p_item;

                    TorsionsModel->appendRow(items);
                    count++;
                    sum += diff;
                    sum2 += diff*diff;
                    if( fabs(mse) < fabs(diff)  ){
                        mse = diff;
                    }
                } else {
                    lcount++;
                }
            }
        }
    }
    WidgetUI.numOfTorsionsSB->setValue(count);
    WidgetUI.linearDependenciesSB->setValue(lcount);

    if( count > 0 ){
        ave = sum / (double)count;
        rmse = sqrt(sum2 / (double)count);
    }
    WidgetUI.aveTorsionsSB->setInternalValue(ave);
    WidgetUI.mseTorsionsSB->setInternalValue(mse);
    WidgetUI.rmseTorsionsSB->setInternalValue(rmse);

    return(true);
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::ClearAllCoordinates(void)
{
    CStructure* p_str1 = WidgetUI.structureW1->getObject<CStructure*>();
    CStructure* p_str2 = WidgetUI.structureW2->getObject<CStructure*>();

    WidgetUI.comparePB->setEnabled( (p_str1!=NULL) && (p_str2!=NULL) );

    // clear counters
    WidgetUI.numOfBondsSB->setValue(0);
    WidgetUI.aveBondsSB->setInternalValue(0);
    WidgetUI.mseBondsSB->setInternalValue(0);
    WidgetUI.rmseBondsSB->setInternalValue(0);

    WidgetUI.numOfAnglesSB->setValue(0);
    WidgetUI.aveAnglesSB->setInternalValue(0);
    WidgetUI.mseAnglesSB->setInternalValue(0);
    WidgetUI.rmseAnglesSB->setInternalValue(0);

    WidgetUI.numOfTorsionsSB->setValue(0);
    WidgetUI.aveTorsionsSB->setInternalValue(0);
    WidgetUI.mseTorsionsSB->setInternalValue(0);
    WidgetUI.rmseTorsionsSB->setInternalValue(0);

    // clear models
    BondsModel->clear();
    AnglesModel->clear();
    TorsionsModel->clear();

    // restore headers
    QStringList header;
    QString     valueitem;

    header.clear();
    header << tr("Atom 1") << tr("Atom 2");
    valueitem = tr("Distance #1 [%1]");
    valueitem = valueitem.arg(PQ_DISTANCE->GetUnitName());
    header << valueitem;
    valueitem = tr("Distance #2 [%1]");
    valueitem = valueitem.arg(PQ_DISTANCE->GetUnitName());
    header << valueitem;
    valueitem = tr("Error [%1]");
    valueitem = valueitem.arg(PQ_DISTANCE->GetUnitName());
    header << valueitem;

    BondsModel->setHorizontalHeaderLabels(header);

    header.clear();
    header << tr("Atom 1") << tr("Atom 2") << tr("Atom 3");
    valueitem = tr("Angle #1 [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    valueitem = tr("Angle #2 [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    valueitem = tr("Error [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    AnglesModel->setHorizontalHeaderLabels(header);

    header.clear();
    header << tr("Atom 1") << tr("Atom 2") << tr("Atom 3") << tr("Atom 4");
    valueitem = tr("Angle #1 [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    valueitem = tr("Angle #2 [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    valueitem = tr("Error [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << valueitem;
    TorsionsModel->setHorizontalHeaderLabels(header);

    // cancel highlighting
    HighlightNone(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCompareInternalGeometry::HighlightingToggled(bool set)
{
    if( set == false ){
        HighlightNone(true);
    } else {
        TabChanged();
    }
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::TabChanged(void)
{
    if( WidgetUI.tabWidget->currentIndex() == 0 ){
        BondsSelectionChanged();
    }
    if( WidgetUI.tabWidget->currentIndex() == 1 ){
        AnglesSelectionChanged();
    }
    if( WidgetUI.tabWidget->currentIndex() == 2 ){
        TorsionsSelectionChanged();
    }
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::HighlightNone(bool repaint)
{
    // do we have structure
    if( WidgetUI.structureW1->getObject() != NULL ) {
        // set EPOF_SELECTED to false for every atom
        CStructure* p_str = WidgetUI.structureW1->getObject<CStructure*>();
        foreach(QObject* p_qobj, p_str->GetAtoms()->children() ){
            CAtom* p_atom = static_cast<CAtom*>(p_qobj);
            p_atom->SetFlag(EPOF_SELECTED,false);
        }
    }
    if( WidgetUI.structureW2->getObject() != NULL ) {
        // set EPOF_SELECTED to false for every atom
        CStructure* p_str = WidgetUI.structureW2->getObject<CStructure*>();
        foreach(QObject* p_qobj, p_str->GetAtoms()->children() ){
            CAtom* p_atom = static_cast<CAtom*>(p_qobj);
            p_atom->SetFlag(EPOF_SELECTED,false);
        }
    }

    // repaint project
    if( repaint ) GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::BondsSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW1->getObject() == NULL ) return;
    if( WidgetUI.structureW2->getObject() == NULL ) return;

    // shell we select bond?
    if( WidgetUI.highlightSelectedCB->isChecked() == false ) return;

    // select atoms
    QModelIndexList selected_rows1 = WidgetUI.bondsTV->selectionModel()->selectedRows(0);
    QModelIndexList selected_rows2 = WidgetUI.bondsTV->selectionModel()->selectedRows(1);
    for(int i=0; i < selected_rows1.count(); i++){
        QStandardItem* p_it1 = BondsModel->itemFromIndex(selected_rows1.at(i));
        QStandardItem* p_it2 = BondsModel->itemFromIndex(selected_rows2.at(i));
        int index11 = p_it1->data(Qt::UserRole + 1).toInt();
        CAtom* p_at11 = dynamic_cast<CAtom*>(GetProject()->FindObject(index11));
        if( p_at11 != NULL ){
            p_at11->SetFlag(EPOF_SELECTED,true);
        }
        int index12 = p_it2->data(Qt::UserRole + 1).toInt();
        CAtom* p_at12 = dynamic_cast<CAtom*>(GetProject()->FindObject(index12));
        if( p_at12 != NULL ){
            p_at12->SetFlag(EPOF_SELECTED,true);
        }

        int index21 = p_it1->data(Qt::UserRole + 2).toInt();
        CAtom* p_at21 = dynamic_cast<CAtom*>(GetProject()->FindObject(index21));
        if( p_at21 != NULL ){
            p_at21->SetFlag(EPOF_SELECTED,true);
        }
        int index22 = p_it2->data(Qt::UserRole + 2).toInt();
        CAtom* p_at22 = dynamic_cast<CAtom*>(GetProject()->FindObject(index22));
        if( p_at22 != NULL ){
            p_at22->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::AnglesSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW1->getObject() == NULL ) return;
    if( WidgetUI.structureW2->getObject() == NULL ) return;

    // shell we select bond?
    if( WidgetUI.highlightSelectedCB->isChecked() == false ) return;

    // select atoms
    QModelIndexList selected_rows1 = WidgetUI.anglesTV->selectionModel()->selectedRows(0);
    QModelIndexList selected_rows2 = WidgetUI.anglesTV->selectionModel()->selectedRows(1);
    QModelIndexList selected_rows3 = WidgetUI.anglesTV->selectionModel()->selectedRows(2);
    for(int i=0; i < selected_rows1.count(); i++){
        QStandardItem* p_it1 = AnglesModel->itemFromIndex(selected_rows1.at(i));
        QStandardItem* p_it2 = AnglesModel->itemFromIndex(selected_rows2.at(i));
        QStandardItem* p_it3 = AnglesModel->itemFromIndex(selected_rows3.at(i));
        int index11 = p_it1->data(Qt::UserRole + 1).toInt();
        CAtom* p_at11 = dynamic_cast<CAtom*>(GetProject()->FindObject(index11));
        if( p_at11 != NULL ){
            p_at11->SetFlag(EPOF_SELECTED,true);
        }
        int index12 = p_it2->data(Qt::UserRole + 1).toInt();
        CAtom* p_at12 = dynamic_cast<CAtom*>(GetProject()->FindObject(index12));
        if( p_at12 != NULL ){
            p_at12->SetFlag(EPOF_SELECTED,true);
        }
        int index13 = p_it3->data(Qt::UserRole + 1).toInt();
        CAtom* p_at13 = dynamic_cast<CAtom*>(GetProject()->FindObject(index13));
        if( p_at13 != NULL ){
            p_at13->SetFlag(EPOF_SELECTED,true);
        }

        int index21 = p_it1->data(Qt::UserRole + 2).toInt();
        CAtom* p_at21 = dynamic_cast<CAtom*>(GetProject()->FindObject(index21));
        if( p_at21 != NULL ){
            p_at21->SetFlag(EPOF_SELECTED,true);
        }
        int index22 = p_it2->data(Qt::UserRole + 2).toInt();
        CAtom* p_at22 = dynamic_cast<CAtom*>(GetProject()->FindObject(index22));
        if( p_at22 != NULL ){
            p_at22->SetFlag(EPOF_SELECTED,true);
        }
        int index23 = p_it3->data(Qt::UserRole + 2).toInt();
        CAtom* p_at23 = dynamic_cast<CAtom*>(GetProject()->FindObject(index23));
        if( p_at23 != NULL ){
            p_at23->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CCompareInternalGeometry::TorsionsSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW1->getObject() == NULL ) return;
    if( WidgetUI.structureW2->getObject() == NULL ) return;

    // shell we select bond?
    if( WidgetUI.highlightSelectedCB->isChecked() == false ) return;

    // select atoms
    QModelIndexList selected_rows1 = WidgetUI.torsionsTV->selectionModel()->selectedRows(0);
    QModelIndexList selected_rows2 = WidgetUI.torsionsTV->selectionModel()->selectedRows(1);
    QModelIndexList selected_rows3 = WidgetUI.torsionsTV->selectionModel()->selectedRows(2);
    QModelIndexList selected_rows4 = WidgetUI.torsionsTV->selectionModel()->selectedRows(3);
    for(int i=0; i < selected_rows1.count(); i++){
        QStandardItem* p_it1 = TorsionsModel->itemFromIndex(selected_rows1.at(i));
        QStandardItem* p_it2 = TorsionsModel->itemFromIndex(selected_rows2.at(i));
        QStandardItem* p_it3 = TorsionsModel->itemFromIndex(selected_rows3.at(i));
        QStandardItem* p_it4 = TorsionsModel->itemFromIndex(selected_rows4.at(i));
        int index11 = p_it1->data(Qt::UserRole + 1).toInt();
        CAtom* p_at11 = dynamic_cast<CAtom*>(GetProject()->FindObject(index11));
        if( p_at11 != NULL ){
            p_at11->SetFlag(EPOF_SELECTED,true);
        }
        int index12 = p_it2->data(Qt::UserRole + 1).toInt();
        CAtom* p_at12 = dynamic_cast<CAtom*>(GetProject()->FindObject(index12));
        if( p_at12 != NULL ){
            p_at12->SetFlag(EPOF_SELECTED,true);
        }
        int index13 = p_it3->data(Qt::UserRole + 1).toInt();
        CAtom* p_at13 = dynamic_cast<CAtom*>(GetProject()->FindObject(index13));
        if( p_at13 != NULL ){
            p_at13->SetFlag(EPOF_SELECTED,true);
        }
        int index14 = p_it4->data(Qt::UserRole + 1).toInt();
        CAtom* p_at14 = dynamic_cast<CAtom*>(GetProject()->FindObject(index14));
        if( p_at14 != NULL ){
            p_at14->SetFlag(EPOF_SELECTED,true);
        }

        int index21 = p_it1->data(Qt::UserRole + 2).toInt();
        CAtom* p_at21 = dynamic_cast<CAtom*>(GetProject()->FindObject(index21));
        if( p_at21 != NULL ){
            p_at21->SetFlag(EPOF_SELECTED,true);
        }
        int index22 = p_it2->data(Qt::UserRole + 2).toInt();
        CAtom* p_at22 = dynamic_cast<CAtom*>(GetProject()->FindObject(index22));
        if( p_at22 != NULL ){
            p_at22->SetFlag(EPOF_SELECTED,true);
        }
        int index23 = p_it3->data(Qt::UserRole + 2).toInt();
        CAtom* p_at23 = dynamic_cast<CAtom*>(GetProject()->FindObject(index23));
        if( p_at23 != NULL ){
            p_at23->SetFlag(EPOF_SELECTED,true);
        }
        int index24 = p_it4->data(Qt::UserRole + 2).toInt();
        CAtom* p_at24 = dynamic_cast<CAtom*>(GetProject()->FindObject(index24));
        if( p_at24 != NULL ){
            p_at24->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCompareInternalGeometry::CompareAtoms(CAtom* p_a1,CAtom* p_a2)
{
    bool ok = true;

    ok &= p_a1 != NULL;
    ok &= p_a2 != NULL;

    if( ok ){
        ok &= p_a1->GetName() == p_a2->GetName();
        ok &= p_a1->GetZ() == p_a2->GetZ();
        ok &= p_a1->GetNumberOfBonds() == p_a2->GetNumberOfBonds();
    }

    if( ok == false ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in atoms)"));
        ClearAllCoordinates();
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CCompareInternalGeometry::CompareBonds(CBond* p_b1,CBond* p_b2)
{
    bool ok = true;

    ok &= p_b1 != NULL;
    ok &= p_b2 != NULL;

    if( ok ){
        ok &= p_b1->GetName() == p_b2->GetName();
        ok &= p_b1->GetBondOrder() == p_b2->GetBondOrder();
        ok &= p_b1->IsInvalidBond() == p_b2->IsInvalidBond();
    }

    if( ok == false ){
        QMessageBox::critical(this,tr("Error"),tr("Structures do not have identical topologies!\n(difference in bonds)"));
        ClearAllCoordinates();
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



