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

#include "GlobalWorkPanelsModule.hpp"
#include "QuantityUnitSetupWorkPanel.hpp"

#include <PluginObject.hpp>
#include <ErrorSystem.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <PhysicalQuantities.hpp>
#include <StdPhysicalQuantity.hpp>
#include <ContainerModel.hpp>
#include <ProjectList.hpp>
#include <QMessageBox>

#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* QuantityUnitSetupWorkPanelCB(void* p_data);

CExtUUID        QuantityUnitSetupWorkPanelID(
                    "{QUANTITY_UNIT_SETUP_WORK_PANEL:98627045-b118-49e1-ab08-e779c19e2423}",
                    "Quantity Unit Setup" );

CPluginObject   QuantityUnitSetupWorkPanelObject(&GlobalWorkPanelsPlugin,
                    QuantityUnitSetupWorkPanelID,WORK_PANEL_CAT,
                    ":images/GlobalWorkPanels/QuantitySetup.svg",
                    QuantityUnitSetupWorkPanelCB);

// -----------------------------------------------------------------------------

CQuantityUnitSetupWorkPanel* p_s5_window = NULL;

// -----------------------------------------------------------------------------

QObject* QuantityUnitSetupWorkPanelCB(void* p_data)
{
    if( p_s5_window != NULL ) return(p_s5_window);
    p_s5_window = new CQuantityUnitSetupWorkPanel;
    return(p_s5_window);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CQuantityUnitSetupWorkPanel::CQuantityUnitSetupWorkPanel(void)
    : CWorkPanel(&QuantityUnitSetupWorkPanelObject,NULL,EWPR_WINDOW)
{
    WidgetUI.setupUi(this);

    ActualQuantity = 0;
    ActualUnit = 0;

    // setup models ------------------------------
    QuantitiesModel = PhysicalQuantities->GetContainerModel(this);
    WidgetUI.quantitiesView->setModel(QuantitiesModel);
    if( QuantitiesModel ){
        for(int i=0; i < QuantitiesModel->columnCount(); i++) {
            WidgetUI.quantitiesView->resizeColumnToContents(i);
        }
    }

    UnitModel = NULL;
    WidgetUI.unitsView->setModel(UnitModel);

    WidgetUI.buttonApply->setEnabled(false);
    ChangeQuantity(QModelIndex());


    // events ------------------------------------
    connect(WidgetUI.quantitiesView, SIGNAL(clicked(const QModelIndex & )),
            SLOT(ChangeQuantity(const QModelIndex & )));

    connect(WidgetUI.unitsView, SIGNAL(clicked( const QModelIndex &)),
            SLOT(ChangeUnit(const QModelIndex &)));

    connect(WidgetUI.buttonNewUnit, SIGNAL(clicked()),
            SLOT(NewUnit()));

    connect(WidgetUI.buttonApply, SIGNAL(clicked()),
            SLOT(ApplyUnit()));

    connect(WidgetUI.buttonActivate, SIGNAL(clicked()),
            SLOT(ActivateUnit()));

    connect(WidgetUI.buttonRemove, SIGNAL(clicked()),
            SLOT(RemoveUnit()));

    connect(WidgetUI.unitNameEdit, SIGNAL(textEdited(const QString &)),
            SLOT(ChangeUnit(const QString &)));
    connect(WidgetUI.unitFormatEdit, SIGNAL(textEdited(const QString &)),
            SLOT(ChangeUnit(const QString &)));
    connect(WidgetUI.decimalsSB, SIGNAL(valueChanged(const QString &)),
            SLOT(ChangeUnit(const QString &)));
    connect(WidgetUI.scaleSpinBox, SIGNAL(valueChanged(const QString &)),
            SLOT(ChangeUnit(const QString &)));
    connect(WidgetUI.offsetSpinBox, SIGNAL(valueChanged(const QString &)),
            SLOT(ChangeUnit(const QString &)));

    connect(WidgetUI.buttonBox,SIGNAL(clicked (QAbstractButton*)),
            this,SLOT(ButtonBoxClicked(QAbstractButton*)));

    // load work panel setup
    LoadWorkPanelSetup();
}

//------------------------------------------------------------------------------

CQuantityUnitSetupWorkPanel::~CQuantityUnitSetupWorkPanel()
{
    SaveWorkPanelSetup();
    p_s5_window = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CQuantityUnitSetupWorkPanel::ButtonBoxClicked(QAbstractButton* p_button)
{
    if( p_button == NULL ) {
        INVALID_ARGUMENT("button == NULL");
    }

    // restore system default setup
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::RestoreDefaults ) {
        WidgetUI.buttonApply->setEnabled(false);
        PhysicalQuantities->LoadSystemConfig();
        ChangeQuantity(QModelIndex());
        return;
    }

    // restore user (or system) setup
    if( WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset ) {
        WidgetUI.buttonApply->setEnabled(false);
        PhysicalQuantities->LoadConfig();
        ChangeQuantity(QModelIndex());
        return;
    }

    // save setup
    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::SaveAll) {
        PhysicalQuantities->SaveUserConfig();
        return;
    }

    // close workpanel
    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CQuantityUnitSetupWorkPanel::ChangeQuantity(const QModelIndex & index)
{
    ApplyOrDiscardQuestion();

    if(index.isValid()) {
        ActualQuantity = index.row();
    } else {
        ActualQuantity = 0;
    }

    WidgetUI.quantitiesView->setCurrentIndex(QuantitiesModel->index(ActualQuantity,0));

    if( UnitModel ){
        delete UnitModel;
    }
    UnitModel = PhysicalQuantities->GetQuantity(ActualQuantity)->GetContainerModel(this);
    WidgetUI.unitsView->setModel(UnitModel);

    for(int i=0; i < UnitModel->columnCount(); i++) {
        WidgetUI.unitsView->resizeColumnToContents(i);
    }

    ChangeUnit(0);
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ChangeUnit(const QModelIndex & index)
{
    if(index.isValid()) {
        ChangeUnit(index.row());
    } else {
        ChangeUnit(0);
    }
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ChangeUnit(int index)
{
    ApplyOrDiscardQuestion();

    ActualUnit = index;
    WidgetUI.unitsView->setCurrentIndex(UnitModel->index(ActualUnit,0));

    CStdPhysicalQuantity* p_quantity = PhysicalQuantities->GetQuantity(ActualQuantity);
    if( p_quantity == NULL ) {
        LOGIC_ERROR("p_quantity is NULL");
    }

    QString       unitName;
    char          format;
    int           decimals;
    bool          active;
    double        offset, scale;
    p_quantity->GetUnit(ActualUnit, unitName, format, decimals, offset, scale, active);

    WidgetUI.unitNameEdit->setText(unitName);
    WidgetUI.unitFormatEdit->setText(QString(format));
    WidgetUI.decimalsSB->setValue(decimals);
    WidgetUI.scaleSpinBox->setValue(scale);
    WidgetUI.offsetSpinBox->setValue(offset);

    if(ActualUnit == 0) {
        WidgetUI.buttonRemove->setEnabled(false);
        WidgetUI.unitNameEdit->setEnabled(false);
        WidgetUI.unitFormatEdit->setEnabled(false);
        WidgetUI.decimalsSB->setEnabled(false);
        WidgetUI.scaleSpinBox->setEnabled(false);
        WidgetUI.offsetSpinBox->setEnabled(false);
    } else {
        WidgetUI.buttonRemove->setEnabled(true);
        WidgetUI.unitNameEdit->setEnabled(true);
        WidgetUI.unitFormatEdit->setEnabled(true);
        WidgetUI.decimalsSB->setEnabled(true);
        WidgetUI.scaleSpinBox->setEnabled(true);
        WidgetUI.offsetSpinBox->setEnabled(true);
    }

    WidgetUI.buttonActivate->setEnabled(!active);
    WidgetUI.buttonApply->setEnabled(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CQuantityUnitSetupWorkPanel::NewUnit(void)
{
    ApplyOrDiscardQuestion();

    CStdPhysicalQuantity* p_quantity = PhysicalQuantities->GetQuantity(ActualQuantity);
    if( p_quantity == NULL ) {
        LOGIC_ERROR("p_quantity is NULL");
    }

    p_quantity->NewUnit("name",'f', 2, 0.0, 1.0,false);
    ChangeUnit(p_quantity->GetNumberOfUnits()-1);

    WidgetUI.unitNameEdit->setFocus(Qt::OtherFocusReason);
    WidgetUI.buttonApply->setEnabled(false);
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ApplyUnit(void)
{
    CStdPhysicalQuantity* p_quantity = PhysicalQuantities->GetQuantity(ActualQuantity);
    if( p_quantity == NULL ) {
        LOGIC_ERROR("p_quantity is NULL");
    }

    char format = 'f';
    if( WidgetUI.unitFormatEdit->text().length() > 0 ){
        format = WidgetUI.unitFormatEdit->text()[0].toLatin1();
    }
    p_quantity->SetUnit(ActualUnit,
                        WidgetUI.unitNameEdit->text(), format,
                        WidgetUI.decimalsSB->value(),  WidgetUI.offsetSpinBox->value(),
                        WidgetUI.scaleSpinBox->value());

    WidgetUI.buttonApply->setEnabled(false);
    ChangeUnit(ActualUnit);
    Projects->RepaintAllProjects();
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ActivateUnit(void)
{
    CStdPhysicalQuantity* p_quantity = PhysicalQuantities->GetQuantity(ActualQuantity);
    if( p_quantity == NULL ){
        LOGIC_ERROR("p_quantity is NULL");
    }

    char format = 'f';
    if( WidgetUI.unitFormatEdit->text().length() > 0 ){
        format = WidgetUI.unitFormatEdit->text()[0].toLatin1();
    }
    p_quantity->SetUnit(ActualUnit,
                        WidgetUI.unitNameEdit->text(), format,
                        WidgetUI.decimalsSB->value(),
                        WidgetUI.offsetSpinBox->value(), WidgetUI.scaleSpinBox->value(),
                        true);
    ChangeUnit(ActualUnit);
    WidgetUI.buttonApply->setEnabled(false);
    Projects->RepaintAllProjects();
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::RemoveUnit(void)
{
    CStdPhysicalQuantity* p_quantity = PhysicalQuantities->GetQuantity(ActualQuantity);
    if( p_quantity == NULL ){
        LOGIC_ERROR("p_quantity is NULL");
    }

    p_quantity->RemoveUnit(ActualUnit);
    ChangeUnit(0);
    Projects->RepaintAllProjects();
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ChangeUnit(const QString& text)
{
    WidgetUI.buttonApply->setEnabled(true);
}

//------------------------------------------------------------------------------

void CQuantityUnitSetupWorkPanel::ApplyOrDiscardQuestion(void)
{
    if( WidgetUI.buttonApply->isEnabled() == false ) return;

    int ret = QMessageBox::question(this, tr("Quantity unit setup"),
                                    tr("The unit values has been modified.\n"
                                       "Do you want to save your changes?"),
                                    QMessageBox::Save | QMessageBox::No,
                                    QMessageBox::Save);
    if(ret == QMessageBox::Save) {
        ApplyUnit();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

