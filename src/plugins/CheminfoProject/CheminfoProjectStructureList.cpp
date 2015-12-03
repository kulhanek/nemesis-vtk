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

#include <QMessageBox>

#include <XMLElement.hpp>
#include <ErrorSystem.hpp>
#include <CategoryUUID.hpp>
#include <Project.hpp>
#include <Graphics.hpp>
#include <GraphicsProfileList.hpp>

#include <FirebirdTransaction.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

#include "CheminfoProjectModule.hpp"

#include "CheminfoProjectStructureListProject.hpp"

#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectStructureList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        CheminfoProjectStructureListID(
                    "{CHEMINFO_STRUCTURE_LIST:f05da743-ea9a-48ad-9c9e-7ded9c805580}",
                    "Cheminfo Project Structure List");

CPluginObject   CheminfoProjectStructureListObject(&CheminfoProjectPlugin,
                    CheminfoProjectStructureListID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/CheminfoProjectStructureList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructureList::CCheminfoProjectStructureList(CProject* p_project,
                                                CCheminfoProjectInfo* p_info, CCheminfoProjectStructureListProject *p_listproj)
    : CProObject(&CheminfoProjectStructureListObject,p_info,p_info->GetProject())
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);
    StructureListProject = p_listproj;
    ProjectInfo = p_info;

}

//------------------------------------------------------------------------------

CCheminfoProjectStructureList::~CCheminfoProjectStructureList(void)
{
    foreach(QObject* p_qobj,children()) {
        delete p_qobj;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectStructureList::LoadStructureList (void)
{
    CFirebirdTransaction trans;

    if( !CheminfoProject->GetDatabase()->IsLogged() ) {
        QMessageBox::critical(NULL,"Error","Database is not connected!\n Connect to database first.",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(trans.AssignToDatabase(CheminfoProject->GetDatabase()->GetFirebird()) == false) {
        QMessageBox::critical(NULL,"Error","unable to initiate transaction",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(trans.StartTransaction() == false) {
        QMessageBox::critical(NULL,"Error","unable to initiate transaction",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    CSmallString sql;

    sql << "SELECT \"ID\" FROM \"" << this->GetProjectInfo()->GetName() <<"\"";

    CFirebirdQuerySQL query;

    if(query.AssignToTransaction(&trans) == false) {
        QMessageBox::critical(NULL,"Error","unable to initiate sql statement",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(query.OpenQuery(sql) == false) {
        QMessageBox::critical(NULL,"Error","unable to prepare sql query",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    while(query.QueryRecord() == true) {
        CSmallString id = query.GetOutputItem(0)->GetTruncatedString();
        CCheminfoProjectStructure* p_cheminfo = new CCheminfoProjectStructure(this);
        p_cheminfo->LoadStructure(id,&trans);
    }

    if(query.CloseQuery() == false) {
        QMessageBox::critical(NULL,"Error","Unable to close sql query!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(trans.CommitTransaction() == false) {
        QMessageBox::critical(NULL,"Error","unable to commit transaction",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }
    EmitOnCheminfoProjectStructureListChanged();
    StructureListProject->EmitOnCheminfoProjectStructureListProjectChanged();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfo* CCheminfoProjectStructureList::GetProjectInfo(void) const
{
    return (ProjectInfo);
}

//------------------------------------------------------------------------------

void  CCheminfoProjectStructureList::SetProjectInfo(CCheminfoProjectInfo* p_info)
{
    ProjectInfo = p_info;
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructureList::EmitOnCheminfoProjectStructureListChanged(void)
{
    //if( UpdateLevel > 0 ) Changed = true;
    emit OnCheminfoProjectStructureListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

