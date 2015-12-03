// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu,
//                       Jakub Stepan, xstepan3@chemi.muni.cz
//    Copyright (C) 1998-2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <stdlib.h>
#include <unistd.h>

#include <QString>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

#include <Project.hpp>
#include <ProjectList.hpp>
#include <ExtUUID.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GlobalSetup.hpp>
#include <MainWindow.hpp>
#include <JobList.hpp>
#include <FileSystem.hpp>

#include <Structure.hpp>
#include <StructureList.hpp>
#include <Trajectory.hpp>
#include <TrajectoryList.hpp>

#include "DockingProjectModule.hpp"
#include "DockingProjectDockVina.hpp"
#include "DockingProjectDockVina.moc"


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* DockingProjectDockVinaCB(void* p_data);

CExtUUID        DockingProjectDockVinaID(
                    "{DOCKING_PROJECT_DOCK_VINA:53cd67b0-3702-4477-94f0-c217f1b98aa7}",
                    "Docking Project module for Dock");
CPluginObject   DockingProjectDockVinaObject(&DockingProjectPlugin,
                    DockingProjectDockVinaID,JOB_CAT,
                    ":/images/DockingProject/13.docking/Dock.svg",
                    DockingProjectDockVinaCB);

// -----------------------------------------------------------------------------

QObject* DockingProjectDockVinaCB(void* p_data)
{
    CProject* p_proj = static_cast<CProject*>(p_data);
    QObject* p_object = new CDockingProjectDockVina(p_proj);
    return(p_object);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDockingProjectDockVina::CDockingProjectDockVina(CProject* p_project)
    : CDockingProjectDock(&DockingProjectDockVinaObject,p_project)
{
    Tick = 0;
}

//------------------------------------------------------------------------------

CDockingProjectDockVina::~CDockingProjectDockVina(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CDockingProjectDockVina::Initialization(void)
{
    //--------
    connect(this,SIGNAL(OnStartDock(int)),
            this,SLOT(StartDockingProgress(int)));
    connect(this,SIGNAL(OnInitDock(int)),
            this,SLOT(SetInitProgress(int)));
    connect(this,SIGNAL(OnProgressDock(int,bool)),
            this,SLOT(SetDockingProgress(int,bool)));

    // Vina has 51 aterisk '*' and one is about 2 %
    emit OnStartDock(52);
    Tick++;
    emit OnInitDock(Tick);
    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDockVina::Docking(void)
{
    emit OnProgressDock(Tick);
    // Run dock vina and read
    FILE *fp;
    QString line, last_line;
    std::string progress;
    int MAX_BUFFER = 2;
    char buffer[MAX_BUFFER];

    QProcess vina_check;
    vina_check.setProcessChannelMode(QProcess::MergedChannels);
    QString cmd_chk_vina ("vina -h");
    vina_check.start(cmd_chk_vina);

    if (!vina_check.waitForFinished()) {
        CSmallString error;
        error << tr("issue during docking progress: Autodock vina is not installed on system! (Try 'module add autodock-vina or install it').");
        DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return (false);
    }
    if ( (vina_check.exitCode() != 0) && (vina_check.exitCode() != 1) ) {
        CSmallString error;
        error << tr("issue during docking progress: Autodock vina is not installed on system! (Try 'module add autodock-vina or install it').");
        DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return (false);
    }

    // at first change dir to compute algoritm in right folder
    CSmallString vina_cmd = "cd \"";
    vina_cmd += ComputePath;
    vina_cmd += "\" && ";
    // add vina command for compute with generated config file
    vina_cmd += "vina --config \"";
    vina_cmd += ConfigFile;
    vina_cmd += "\" 2>&1";

    // run docking algorithm as open the command for reading
    fp = popen(vina_cmd.GetBuffer(), "r");
    if (fp == NULL) {
        return (false);
    }
    CFileName log_file =  ComputePath/"log.txt";
    QFile log(log_file.GetBuffer());
    if (SaveLogOutput) {
        // open log file
        if( ! log.open(QIODevice::WriteOnly | QIODevice::Text) ) {
            CSmallString error;
            error << tr("issue during docking progress: Cannot open logging file.");
            DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return false;
        }
    }
    // write log to file
    QTextStream logOutput(&log);

    // parse output of Vina program
    while (fgets(buffer, MAX_BUFFER, fp) != NULL ) {
        progress.append(buffer);

        if (buffer[0] == '\n') {
            last_line = line;
            if (SaveLogOutput) {
                logOutput << line;
            }
            line.clear();
        }
        line += buffer[0];

        // every '*' in output means 2%
        if (buffer[0] == '*') {
            Tick++;
            emit OnProgressDock(Tick);
        }
    }
    if (SaveLogOutput) {
        log.close();
    }
    // if we dont want save config file, then will be remove
    if (!SaveCofigFile) {
        QProcess rm_config;
        unsigned int path_l = ComputePath.GetLength();

        if (ComputePath.GetSubStringFromTo(path_l-1,path_l) != "/") {
            ComputePath += "/";
        }
        QString config_whole;
        config_whole += "\"";
        config_whole += ComputePath.GetBuffer();
        config_whole += ConfigFile;
        config_whole += "\"";
        QString cmd_rm ("rm " + config_whole);
        rm_config.setProcessChannelMode(QProcess::MergedChannels);
        rm_config.start(cmd_rm);

        if (!rm_config.waitForFinished()) {
            CSmallString error;
            error << tr("issue during docking progress: Cannot init rm command: ") << cmd_rm << "!";
            DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return (false);
        }
        if (rm_config.exitCode() != 0) {
            CSmallString error;
            error << tr("issue during docking progress: Cannot delete config file: ") << config_whole << "!";
            DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return (false);
        }
    }
    int status = pclose(fp);
    if (status != 0) {
        CSmallString error;
        error << tr("issue during docking progress: ") << "'" << last_line.toStdString() << "' (during command )";
        DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
        return (false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CDockingProjectDockVina::Finalization(void)
{
    emit OnProgressDock(Tick,true);
    // open results into project and delete if are unwanted

    CDockingProject* p_docproj = dynamic_cast<CDockingProject*>(DockProject);
    if (p_docproj != NULL) {
        unsigned int path_l = ComputePath.GetLength();

        if (ComputePath.GetSubStringFromTo(path_l-1,path_l) != "/") {
            ComputePath += "/";
        }
        QString results_whole;
        results_whole += (ComputePath + ResultsFile).GetBuffer();
        if (p_docproj->LoadResultsStructure(results_whole) ) {
            CSmallString warning;
            warning << tr("Docking process was successfully done! ") << "'" << results_whole << "'";
            DockProject->TextNotification(ETNT_WARNING,QString(warning),ETNT_WARNING_DELAY);
        } else {
            CSmallString error;
            error << tr("issue during docking progress: Results cannot be loaded into Docking project!");
            DockProject->TextNotification(ETNT_ERROR,QString(error),ETNT_ERROR_DELAY);
            return (false);
        }
    } else {
        CSmallString warning;
        warning << tr("Docking process was successfully done. Files are in choosen dir: ") << "'" << ComputePath.GetBuffer() << "'";
        DockProject->TextNotification(ETNT_WARNING,QString(warning),ETNT_WARNING_DELAY);
        return (false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
