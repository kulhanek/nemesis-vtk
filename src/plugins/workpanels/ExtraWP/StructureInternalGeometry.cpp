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

#include "ExtraWPModule.hpp"

#include "StructureInternalGeometry.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* StructureInternalGeometryCB(void* p_data);

CExtUUID        StructureInternalGeometryID(
                    "{STR_INTERNAL_GEO:412c1f0b-5675-4198-9687-a89555971110}",
                    "Internal Geometry");

CPluginObject   StructureInternalGeometryObject(&ExtraWPPlugin,
                    StructureInternalGeometryID,WORK_PANEL_CAT,
                    ":/images/ExtraWP/StructureInternalGeometry.svg",
                    StructureInternalGeometryCB);

// -----------------------------------------------------------------------------

QObject* StructureInternalGeometryCB(void* p_data)
{
    CProject* p_project = static_cast<CProject*>(p_data);
    if( p_project == NULL ){
        ES_ERROR("CStructureInternalGeometry requires active project");
        return(NULL);
    }

    QObject* p_build_wp = new CStructureInternalGeometry(p_project);
    return(p_build_wp);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CStructureInternalGeometry::CStructureInternalGeometry(CProject* p_project)
    : CWorkPanel(&StructureInternalGeometryObject,p_project,EWPR_TOOL)
{
    WidgetUI.setupUi(this);

    // create and assign models
    BondsModel = new QStandardItemModel(this);
    BondsModel->setSortRole(Qt::UserRole + 2);
    WidgetUI.bondsTV->setModel(BondsModel);

    AnglesModel = new QStandardItemModel(this);
    AnglesModel->setSortRole(Qt::UserRole + 2);
    WidgetUI.anglesTV->setModel(AnglesModel);

    TorsionsModel = new QStandardItemModel(this);
    TorsionsModel->setSortRole(Qt::UserRole + 2);
    WidgetUI.torsionsTV->setModel(TorsionsModel);

    // set initial structure
    WidgetUI.structureW->setProject(p_project);
    WidgetUI.structureW->setObjectBaseMIMEType("structure.indexes");
    WidgetUI.structureW->setSelectionHandler(&SH_Structure);
    WidgetUI.structureW->setObject(p_project->GetStructures()->GetActiveStructure());

    // signals
    connect(WidgetUI.generatePB,SIGNAL(clicked(bool)),
        this,SLOT(GenerateInternalCoordinates(void)));
    // -------------
    connect(WidgetUI.excludeHAtomsCB,SIGNAL(toggled(bool)),
        this,SLOT(ClearAllCoordinates(void)));
    // -------------
    connect(WidgetUI.highlightSelectedCB,SIGNAL(toggled(bool)),
        this,SLOT(HighlightingToggled(bool)));
    // -------------
    connect(WidgetUI.structureW,SIGNAL(OnObjectChanged(void)),
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

CStructureInternalGeometry::~CStructureInternalGeometry()
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructureInternalGeometry::GenerateInternalCoordinates(void)
{
    // clear first everything
    ClearAllCoordinates();

    // do we have structure
    if( WidgetUI.structureW->getObject() == NULL ) return;

    CStructure* p_str = WidgetUI.structureW->getObject<CStructure*>();

    int count;
    // create list of bonds
    count = 0;
    foreach(QObject* p_qobj, p_str->GetBonds()->children() ){
        CBond* p_bond = static_cast<CBond*>(p_qobj);
        if( p_bond->IsInvalidBond() ) continue;
        CAtom* p_atom1 = p_bond->GetFirstAtom();
        CAtom* p_atom2 = p_bond->GetSecondAtom();
        if( WidgetUI.excludeHAtomsCB->isChecked() ){
            if( p_atom1->IsVirtual() ) continue;
            if( p_atom2->IsVirtual() ) continue;
        }
        QList<QStandardItem*> items;
        QStandardItem* p_item;

        p_item = new QStandardItem(p_atom1->GetName());
        p_item->setData(p_atom1->GetIndex(),Qt::UserRole + 1);
        p_item->setData(p_atom1->GetName(),Qt::UserRole + 2);
        items << p_item;

        p_item = new QStandardItem(p_atom2->GetName());
        p_item->setData(p_atom2->GetIndex(),Qt::UserRole + 1);
        p_item->setData(p_atom2->GetName(),Qt::UserRole + 2);
        items << p_item;

        double dist = p_bond->GetLength();
        p_item = new QStandardItem(PQ_DISTANCE->GetRealValueText(dist));
        p_item->setTextAlignment(Qt::AlignRight);
        p_item->setData(dist,Qt::UserRole + 2);

        items << p_item;
        BondsModel->appendRow(items);
        count++;
    }
    WidgetUI.numOfBondsSB->setValue(count);

    // create list of angles
    count = 0;
    foreach(QObject* p_qobj, p_str->GetAtoms()->children() ){
        CAtom* p_atom2 = static_cast<CAtom*>(p_qobj);
        int nbonds = p_atom2->GetNumberOfBonds();
        if(  nbonds < 2 ) continue;

        for(int i=0; i < nbonds; i++){
            for(int j=i+1; j < nbonds; j++ ){
                CBond* p_bond1 = p_atom2->GetBonds().at(i);
                CBond* p_bond2 = p_atom2->GetBonds().at(j);
                CAtom* p_atom1 = p_bond1->GetOppositeAtom(p_atom2);
                CAtom* p_atom3 = p_bond2->GetOppositeAtom(p_atom2);
                if( (p_atom1 == NULL) || (p_atom3 == NULL) ) continue;
                if( WidgetUI.excludeHAtomsCB->isChecked() ){
                    if( p_atom1->IsVirtual() ) continue;
                    if( p_atom2->IsVirtual() ) continue;
                    if( p_atom3->IsVirtual() ) continue;
                }
                QList<QStandardItem*> items;
                QStandardItem* p_item;
                p_item = new QStandardItem(p_atom1->GetName());
                p_item->setData(p_atom1->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom1->GetName(),Qt::UserRole + 2);
                items << p_item;
                p_item = new QStandardItem(p_atom2->GetName());
                p_item->setData(p_atom2->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom2->GetName(),Qt::UserRole + 2);
                items << p_item;

                p_item = new QStandardItem(p_atom3->GetName());
                p_item->setData(p_atom3->GetIndex(),Qt::UserRole + 1);
                p_item->setData(p_atom3->GetName(),Qt::UserRole + 2);
                items << p_item;

                double angle = CGeoMeasurement::GetAngle(p_atom1->GetPos(),p_atom2->GetPos(),p_atom3->GetPos());
                p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle));
                p_item->setTextAlignment(Qt::AlignRight);
                p_item->setData(angle,Qt::UserRole + 2);
                items << p_item;

                AnglesModel->appendRow(items);
                count++;

            }
        }
    }
    WidgetUI.numOfAnglesSB->setValue(count);

    // create list of torsions
    count = 0;
    int lcount = 0;
    foreach(QObject* p_qobj, p_str->GetBonds()->children() ){
        CBond* p_bond2 = static_cast<CBond*>(p_qobj);
        if( p_bond2->IsInvalidBond() ) continue;

        CAtom* p_atom2 = p_bond2->GetFirstAtom();
        CAtom* p_atom3 = p_bond2->GetSecondAtom();

        for(int i=0; i < p_atom2->GetNumberOfBonds(); i++){
            CBond* p_bond1 = p_atom2->GetBonds().at(i);
            if( p_bond1 == p_bond2 ) continue;
            for(int j=0; j < p_atom3->GetNumberOfBonds(); j++){
                CBond* p_bond3 = p_atom3->GetBonds().at(j);
                if( p_bond3 == p_bond2 ) continue;
                if( p_bond3 == p_bond1 ) continue;
                CAtom* p_atom1 = p_bond1->GetOppositeAtom(p_atom2);
                CAtom* p_atom4 = p_bond3->GetOppositeAtom(p_atom3);
                if( (p_atom1 == NULL) || (p_atom4 == NULL) ) continue;

                if( WidgetUI.excludeHAtomsCB->isChecked() ){
                    if( p_atom1->IsVirtual() ) continue;
                    if( p_atom2->IsVirtual() ) continue;
                    if( p_atom3->IsVirtual() ) continue;
                    if( p_atom4->IsVirtual() ) continue;
                }

                bool nolindep = false;

                nolindep |= CGeoMeasurement::GetAngle(p_atom1->GetPos(),p_atom2->GetPos(),p_atom3->GetPos()) > M_PI*0.95;

                nolindep |= CGeoMeasurement::GetAngle(p_atom2->GetPos(),p_atom3->GetPos(),p_atom4->GetPos()) > M_PI*0.95;

                if( nolindep == false ) {

                    QList<QStandardItem*> items;
                    QStandardItem* p_item;

                    p_item = new QStandardItem(p_atom1->GetName());
                    p_item->setData(p_atom1->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom1->GetName(),Qt::UserRole + 2);
                    items << p_item;

                    p_item = new QStandardItem(p_atom2->GetName());
                    p_item->setData(p_atom2->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom2->GetName(),Qt::UserRole + 2);
                    items << p_item;

                    p_item = new QStandardItem(p_atom3->GetName());
                    p_item->setData(p_atom3->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom3->GetName(),Qt::UserRole + 2);
                    items << p_item;

                    p_item = new QStandardItem(p_atom4->GetName());
                    p_item->setData(p_atom4->GetIndex(),Qt::UserRole + 1);
                    p_item->setData(p_atom4->GetName(),Qt::UserRole + 2);
                    items << p_item;

                    double angle = CGeoMeasurement::GetTorsion(p_atom1->GetPos(),p_atom2->GetPos(),
                                                               p_atom3->GetPos(),p_atom4->GetPos());
                    p_item = new QStandardItem(PQ_ANGLE->GetRealValueText(angle));
                    p_item->setTextAlignment(Qt::AlignRight);
                    p_item->setData(angle,Qt::UserRole + 2);

                    items << p_item;
                    TorsionsModel->appendRow(items);
                    count++;
                } else {
                    lcount++;
                }
            }
        }
    }
    WidgetUI.numOfTorsionsSB->setValue(count);
    WidgetUI.linearDependenciesSB->setValue(lcount);
}

//------------------------------------------------------------------------------

void CStructureInternalGeometry::ClearAllCoordinates(void)
{
    // clear counters
    WidgetUI.numOfBondsSB->setValue(0);
    WidgetUI.numOfAnglesSB->setValue(0);
    WidgetUI.numOfTorsionsSB->setValue(0);

    // clear models
    BondsModel->clear();
    AnglesModel->clear();
    TorsionsModel->clear();

    // restore headers
    QStringList header;
    QString     valueitem;

    valueitem = tr("Distance [%1]");
    valueitem = valueitem.arg(PQ_DISTANCE->GetUnitName());
    header << tr("Atom 1") << tr("Atom 2") << valueitem;
    BondsModel->setHorizontalHeaderLabels(header);

    header.clear();
    valueitem = tr("Angle [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << tr("Atom 1") << tr("Atom 2") << tr("Atom 3") << valueitem;
    AnglesModel->setHorizontalHeaderLabels(header);

    header.clear();
    valueitem = tr("Angle [%1]");
    valueitem = valueitem.arg(PQ_ANGLE->GetUnitName());
    header << tr("Atom 1") << tr("Atom 2") << tr("Atom 3") << tr("Atom 4") << valueitem;
    TorsionsModel->setHorizontalHeaderLabels(header);

    // cancel highlighting
    HighlightNone(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CStructureInternalGeometry::HighlightingToggled(bool set)
{
    if( set == false ){
        HighlightNone(true);
    } else {
        TabChanged();
    }
}

//------------------------------------------------------------------------------

void CStructureInternalGeometry::TabChanged(void)
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

void CStructureInternalGeometry::HighlightNone(bool repaint)
{
    // do we have structure
    if( WidgetUI.structureW->getObject() == NULL ) return;

    // set EPOF_SELECTED to false for every atom
    CStructure* p_str = WidgetUI.structureW->getObject<CStructure*>();
    foreach(QObject* p_qobj, p_str->GetAtoms()->children() ){
        CAtom* p_atom = static_cast<CAtom*>(p_qobj);
        p_atom->SetFlag(EPOF_SELECTED,false);
    }

    // repaint project
    if( repaint ) GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CStructureInternalGeometry::BondsSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW->getObject() == NULL ) return;

    // shell we select bond?
    if( WidgetUI.highlightSelectedCB->isChecked() == false ) return;

    // select atoms
    QModelIndexList selected_rows1 = WidgetUI.bondsTV->selectionModel()->selectedRows(0);
    QModelIndexList selected_rows2 = WidgetUI.bondsTV->selectionModel()->selectedRows(1);
    for(int i=0; i < selected_rows1.count(); i++){
        QStandardItem* p_it1 = BondsModel->itemFromIndex(selected_rows1.at(i));
        QStandardItem* p_it2 = BondsModel->itemFromIndex(selected_rows2.at(i));
        int index1 = p_it1->data().toInt();
        CAtom* p_at1 = dynamic_cast<CAtom*>(GetProject()->FindObject(index1));
        if( p_at1 != NULL ){
            p_at1->SetFlag(EPOF_SELECTED,true);
        }
        int index2 = p_it2->data().toInt();
        CAtom* p_at2 = dynamic_cast<CAtom*>(GetProject()->FindObject(index2));
        if( p_at2 != NULL ){
            p_at2->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CStructureInternalGeometry::AnglesSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW->getObject() == NULL ) return;

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
        int index1 = p_it1->data().toInt();
        CAtom* p_at1 = dynamic_cast<CAtom*>(GetProject()->FindObject(index1));
        if( p_at1 != NULL ){
            p_at1->SetFlag(EPOF_SELECTED,true);
        }
        int index2 = p_it2->data().toInt();
        CAtom* p_at2 = dynamic_cast<CAtom*>(GetProject()->FindObject(index2));
        if( p_at2 != NULL ){
            p_at2->SetFlag(EPOF_SELECTED,true);
        }
        int index3 = p_it3->data().toInt();
        CAtom* p_at3 = dynamic_cast<CAtom*>(GetProject()->FindObject(index3));
        if( p_at3 != NULL ){
            p_at3->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//------------------------------------------------------------------------------

void CStructureInternalGeometry::TorsionsSelectionChanged(void)
{
    // reset highlighting
    HighlightNone();

    // do we have structure
    if( WidgetUI.structureW->getObject() == NULL ) return;

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
        int index1 = p_it1->data().toInt();
        CAtom* p_at1 = dynamic_cast<CAtom*>(GetProject()->FindObject(index1));
        if( p_at1 != NULL ){
            p_at1->SetFlag(EPOF_SELECTED,true);
        }
        int index2 = p_it2->data().toInt();
        CAtom* p_at2 = dynamic_cast<CAtom*>(GetProject()->FindObject(index2));
        if( p_at2 != NULL ){
            p_at2->SetFlag(EPOF_SELECTED,true);
        }
        int index3 = p_it3->data().toInt();
        CAtom* p_at3 = dynamic_cast<CAtom*>(GetProject()->FindObject(index3));
        if( p_at3 != NULL ){
            p_at3->SetFlag(EPOF_SELECTED,true);
        }
        int index4 = p_it4->data().toInt();
        CAtom* p_at4 = dynamic_cast<CAtom*>(GetProject()->FindObject(index4));
        if( p_at4 != NULL ){
            p_at4->SetFlag(EPOF_SELECTED,true);
        }
    }
    // repaint project
    GetProject()->RepaintProject();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



