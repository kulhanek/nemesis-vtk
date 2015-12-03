// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
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

#include <PeriodicTableWidget.hpp>
#include <PeriodicTableWidget.moc>

#include <ErrorSystem.hpp>
#include <SmallColor.hpp>
#include <stdio.h>
#include <ElementColors.hpp>
#include <ElementColorsList.hpp>
#include <QAbstractButton>
#include <QGridLayout>
#include <QPushButton>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CPeriodicTableWidget::CPeriodicTableWidget(QWidget* p_parent)
    : QWidget(p_parent)
{
    setWindowTitle(tr("Periodic Table"));

    ActiveElement = -1;

    Label = new QLabel("XX", this);

    ButtonGroup = new QButtonGroup(this);
    ButtonGroup->setExclusive(true);

    GeneratePeriodicTableLayout();

    setContentsMargins(0,0,0,0);
    ChangeElement(1);

    setStyleSheet(
        "QPushButton {"
        "  border: 1px solid #101010;"
        "  min-width: 24px;"
        "  min-height: 24px;"
        "}"
        "QPushButton:checked {"
        "   border: 2px solid #FFFFFF;"
        "}"
    );

}

// -----------------------------------------------------------------------------

CPeriodicTableWidget::~CPeriodicTableWidget(void)
{
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CPeriodicTableWidget::GetActiveElement(void)
{
    return(ActiveElement);
}

//------------------------------------------------------------------------------

void CPeriodicTableWidget::UpdateColors(void)
{
    for(int i = 1; i < 104; i++) {
        QAbstractButton* p_button =  ButtonGroup->button(i);
        CElementColors* p_ecol = ColorsList.GetElementColorPointer(i);
        QString sh;
        sh = "QPushButton { background-color: #%1;}";
        sh = sh.arg(QString(p_ecol->Color.GetRGBHex()));
        p_button->setStyleSheet(sh);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPeriodicTableWidget::GeneratePeriodicTableLayout(void)
{
    QGridLayout* p_layout = new QGridLayout(this);
    p_layout->setContentsMargins(6,6,6,6);
    p_layout->setSpacing(1);

    // add info label widget
    p_layout->addWidget(Label,0,3,3,8,Qt::AlignVCenter|Qt::AlignLeft);

    for(int i = 1; i < 104; i++) {
        QPushButton* p_button = new QPushButton(PeriodicTable.GetSymbol(i), this);

        p_button->setToolTip(QString(PeriodicTable.GetName(i)));
        p_button->setFocusPolicy(Qt::NoFocus);

        QString         sh;
        CElementColors* p_ecol = ColorsList.GetElementColorPointer(i);
        float lum = 0;
        lum += p_ecol->Color.R();
        lum += p_ecol->Color.G();
        lum += p_ecol->Color.B();
        lum /= 3.0;
        if( lum > 0.5 ){
            sh = "QPushButton { background-color: #%1; color: #111111; }";
        } else {
            sh = "QPushButton { background-color: #%1; color: #EEEEEE; }";
        }
        sh = sh.arg(QString(p_ecol->Color.GetRGBHex()));
        p_button->setStyleSheet(sh);

        p_button->setCheckable(true);
        if( i==1 ) {
            p_button->setChecked(true);
        }
        ButtonGroup->addButton(p_button,i);

        int period,group;
        GetPeriodAndGroup(i, period, group);
        p_layout->addWidget(p_button, period, group-1);
    }

    // signals --------------------------------------
    connect(ButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(ChangeElementWithSignal(int)));

    ActiveElement = 1;
}

// -----------------------------------------------------------------------------

void CPeriodicTableWidget::GetPeriodAndGroup(int z, int &period, int &group)
{
    if (z <= 2) {
        period = 0;
        group = (z == 1 ? 1 : 18);
        return;
    }

    if (z <= 18) {
        period = ((z - 3) / 8) + 1;
        if ( z == 3 || z == 4 ) {
            group = z - 2;
        }
        if (5 <= z && z <= 10) {
            group = z + 8;
        }
        if (z == 11 || z == 12) {
            group = z - 10;
        }
        if (13 <= z && z <= 18) {
            group = z;
        }
        return;
    }

    if (z <= 57) {
        period = ((z - 19) / 18) + 3;
        group = (z - 19) % 18 + 1;
        return;
    }

    if (z <= 71) {
        period = 8;
        group = 3 + z - 56;
        return;
    }

    if (z <= 86) {
        period = 5;
        group = 3 + z - 71;
        return;
    }

    if (z <= 89) {
        period = 6;
        group = z - 86;
        return;
    }

    if (z <= 103) {
        period = 9;
        group = 3 + z - 88;
        return;
    } else {
        period = 6;
        group = 3 + z - 103;
        return;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CPeriodicTableWidget::ChangeElementWithSignal(int z)
{
    ChangeElement(z);
    emit OnElementChanged(z);
}

// -----------------------------------------------------------------------------

void CPeriodicTableWidget::ChangeElement(int z)
{
    ActiveElement = z;
    QString text;

    text += "<h2><sub>";
    text += QString::number(z) + "</sub>";
    text += QString(PeriodicTable.GetSymbol(z)) + "</h2>";
    text += QString(PeriodicTable.GetName(z));
    text += "<br>M<sub>A</sub>=" + QString::number(PeriodicTable.GetMass(z));

    Label->setText(text);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
