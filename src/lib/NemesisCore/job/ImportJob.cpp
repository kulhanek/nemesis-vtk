// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program sin free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program sin distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <ImportJob.hpp>
#include <RecentFileList.hpp>
#include <Graphics.hpp>
#include <Structure.hpp>
#include <Project.hpp>
#include <AtomList.hpp>
#include <GraphicsProfileList.hpp>
#include <GraphicsProfile.hpp>
#include <GraphicsObjectList.hpp>
#include <GraphicsObject.hpp>
#include <GraphicsView.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CImportJob::CImportJob(CPluginObject* p_objectinfo,CProject* p_project)
    : CJob(p_objectinfo,p_project)
{
    Structure = NULL;
}

//------------------------------------------------------------------------------

bool CImportJob::SetImportedStructure(CStructure* p_mol,const QString& name)
{
    Structure = p_mol;
    FileName = name;
    return(true);
}

//------------------------------------------------------------------------------

void CImportJob::PushToRecentFiles(void)
{
    if( GetJobStatus() == EJS_FINISHED ){
        RecentFiles->PushFile(FileName,GetType());
    }
}

//------------------------------------------------------------------------------

void CImportJob::AdjustGraphics(void)
{
    CProject*  p_project = Structure->GetProject();
    CGraphics* p_grp = p_project->GetGraphics();

    if( Structure->GetAtoms()->GetNumberOfAtoms() > 500 ){
        // try to change primary model from standard to fast line

        CHistoryNode* History = p_project->BeginChangeWH(EHCL_GRAPHICS,"graphics");
        if( History != NULL ){

            CExtUUID objectuuid;
            objectuuid.LoadFromString("{STANDARD_MODEL_OBJECT:b8ec4377-b886-44c3-a206-4d40eaf7d5f2}");

            CGraphicsObject* p_stdmodel = p_grp->GetObjects()->GetPrimaryObject(objectuuid);
            if( p_stdmodel ){
                // hide primary standard model
                if( p_stdmodel->IsFlagSet(EPOF_VISIBLE) ){
                    p_stdmodel->ToggleShowHideFlagWH();
                }
            }

            objectuuid.LoadFromString("{FAST_MODEL_OBJECT:172377a6-4faf-4803-8245-eb07eb6a459f}");
            CGraphicsObject* p_fastmodel = p_grp->GetObjects()->GetPrimaryObject(objectuuid);
            CGraphicsProfile* p_profile = p_grp->GetProfiles()->GetActiveProfile();

            if( (p_fastmodel == NULL) && (p_profile != NULL) ){
                // create fast model
                CGraphicsObject* p_obj = p_grp->GetObjects()->CreateObject(objectuuid);
                if( p_obj == NULL ) {
                    CSmallString error;
                    error << "unable to create object " << objectuuid.GetStringForm();
                    RUNTIME_ERROR(error);
                }
                if( p_profile->AddObject(p_obj) == NULL) {
                    delete p_obj;
                    CSmallString error;
                    error << "unable to add object " << objectuuid.GetStringForm() << " into a profile";
                    RUNTIME_ERROR(error);
                }
                p_obj->AddObjectWH(Structure);
            }

        p_project->EndChangeWH();
        }
    }

    //! autofit scene
    p_project->GetGraphics()->GetPrimaryView()->FitScene(false);
}

//------------------------------------------------------------------------------

void CImportJob::AddGraphicModel(bool adjust_graphic)
{
    CProject*  p_project = Structure->GetProject();
    CGraphics* p_grp = p_project->GetGraphics();

    CHistoryNode* History = p_project->BeginChangeWH(EHCL_GRAPHICS,"graphics");
        if( History != NULL ){

            CExtUUID objectuuid;
            objectuuid.LoadFromString("{STANDARD_MODEL_OBJECT:b8ec4377-b886-44c3-a206-4d40eaf7d5f2}");

            CGraphicsProfile* p_profile = p_grp->GetProfiles()->GetActiveProfile();

            if( p_profile != NULL ){
                QString model_name = Structure->GetName() + " Standard Model";
                // create standard model
                CGraphicsObject* p_obj = p_grp->GetObjects()->CreateObject(objectuuid, model_name);
                if( p_obj == NULL ) {
                    CSmallString error;
                    error << "unable to create object " << objectuuid.GetStringForm();
                    RUNTIME_ERROR(error);
                }
                if( p_profile->AddObject(p_obj) == NULL) {
                    delete p_obj;
                    CSmallString error;
                    error << "unable to add object " << objectuuid.GetStringForm() << " into a profile";
                    RUNTIME_ERROR(error);
                }
                p_obj->AddObjectWH(Structure);
            }

        p_project->EndChangeWH();
        }

    if (adjust_graphic) {
        //! autofit scene
        p_project->GetGraphics()->GetPrimaryView()->FitScene(Structure,false);
    } else {
        p_project->GetGraphics()->GetPrimaryView()->FitScene(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
