// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CategoryUUID.hpp>
#include <DesktopSystem.hpp>
#include <ProjectList.hpp>
#include <Project.hpp>
#include <ContainerModel.hpp>

#include <SelectionList.hpp>

#include <SelBondOrderDialog.hpp>
#include <NemesisCoreModule.hpp>
#include <QDialog>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        SelBondOrderDialogID(
                    "{SEL_BOND_ORDER_WORK_PANEL:9574a4f8-43c2-4586-8103-f8ee154dfa83}",
                    "Select Bond Order Dialog");

CPluginObject   SelBondOrderDialogObject(&NemesisCorePlugin,
                    SelBondOrderDialogID,WORK_PANEL_CAT,
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CSelBondOrderDialog::CSelBondOrderDialog(const QString& title,CProject* p_project)
    : CWorkPanel(&SelBondOrderDialogObject,p_project,EWPR_DIALOG)
{
    // set up ui file
    WidgetUI.setupUi(this);

    Order = BO_SINGLE;
    Operator = ECO_EQ;

    // populate combo box
    WidgetUI.orderCB->addItem("single");
    WidgetUI.orderCB->addItem("single and half");
    WidgetUI.orderCB->addItem("double");
    WidgetUI.orderCB->addItem("double and half");
    WidgetUI.orderCB->addItem("triple");
    WidgetUI.orderCB->addItem("weak");

    WidgetUI.operatorCB->addItem("==");
    WidgetUI.operatorCB->addItem("<");
    WidgetUI.operatorCB->addItem("<=");
    WidgetUI.operatorCB->addItem(">");
    WidgetUI.operatorCB->addItem(">=");
    WidgetUI.operatorCB->addItem("!=");

    // local events
    connect(WidgetUI.buttonBox, SIGNAL(clicked (QAbstractButton *)),
            SLOT(ButtonBoxClicked(QAbstractButton *)));

    // load work panel setup
    LoadWorkPanelSetup();
}

// -----------------------------------------------------------------------------

CSelBondOrderDialog::~CSelBondOrderDialog(void)
{
    SaveWorkPanelSetup();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EBondOrder  CSelBondOrderDialog::GetBondOrder(void)
{
    return(Order);
}

//------------------------------------------------------------------------------

EComparisonOperator CSelBondOrderDialog::GetOperator(void)
{
    return(Operator);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CSelBondOrderDialog::ButtonBoxClicked(QAbstractButton *button)
{
    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Ok ) {

        switch(WidgetUI.orderCB->currentIndex()){
            case 0:
                Order = BO_SINGLE;
                break;
            case 1:
                Order = BO_SINGLE_H;
                break;
            case 2:
                Order = BO_DOUBLE;
                break;
            case 3:
                Order = BO_DOUBLE_H;
                break;
            case 4:
                Order = BO_TRIPLE;
                break;
            case 5:
                Order = BO_WEAK;
                break;
        }

        switch(WidgetUI.operatorCB->currentIndex()){
            case 0:
                Operator = ECO_EQ;
                break;
            case 1:
                Operator = ECO_LT;
                break;
            case 2:
                Operator = ECO_LE;
                break;
            case 3:
                Operator = ECO_GT;
                break;
            case 4:
                Operator = ECO_GE;
                break;
            case 5:
                Operator = ECO_NE;
                break;
        }

        SetDialogResult(QDialog::Accepted);
        close();
        return;
    }

    if( WidgetUI.buttonBox->standardButton(button) == QDialogButtonBox::Cancel ) {
        SetDialogResult(QDialog::Rejected);
        close();
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



