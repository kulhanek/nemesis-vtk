// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
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

#include <QMessageBox>

#include <CategoryUUID.hpp>
#include <StaticIndexes.hpp>

#include <FirebirdTransaction.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectStructureListProject.hpp"
#include "CheminfoProjectInfo.hpp"
#include "CheminfoProjectInfo.moc"
#include "CheminfoProjectDatabase.hpp"
#include "CheminfoProjectDatabaseStructure.hpp"


//------------------------------------------------------------------------------

CExtUUID        CheminfoProjectInfoID(
                    "{CHEMINFO_INFO:800eebc3-3803-4d01-83d2-68455fe7be9b}",
                    "Cheminfo Project Info");

CPluginObject   CheminfoProjectInfoObject(&CheminfoProjectPlugin,
                    CheminfoProjectInfoID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/ProjectInfo.svg",
                    NULL);


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfo::CCheminfoProjectInfo(CCheminfoProjectInfoList* p_il)
    : CProObject(&CheminfoProjectInfoObject,p_il,p_il->GetProject())
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(GetProject());
}

//------------------------------------------------------------------------------

CCheminfoProjectInfo::CCheminfoProjectInfo(CCheminfoProjectInfoList *p_il,bool no_index)
    : CProObject(&CheminfoProjectInfoObject,p_il,p_il->GetProject(),no_index)
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(GetProject());
}

//------------------------------------------------------------------------------

CCheminfoProjectInfo::~CCheminfoProjectInfo(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectInfo::SetProjectInfo(const CSmallString &name)
{
//    CFirebirdTransaction trans;
//    Name = name;

//    if(trans.AssignToDatabase(CheminfoProject->GetDatabase()->GetFirebird()) == false) {
//        QMessageBox::critical(NULL,"Error","unable to initiate transaction",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

//    if(trans.StartTransaction() == false) {
//        QMessageBox::critical(NULL,"Error","unable to initiate transaction",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

//    CSmallString sql;
//    sql << "SELECT \"Operator\",\"NDynProps\",\"DynProps\",\"NResults\",\"Description\" FROM \"ROOT\" WHERE \"Name\" = ?";

//  //  sql << "SELECT \"Operator\",\"NDynProps\",\"DynProps\",\"NResults\",\"Description\",\"DB_KEY\" FROM \"ROOT\" WHERE \"Name\" = ?";

//    CFirebirdQuerySQL query;

//    if(query.AssignToTransaction(&trans) == false) {
//        QMessageBox::critical(NULL,"Error","unable to initiate sql statement",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

//    if(query.PrepareQuery(sql) == false) {
//        QMessageBox::critical(NULL,"Error","unable to prepare sql query",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

//    query.GetInputItem(0)->SetString(Name);

//    if(query.ExecuteQueryOnce() == false) {
//        // record not found - project does not exist -> exit
//        QMessageBox::critical(NULL,"Error","project does not exists",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

//    Operator      = query.GetOutputItem(0)->GetTruncatedString();
//    NumberOfProperties = query.GetOutputItem(1)->GetInt();
//    Properties  = query.GetOutputItem(2)->GetTruncatedString();
//    NumberOfResults  = query.GetOutputItem(3)->GetInt();
//    Description     = query.GetOutputItem(4)->GetTruncatedString();
//  //  ID     = query.GetOutputItem(5)->GetInt();
//    if(trans.CommitTransaction() == false) {
//        QMessageBox::critical(NULL,"Error","unable to commit transaction",QMessageBox::Ok,QMessageBox::Ok);
//        return(false);
//    }

    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectInfo::SetProjectInfo(const CSmallString &name, CFirebirdTransaction* trans)
{
    Name = name;

    CSmallString sql;

    sql << "SELECT \"Operator\",\"NDynProps\",\"DynProps\",\"NResults\",\"Description\" FROM \"ROOT\" WHERE \"Name\" = ?";

    CFirebirdQuerySQL query;

    if(query.AssignToTransaction(trans) == false) {
        QMessageBox::critical(NULL,"Error","unable to initiate sql statement",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(query.PrepareQuery(sql) == false) {
        QMessageBox::critical(NULL,"Error","unable to prepare sql query",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    query.GetInputItem(0)->SetString(Name);

    if(query.ExecuteQueryOnce() == false) {
        // record not found - project does not exist -> exit
        QMessageBox::critical(NULL,"Error","project does not exists",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    Operator      = query.GetOutputItem(0)->GetTruncatedString();
    NumberOfProperties = query.GetOutputItem(1)->GetInt();
    Properties  = query.GetOutputItem(2)->GetTruncatedString();
    NumberOfResults  = query.GetOutputItem(3)->GetInt();
    Description     = query.GetOutputItem(4)->GetTruncatedString();

    if(query.CloseQuery() == false) {
        QMessageBox::critical(NULL,"Error","Unable to close sql query!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }
    EmitOnStatusChanged();
    return(true);
}

//------------------------------------------------------------------------------

bool CCheminfoProjectInfo::LoadStructureList(void)
{
    // set it active in project info list
    GetProjectInfoList()->SetActiveProjectInfo(this);

    // create new structure list and  set project info to the structure list
    CCheminfoProjectStructureList* p_strlist = new CCheminfoProjectStructureList(CheminfoProject,this,CheminfoProject->GetStructureListProject());

    // load structures from database
    if (p_strlist->LoadStructureList() == false)
    {
        delete p_strlist;
        return false;
    }
    // set active structure list in to project
    CheminfoProject->GetStructureListProject()->SetActiveStructureList(p_strlist);
    EmitOnStatusChanged();
    return (true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectInfo* CCheminfoProjectInfo::GetProjectInfo(void)
{
    return (this);
}

//------------------------------------------------------------------------------

CCheminfoProjectInfoList* CCheminfoProjectInfo::GetProjectInfoList(void) const
{
    return(static_cast<CCheminfoProjectInfoList*>(parent()));
}

int CCheminfoProjectInfo::GetID(void)
{
    return (ID);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectInfo::GetName(void)
{
        return (Name);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectInfo::GetOperator(void)
{
        return (Operator);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectInfo::GetDescription(void)
{
        return (Description);
}

//------------------------------------------------------------------------------

int CCheminfoProjectInfo::GetNumberOfProperties(void)
{
        return (NumberOfProperties);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectInfo::GetProperties(void)
{
        return (Properties);
}

//------------------------------------------------------------------------------

int CCheminfoProjectInfo::GetNumberOfResults(void)
{
        return (NumberOfResults);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectInfo::EmitOnStatusChanged(void)
{
    emit OnStatusChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
