# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(GLOBAL_WORK_PANELS_SRC
        GlobalWorkPanelsMainHeader.cpp
        GlobalWorkPanelsModule.cpp
        ErrorConsoleWorkPanel.cpp
        PluginDatabaseWorkPanel.cpp
        GlobalSetupWorkPanel.cpp
        QuantityUnitSetupWorkPanel.cpp
        GraphicsSetupWorkPanel.cpp
        ColorSetupWorkPanel.cpp
        JSTerminalWorkPanel.cpp
        JobSchedulerSetupWorkPanel.cpp
        ObjectInspectorWorkPanel.cpp
        PeriodicTableWorkPanel.cpp
        NewProjectWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(GLOBAL_WORK_PANELS_UIS
        ErrorConsoleWorkPanel.ui
        PluginDatabaseWorkPanel.ui
        GlobalSetupWorkPanel.ui
        QuantityUnitSetupWorkPanel.ui
        GraphicsSetupWorkPanel.ui
        ColorSetupWorkPanel.ui
        JSTerminalWorkPanel.ui
        ObjectInspectorWorkPanel.ui
        JobSchedulerSetupWorkPanel.ui
        NewProjectWorkPanel.ui
        )

# set resource files -----------------------------------------------------------
SET(GLOBAL_WORK_PANELS_RCS
        GlobalWorkPanels.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_WRAP_UI(GLOBAL_WORK_PANELS_SRC ${GLOBAL_WORK_PANELS_UIS})
QT5_ADD_RESOURCES(GLOBAL_WORK_PANELS_SRC ${GLOBAL_WORK_PANELS_RCS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(global_work_panels_shared SHARED ${GLOBAL_WORK_PANELS_SRC})
ADD_DEPENDENCIES(global_work_panels_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(global_work_panels_shared PROPERTIES
                        OUTPUT_NAME GlobalWorkPanels
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(global_work_panels_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(global_work_panels_shared Core Widgets Gui Script)

INSTALL(TARGETS
            global_work_panels_shared
        DESTINATION
            lib
        )

