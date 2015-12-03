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

#include "CheminfoProjectInfoList.hpp"
#include "CheminfoProjectInfoList.moc"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CExtUUID        CheminfoProjectInfoListID(
                    "{CHEMINFO_INFO_LIST:b169b023-ffd4-4afe-acc3-f061de216f74}",
                    "Cheminfo Projects List");

CPluginObject   CheminfoProjectInfoListObject(&CheminfoProjectPlugin,
                    CheminfoProjectInfoListID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/CheminfoProjectInfoList.svg",
                    NULL);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfoList::CCheminfoProjectInfoList(CProject* p_project)
    : CProObject(&CheminfoProjectInfoListObject,p_project,p_project)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);

}

//------------------------------------------------------------------------------

CCheminfoProjectInfoList::CCheminfoProjectInfoList(CProject* p_project,bool no_index)
    : CProObject(&CheminfoProjectInfoListObject,p_project,p_project,no_index)
{
    ConFlags |= EECOF_SUB_CONTAINERS;
    ConFlags |= EECOF_DYNAMICAL_CONTENTS;
    CheminfoProject = dynamic_cast<CCheminfoProject*>(p_project);

}

//------------------------------------------------------------------------------

CCheminfoProjectInfoList::~CCheminfoProjectInfoList(void)
{
    foreach(QObject* p_qobj,children()) {
        delete p_qobj;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectInfoList::LoadProjectInfoList (void)
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

    sql << "SELECT \"Name\" FROM \"ROOT\"";

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
        CSmallString name = query.GetOutputItem(0)->GetTruncatedString();
        CCheminfoProjectInfo* p_cheminfo = new CCheminfoProjectInfo(CheminfoProject->GetProjectInfoList());

             p_cheminfo->SetProjectInfo(name,&trans);
    }

    if(query.CloseQuery() == false) {
        QMessageBox::critical(NULL,"Error","Unable to close sql query!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(trans.CommitTransaction() == false) {
        QMessageBox::critical(NULL,"Error","unable to commit transaction",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }
    EmitOnCheminfoProjectInfoListChanged();
    return(true);
}

//------------------------------------------------------------------------------

void  CCheminfoProjectInfoList::SetActiveProjectInfo(CCheminfoProjectInfo* p_info)
{
    ActiveProject = p_info;
}
CCheminfoProjectInfo*  CCheminfoProjectInfoList::GetActiveProjectInfo(void) const
{
    return (ActiveProject);
}

void CCheminfoProjectInfoList::EmitOnCheminfoProjectInfoListChanged(void)
{
    emit OnCheminfoProjectInfoListChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

