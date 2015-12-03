# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# for UI & moc
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# objects in library -----------------------------------------------------------
SET(CHEMINFO_PROJECT_SRC
        CheminfoProjectMainHeader.cpp
        CheminfoProjectModule.cpp
        CheminfoProject.cpp
        CheminfoProjectInfo.cpp
        CheminfoProjectInfoDesigner.cpp
        CheminfoProjectInfoList.cpp
        CheminfoProjectInfoListDesigner.cpp
        CheminfoProjectDatabase.cpp
        CheminfoProjectDatabaseStructure.cpp
        CheminfoProjectDesigner.cpp
        CheminfoProjectSetDatabase.cpp
        CheminfoProjectSetDatabaseStructure.cpp
        CheminfoProjectStructure.cpp
        CheminfoProjectStructureDesigner.cpp
        CheminfoProjectStructureList.cpp
        CheminfoProjectStructureListProject.cpp
        CheminfoProjectStructureListDesigner.cpp
        CheminfoProjectTransfer.cpp
	CheminfoProjectWindow.cpp
        CheminfoProjectWindowCheminfo.cpp
        models/CheminfoProjectInfoListModel.cpp
        models/CheminfoStructureListModel.cpp
        transfer/CheminfoStructureImportJob.cpp
        transfer/CheminfoResultsImportJob.cpp
        utils/PODesignerCheminfoProjectInfoList.cpp
        utils/PODesignerCheminfoProjectStructureList.cpp
        workpanels/CheminfoProjectWorkPanel.cpp
        workpanels/CheminfoDatabaseWorkPanel.cpp
        workpanels/CheminfoStructureWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(CHEMINFO_PROJECT_UIS
        CheminfoProjectWindow.ui
        CheminfoProjectDesigner.ui
        CheminfoProjectInfoDesigner.ui
        CheminfoProjectInfoListDesigner.ui
        CheminfoProjectStructureDesigner.ui
        CheminfoProjectStructureListDesigner.ui
        CheminfoProjectSetDatabase.ui
        CheminfoProjectSetDatabaseStructure.ui
        utils/PODesignerCheminfoProjectInfoList.ui
        utils/PODesignerCheminfoProjectStructureList.ui
        workpanels/CheminfoProjectWorkPanel.ui
        workpanels/CheminfoDatabaseWorkPanel.ui
        workpanels/CheminfoStructureWorkPanel.ui
        )

# program resources ------------------------------------------------------------
SET(CHEMINFO_PROJECT_RESOURCES
        CheminfoProject.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(CHEMINFO_PROJECT_SRC ${CHEMINFO_PROJECT_RESOURCES})
QT5_WRAP_UI(CHEMINFO_PROJECT_SRC ${CHEMINFO_PROJECT_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(cheminfo_project_shared SHARED ${CHEMINFO_PROJECT_SRC})

SET_TARGET_PROPERTIES(cheminfo_project_shared PROPERTIES
                        OUTPUT_NAME CheminfoProject
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(cheminfo_project_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${FIREBIRD_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(cheminfo_project_shared Core Widgets Gui Script OpenGL)

INSTALL(TARGETS
            cheminfo_project_shared
        DESTINATION
            lib
        )

