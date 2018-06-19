// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <Structure.hpp>
#include <StructureList.hpp>
#include <Atom.hpp>
#include <AtomList.hpp>
#include <GraphicsProfileList.hpp>
#include <GLWidget.hpp>
#include <PeriodicTable.hpp>

#include <NemesisCoreModule.hpp>
#include <FreezedAtomsObject.hpp>
#include <FreezedAtomsSetup.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* FreezedAtomsObjectCB(void* p_data);

CExtUUID        FreezedAtomsObjectID(
                    "{FREEZED_ATOMS_OBJECT:d638ed90-b60a-4344-a810-eb55f09125a6}",
                    "Freezed Atoms");

CPluginObject   FreezedAtomsObject(&NemesisCorePlugin,
                     FreezedAtomsObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/FreezedAtoms.svg",
                     FreezedAtomsObjectCB);

// -----------------------------------------------------------------------------

QObject* FreezedAtomsObjectCB(void* p_data)
{
    return(new CFreezedAtomsObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFreezedAtomsObject::CFreezedAtomsObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&FreezedAtomsObject,p_gl,DP_NORMAL_PRIORITY_OBJECT)
{
    RegisterAllowedObjectType(StructureListID);
    RegisterAllowedObjectType(StructureID);
    AddObject(GetProject()->GetStructures());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFreezedAtomsObject::Draw(void)
{
    Setup = GetSetup<CFreezedAtomsSetup>();
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    if( IsFlagSet(EPOF_VISIBLE) == false ) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    glColor4fv(Setup->Color.ForGL());
    glLineWidth(Setup->Width);

    foreach(QObject* p_qobj,Objects) {
        CProObject* p_object = static_cast<CProObject*>(p_qobj);
        CStructureList*  p_sl = dynamic_cast<CStructureList*>(p_object);
        if( p_sl != NULL ){
            DrawStructureList(p_sl);
        }
        CStructure*  p_str = dynamic_cast<CStructure*>(p_object);
        if( p_str != NULL ){
            DrawStructure(p_str);
        }
    }

}

//------------------------------------------------------------------------------

void CFreezedAtomsObject::DrawStructureList(CStructureList* p_sl)
{
    foreach(QObject* p_qobj,p_sl->children()) {
        CStructure*  p_str = static_cast<CStructure*>(p_qobj);
        DrawStructure(p_str);
    }
}

//------------------------------------------------------------------------------

void CFreezedAtomsObject::DrawStructure(CStructure* p_str)
{
    foreach(QObject* p_qobj,p_str->GetAtoms()->children()) {
        CAtom* p_atm = static_cast<CAtom*>(p_qobj);
        DrawAtom(p_atm);
    }
}

//------------------------------------------------------------------------------

void CFreezedAtomsObject::DrawAtom(CAtom* p_atm)
{
    if( p_atm->IsFlagSet(EPOF_FREEZED) == false ) return;
    CPoint  pos = p_atm->GetPos();

    double radius;

    radius = PeriodicTable.GetVdWRadius(p_atm->GetZ());
    radius *= Setup->Ratio;

    CSimplePoint<GLfloat> p1;
    CSimplePoint<GLfloat> p2;
    CSimplePoint<GLfloat> p3;
    CSimplePoint<GLfloat> p4;
    CSimplePoint<GLfloat> p5;
    CSimplePoint<GLfloat> p6;

    p1 = pos;
    p2 = pos;
    p1.x -= radius;
    p2.x += radius;

    p3 = pos;
    p4 = pos;
    p3.y -= radius;
    p4.y += radius;

    p5 = pos;
    p6 = pos;
    p5.z -= radius;
    p6.z += radius;

    glBegin(GL_LINES);
        glVertex3fv(p1);
        glVertex3fv(p2);
        glVertex3fv(p3);
        glVertex3fv(p4);
        glVertex3fv(p5);
        glVertex3fv(p6);

        glVertex3fv(p1);
        glVertex3fv(p3);
        glVertex3fv(p1);
        glVertex3fv(p4);

        glVertex3fv(p2);
        glVertex3fv(p3);
        glVertex3fv(p2);
        glVertex3fv(p4);

        glVertex3fv(p5);
        glVertex3fv(p1);
        glVertex3fv(p5);
        glVertex3fv(p2);
        glVertex3fv(p5);
        glVertex3fv(p3);
        glVertex3fv(p5);
        glVertex3fv(p4);

        glVertex3fv(p6);
        glVertex3fv(p1);
        glVertex3fv(p6);
        glVertex3fv(p2);
        glVertex3fv(p6);
        glVertex3fv(p3);
        glVertex3fv(p6);
        glVertex3fv(p4);
    glEnd();

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

