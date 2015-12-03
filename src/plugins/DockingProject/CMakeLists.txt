# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(DOCKING_PROJECT_SRC
        DockingProjectMainHeader.cpp
        DockingProjectModule.cpp
        DockingProject.cpp
        DockingProjectDesigner.cpp
        DockingProjectWindow.cpp
        DockingProjectWindowDocking.cpp
        DockingProjectLigandImportJob.cpp      
        DockingProjectLigandImportTool.cpp
	DockingProjectLigandExportJob.cpp
	DockingProjectLigandExportTool.cpp
        DockingProjectReceptorImportJob.cpp
        DockingProjectReceptorImportTool.cpp        
	DockingProjectReceptorExportJob.cpp
        DockingProjectReceptorExportTool.cpp
        DockingProjectResultsImportTool.cpp
        DockingProjectInputExportTool.cpp
        DockingProjectInputVinaExportTool.cpp
        DockingProjectDock.cpp
        DockingProjectDockVina.cpp
        DockingProjectDockPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(DOCKING_PROJECT_UIS
	DockingProjectWindow.ui
        DockingProjectDesigner.ui
        DockingProjectLigandImportTool.ui
        DockingProjectLigandExportTool.ui
        DockingProjectReceptorImportTool.ui
        DockingProjectReceptorExportTool.ui
        DockingProjectInputExportTool.ui
	DockingProjectDockPanel.ui        
        )

# program resources ------------------------------------------------------------
SET(DOCKING_PROJECT_RESOURCES
        DockingProject.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(DOCKING_PROJECT_SRC ${DOCKING_PROJECT_RESOURCES})
QT5_WRAP_UI(DOCKING_PROJECT_SRC ${DOCKING_PROJECT_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(docking_project_shared SHARED ${DOCKING_PROJECT_SRC})

SET_TARGET_PROPERTIES(docking_project_shared PROPERTIES
                        OUTPUT_NAME DockingProject
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(docking_project_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(docking_project_shared Core Widgets Gui Script OpenGL)


INSTALL(TARGETS
            docking_project_shared
        DESTINATION
            lib
        )

