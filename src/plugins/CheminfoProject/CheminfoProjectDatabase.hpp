#ifndef CheminfoProjectDatabaseH
#define CheminfoProjectDatabaseH
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

#include <ProObject.hpp>

#include <FirebirdDatabase.hpp>

#include <SmallString.hpp>

//------------------------------------------------------------------------------
enum TypeDBMS {
    FIREBIRD
};

class CCheminfoProjectDatabase : public CProObject {
    Q_OBJECT
public:
    // constructors and destructors -----------------------------------------------
    /// Constructor
    CCheminfoProjectDatabase(CProject *p_project);
    /// Destructor
    ~CCheminfoProjectDatabase(void);

    // public API to Firebird
    CFirebirdDatabase* GetFirebird(void);

    // section of public data ----------------------------------------------------
    /// Check init values from user
    bool         AreInitValuesFill(void);

    /// Check if is database open
    bool         IsLogged(void);

    /// Return information about database
    CSmallString GetDatabaseName(void);
    CSmallString GetDatabaseUserName(void);
    CSmallString GetDatabasePassword(void);
    TypeDBMS     GetDatabaseType(void);

    /// Set information about database
    void SetDatabaseName(CSmallString dbname);
    void SetDatabaseUserName(CSmallString dbuser);
    void SetDatabasePassword(CSmallString dbpassword);
    void SetDatabaseType(TypeDBMS dbtype);

    /// Save database setting in to the settings file
    bool SaveDatabaseSettings(void);
    /// Load database setting from the settings file
    bool LoadDatabaseSettings(void);

public:
    // Firebird ------------------------------------------------------------------
    
    /// Connect to Firebird Database
    void ConnectDatabaseFirebird (const CSmallString& dbname, const CSmallString& user,const CSmallString& password);
    /// Connect to Firebird Database when init values was filled by public API
    void ConnectDatabaseFirebird (void);

    /// Disconnect from Firebird Database
    void DisconnectDatabaseFirebird (void);

    /// emit OnDatabaseConnectChanged signal
    void EmitOnDatabaseConnectChanged(void);

signals:
    void OnDatabaseConnectChanged(void);

    // section of private data ----------------------------------------------------
private:
    CSmallString    DBName;
    CSmallString    DBUser;
    CSmallString    DBPassword;
    TypeDBMS        DBType;
    CFirebirdDatabase* FBdb;

};

//------------------------------------------------------------------------------
#endif
