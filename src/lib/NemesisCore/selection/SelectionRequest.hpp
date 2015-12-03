#ifndef SelectionRequestH
#define SelectionRequestH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010        Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <NemesisCoreMainHeader.hpp>
#include <QObject>
#include <SelectionList.hpp>

// -----------------------------------------------------------------------------

class CSelectionList;
class CSelectionHandler;

// -----------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CSelectionRequest : public QObject {
    Q_OBJECT
public:
// constructors and destructors -----------------------------------------------
    CSelectionRequest(QObject* p_parent,const QString& requestor_name);
    ~CSelectionRequest(void);

    /// set selection request
    void SetRequest(CSelectionList* p_list,CSelectionHandler* p_request = NULL,
                    const QString& reason = QString());

    /// get selection list
    CSelectionList* GetSelectionList(void);

    /// get request type
    CSelectionHandler* GetRequestType(void);

    /// get request reason
    const QString GetRequestReason(void);

    /// get requestor name
    const QString GetRequestorName(void);

    /// is completed?
    bool IsCompleted(void);

    /// is attached?
    bool IsAttached(void);

// signals --------------------------------------------------------------------
signals:
    /// emmited when selection is initialized (after selection list reset, request change)
    void OnSelectionInitialized(void);

    /// emmited when selection list was changed
    void OnSelectionChanged(void);

    /// emmited when selection request is fullfield
    void OnSelectionCompleted(void);

    /// emmited when request is detached from the selection
    void OnDetached(void);

    /// emmited before request is changed
    void OnRequestChanging(void);

// section of private data ----------------------------------------------------
private:
    QString             RequestorName;
    CSelectionList*     List;
    CSelectionHandler*  Request;
    QString             Reason;

    void SelectionListStatusChanged(ESelStatus change);

    friend class CSelectionList;
};

// -----------------------------------------------------------------------------

#endif
