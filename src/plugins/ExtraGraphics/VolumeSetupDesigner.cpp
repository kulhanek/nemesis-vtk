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

#include <PluginObject.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <Project.hpp>
#include <ProjectList.hpp>
#include <PhysicalQuantities.hpp>
#include <PhysicalQuantity.hpp>

#include <QColorDialog>

#include <NemesisCoreModule.hpp>
#include <FreezedAtomsSetup.hpp>
#include <FreezedAtomsSetupDesigner.hpp>

#include "FreezedAtomsSetupDesigner.moc"


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FreezedAtomsSetupDesignerCB(void* p_data);

CExtUUID        FreezedAtomsSetupDesignerID(
                    "{FREEZED_ATOM_SETUP_DESIGNER:e4e2f3b8-e9f6-4d7c-8749-30a834752c70}",
                    "Freezed Atoms");

CPluginObject   FreezedAtomsSetupDesignerObject(&NemesisCorePlugin,
                    FreezedAtomsSetupDesignerID,DESIGNER_CAT,
                    ":/images/NemesisCore/graphics/FreezedAtoms.svg",
                    FreezedAtomsSetupDesignerCB);

// -----------------------------------------------------------------------------

QObject* FreezedAtomsSetupDesignerCB(void* p_data)
{
    CFreezedAtomsSetup* p_setup = static_cast<CFreezedAtomsSetup*>(p_data);
    QObject* p_object = new CFreezedAtomsSetupDesigner(p_setup);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFreezedAtomsSetupDesigner::CFreezedAtomsSetupDesigner(CFreezedAtomsSetup* p_setup)
    : CProObjectDesigner(&FreezedAtomsSetupDesignerObject,p_setup)
{
    FreezedAtomsSetup = p_setup;
    WidgetUI.setupUi(this);

    QButtonGroup* bg = new QButtonGroup(this);
    bg->addButton(WidgetUI.colorTB);

    connect(bg, SIGNAL(buttonClicked(QAbstractButton *)),
            SLOT(GetColorFromUser(QAbstractButton *)));

    // signals
    connect(WidgetUI.projectPB, SIGNAL(clicked()),
            SLOT(OpenProjectDesigner()));
    connect(WidgetUI.objectPB, SIGNAL(clicked()),
            SLOT(OpenObjectDesigner()));

    // data changes
    connect(WidgetUI.nameLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.descriptionLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.widthSB, SIGNAL(valueChanged(int)),
            this, SLOT(SetChangedFlagTrue()));
    connect(WidgetUI.ratioSB, SIGNAL(valueChanged(double)),
            this, SLOT(SetChangedFlagTrue()));

    connect(WidgetUI.buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(ButtonBoxClicked(QAbstractButton*)));

    connect(PQ_DISTANCE,SIGNAL(OnUnitChanged()),
            this,SLOT(InitValues()));

    // populate designer with data
    InitValues();
}

//------------------------------------------------------------------------------

CFreezedAtomsSetupDesigner::~CFreezedAtomsSetupDesigner()
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::ButtonBoxClicked(QAbstractButton* p_button)
{
    if(p_button == NULL) {
        INVALID_ARGUMENT("p_button == NULL");
    }

    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Reset) {
        InitValues();
    }

    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Close) {
        close();
    }

    if(WidgetUI.buttonBox->standardButton(p_button) == QDialogButtonBox::Apply) {
        SetValues();
        Projects->RepaintAllProjects();
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::InitValues(void)
{
    // general section
    CProject* p_proj = FreezedAtomsSetup->GetProject();
    if( p_proj != NULL ) {
        WidgetUI.projectPB->setText(p_proj->GetName());

        CProObject* p_object = NULL;
        if( FreezedAtomsSetup->GetObjectID() >= 0 ) {
            p_object = p_proj->FindObject(FreezedAtomsSetup->GetObjectID());
        }
        if( p_object != NULL ) {
            WidgetUI.objectPB->setText(p_object->GetName());
        } else {
            WidgetUI.label2->hide();
            WidgetUI.objectPB->hide();
        }
    } else {
        WidgetUI.label1->hide();
        WidgetUI.projectPB->hide();
        WidgetUI.label2->hide();
        WidgetUI.objectPB->hide();
        WidgetUI.nameLE->setEnabled(false);
    }

    WidgetUI.nameLE->setText(QString(FreezedAtomsSetup->GetName()));
    WidgetUI.descriptionLE->setText(QString(FreezedAtomsSetup->GetDescription()));


    QString ratio_quantity_name = PQ_PERCENTAGE->GetUnitName();
    WidgetUI.ratioSB->setSuffix(" " + ratio_quantity_name);

    // object section
    QString         sht;
    QString         sh;
    sht = "QPushButton { background-color: #%1;}";

    sh = sht.arg(QString(FreezedAtomsSetup->Color.GetRGBHex()));
    WidgetUI.colorTB->setStyleSheet(sh);
    Color = QColor(FreezedAtomsSetup->Color.R()*255,FreezedAtomsSetup->Color.G()*255,
                    FreezedAtomsSetup->Color.B()*255,FreezedAtomsSetup->Color.Alpha()*255);
    WidgetUI.widthSB->setValue(FreezedAtomsSetup->Width);
    WidgetUI.ratioSB->setValue(PQ_PERCENTAGE->GetRealValue(FreezedAtomsSetup->Ratio));

    SetChangedFlag(false);
}

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::SetValues(void)
{
    if( IsChangedFlagSet() == false ) return;

    FreezedAtomsSetup->SetName(WidgetUI.nameLE->text());
    FreezedAtomsSetup->SetDescription(WidgetUI.descriptionLE->text());

    FreezedAtomsSetup->Color = CColor(Color.redF(), Color.greenF(),
                                    Color.blueF(), Color.alphaF());
    FreezedAtomsSetup->Width = WidgetUI.widthSB->value();
    FreezedAtomsSetup->Ratio = PQ_PERCENTAGE->GetInternalValue(WidgetUI.ratioSB->value());

    // some changes can be prohibited - reinit visualization
    InitValues();

    // update objects and repaint all projects
    FreezedAtomsSetup->EmitOnSetupUpdated();
    Projects->RepaintAllProjects();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::OpenProjectDesigner(void)
{
    CProject* p_proj = FreezedAtomsSetup->GetProject();
    if( p_proj == NULL ) return;
    p_proj->OpenObjectDesigner();
}

//------------------------------------------------------------------------------

void CFreezedAtomsSetupDesigner::OpenObjectDesigner(void)
{
    CProject* p_proj = FreezedAtomsSetup->GetProject();
    if( p_proj == NULL ) return;

    CProObject* p_object = NULL;
    if( FreezedAtomsSetup->GetObjectID() >= 0 ) {
        p_object = p_proj->FindObject(FreezedAtomsSetup->GetObjectID());
    }
    if( p_object == NULL ) return;

    p_object->OpenObjectDesigner();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsSetupDesigner::GetColorFromUser(QAbstractButton* p_button)
{
    if( p_button == NULL ) return;

    QColor color;
    if( p_button == WidgetUI.colorTB ) {
        color = Color;
    }
    color = QColorDialog::getRgba(color.rgb());
    if( color.isValid() == false ) return;

    if( p_button == WidgetUI.colorTB ) {
        Color = color;
    }

    QString         sht;
    QString         sh;
    sht = "QPushButton { background-color: #%1;}";

    CColor ncolor  = CColor(color.redF(),color.greenF(),color.blueF(),color.alphaF());

    sh = sht.arg(QString(ncolor.GetRGBHex()));
    p_button->setStyleSheet(sh);

    SetChangedFlag(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


