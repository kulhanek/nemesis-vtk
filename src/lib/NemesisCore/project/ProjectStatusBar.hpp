#ifndef ProjectStatusBarH
#define ProjectStatusBarH
// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2009 Petr Kulhanek, kulhanek@enzim.hu,
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

#include <NemesisCoreMainHeader.hpp>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <Project.hpp>

//------------------------------------------------------------------------------

class NEMESIS_CORE_PACKAGE CProjectStatusBar : public QStatusBar {
    Q_OBJECT
public:
    CProjectStatusBar(QWidget* p_parent,CProject* p_project);

// section of private data -----------------------------------------------------
protected:
    CProject*       Project;
    QFrame*         FlagsSection;
    QFrame*         MouseSection;
    QLabel*         ChangeStatusLabel;
    QLabel*         LockStructureStatusLabel;
    QLabel*         LockTopologyStatusLabel;
    QLabel*         LockGeometryStatusLabel;
    QLabel*         LockGraphicsStatusLabel;
    QLabel*         LockDescriptionStatusLabel;
    QLabel*         LockPropertiesStatusLabel;
    QLabel*         JobStatusLabel;
    QLabel*         MouseModeLabel;
    QLabel*         MouseManipLevelLabel;
    QLabel*         PrimaryDriverLabel;
    QLabel*         SecondaryDriverLabel;
    QLabel*         NotifyLabel;
    QProgressBar*   ProgressBar;

// notifications ---------------------------------------------------------------
    ETextNotificationType       LastTextNotificationType;
    QTimer*                     TextNotificationTimer;

    EProgressNotificationType   ProgressType;
    int                         ProgressMaximum;
    int                         CurrentProgress;
    QString                     ProgressText;

private slots:
    void UpdateStatus(void);
    void UpdateTextNotification(ETextNotificationType type,const QString& text,int time);
    void EndOfTextNotification(void);
    void UpdateProgressNotification(EProgressNotificationType type,int progress,const QString& text);

    friend class CProject;
};

//------------------------------------------------------------------------------

#endif
