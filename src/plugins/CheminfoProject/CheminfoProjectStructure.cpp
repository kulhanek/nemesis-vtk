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

#include <FirebirdTransaction.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

#include "CheminfoProjectModule.hpp"
#include "CheminfoProjectStructureList.hpp"
#include "CheminfoProjectStructure.hpp"
#include "CheminfoProjectStructure.moc"

//------------------------------------------------------------------------------

CExtUUID        CheminfoProjectStructureID(
                    "{CHEMINFO_STRUCTURE:6d62c711-1dec-40bd-ba51-3a3b4002ebc5}",
                    "Cheminfo Project Structure");

CPluginObject   CheminfoProjectStructureObject(&CheminfoProjectPlugin,
                    CheminfoProjectStructureID,CHEMINFO_CAT,
                    ":/images/CheminfoProject/12.cheminfo/Structure.svg",
                    NULL);


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructure::CCheminfoProjectStructure(CCheminfoProjectStructureList* p_il)
    : CProObject(&CheminfoProjectStructureObject,p_il,p_il->GetProject())
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(GetProject());
    ProjectInfo = p_il->GetProjectInfo();
}

//------------------------------------------------------------------------------

CCheminfoProjectStructure::CCheminfoProjectStructure(CCheminfoProjectStructureList *p_il,bool no_index)
    : CProObject(&CheminfoProjectStructureObject,p_il,p_il->GetProject(),no_index)
{
    CheminfoProject = dynamic_cast<CCheminfoProject*>(GetProject());
    ProjectInfo = p_il->GetProjectInfo();
}

//------------------------------------------------------------------------------

CCheminfoProjectStructure::~CCheminfoProjectStructure(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCheminfoProjectStructure::LoadStructure(const CSmallString &name)
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
//    sql << "SELECT \"ID\",\"Flag\",\"NA\",\"TC\",\"MW\",\"R1\" FROM \"?\" WHERE \"?\" = ?";

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
//    query.GetInputItem(0)->SetString(Property);

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

bool CCheminfoProjectStructure::LoadStructure(const CSmallString &id, CFirebirdTransaction* trans)
{
    ID = id.ToInt();

    CSmallString sql;

    sql << "SELECT \"ID\",\"Flag\",\"InChiKey\",\"NA\",\"TC\",\"MW\"";

    // results
    for(int i=0; i < this->GetProjectInfo()->GetNumberOfResults(); i++) {
        sql << ",\"R" << i+1 << "\"";
    }

    sql << " FROM \"" << this->GetProjectInfo()->GetName() << "\" WHERE \"ID\" = " << id;

//    if(Filter != NULL) {
//        sql << " AND ( " << Filter << " )";
//    }

    CFirebirdQuerySQL query;

    if(query.AssignToTransaction(trans) == false) {
        QMessageBox::critical(NULL,"Error","unable to initiate sql statement",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(query.PrepareQuery(sql) == false) {
        QMessageBox::critical(NULL,"Error","unable to prepare sql query",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    if(query.ExecuteQueryOnce() == false) {
        // record not found - project does not exist -> exit
        QMessageBox::critical(NULL,"Error","project does not exists",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }

    ID             = query.GetOutputItem(0)->GetTruncatedString();
    Flag           = query.GetOutputItem(1)->GetInt();
    InChiKey       = query.GetOutputItem(2)->GetTruncatedString();
    NumberOfAtoms  = query.GetOutputItem(3)->GetInt();
    PartialCharge  = query.GetOutputItem(4)->GetInt();
    MolarMass      = query.GetOutputItem(5)->GetDouble();
    // results
    for(int i=0; i < this->GetProjectInfo()->GetNumberOfResults(); i++) {
         Results.push_back( query.GetOutputItem(6+i)->GetDouble());
    }

    if(query.CloseQuery() == false) {
        QMessageBox::critical(NULL,"Error","Unable to close sql query!",QMessageBox::Ok,QMessageBox::Ok);
        return(false);
    }
    EmitOnStatusChanged();
    return(true);
}

//------------------------------------------------------------------------------

void CCheminfoProjectStructure::SetProjectInfo(CCheminfoProjectInfo* p_info)
{
    ProjectInfo = p_info;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCheminfoProjectStructure* CCheminfoProjectStructure::GetStructure(void)
{
    return (this);
}

//------------------------------------------------------------------------------

CCheminfoProjectStructureList* CCheminfoProjectStructure::GetStructureList(void) const
{
    return(static_cast<CCheminfoProjectStructureList*>(parent()));
}

//------------------------------------------------------------------------------

CCheminfoProjectInfo* CCheminfoProjectStructure::GetProjectInfo(void) const
{
    return(ProjectInfo);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectStructure::GetID(void)
{
    return (ID);
}

//------------------------------------------------------------------------------

int CCheminfoProjectStructure::GetFlag(void)
{
        return (Flag);
}

//------------------------------------------------------------------------------

CSmallString CCheminfoProjectStructure::GetInChiKey(void)
{
        return (InChiKey);
}

//------------------------------------------------------------------------------

int CCheminfoProjectStructure::GetNumberOfAtoms(void)
{
        return (NumberOfAtoms);
}

//------------------------------------------------------------------------------

int CCheminfoProjectStructure::GetPartialCharge(void)
{
        return (PartialCharge);
}

//------------------------------------------------------------------------------

double CCheminfoProjectStructure::GetMolarMass(void)
{
        return (MolarMass);
}

//------------------------------------------------------------------------------

vector<double>* CCheminfoProjectStructure::GetResults(void)
{
        return (&Results);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CCheminfoProjectStructure::EmitOnStatusChanged(void)
{
    emit OnStatusChanged();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
