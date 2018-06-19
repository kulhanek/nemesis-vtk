# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(ADMIN_WP_SRC
        AdminWPMainHeader.cpp
        AdminWPModule.cpp
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
SET(ADMIN_WP_UIS
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
SET(ADMIN_WP_RCS
        AdminWP.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_WRAP_UI(ADMIN_WP_SRC ${ADMIN_WP_UIS})
QT5_ADD_RESOURCES(ADMIN_WP_SRC ${ADMIN_WP_RCS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(admin_wp_shared SHARED ${ADMIN_WP_SRC})
ADD_DEPENDENCIES(admin_wp_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(admin_wp_shared PROPERTIES
                        OUTPUT_NAME AdminWP
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(admin_wp_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(admin_wp_shared Core Widgets Gui Script)

INSTALL(TARGETS
            admin_wp_shared
        DESTINATION
            lib
        )

