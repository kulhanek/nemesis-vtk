# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(STANDARD_WORK_PANELS_SRC
        StandardWorkPanelsMainHeader.cpp
        StandardWorkPanelsModule.cpp
        BuildWorkPanel.cpp
        DockingWorkPanel.cpp
        GeometryWorkPanel.cpp
        GeoManipWorkPanel.cpp
        SelectionWorkPanel.cpp
        JobWorkPanel.cpp
        NewGraphicsObjectWorkPanel.cpp
        GraphicsManagerWorkPanel.cpp
        ActiveProfileManagerWorkPanel.cpp
        RestraintsManagerWorkPanel.cpp
        SelectByMaskWorkPanel.cpp
        NewPropertyWorkPanel.cpp
        PropertyManagerWorkPanel.cpp
        StructureListWorkPanel.cpp
        TrajectoryListWorkPanel.cpp
        NewSnapshotFilterWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(STANDARD_WORK_PANELS_UIS
        BuildWorkPanel.ui
        DockingWorkPanel.ui
        GeometryWorkPanel.ui
        GeoManipWorkPanel.ui
        SelectionWorkPanel.ui
        JobWorkPanel.ui
        NewGraphicsObjectWorkPanel.ui
        GraphicsManagerWorkPanel.ui
        ActiveProfileManagerWorkPanel.ui
        RestraintsManagerWorkPanel.ui
        SelectByMaskWorkPanel.ui
        NewPropertyWorkPanel.ui
        PropertyManagerWorkPanel.ui
        StructureListWorkPanel.ui
        TrajectoryListWorkPanel.ui
        NewSnapshotFilterWorkPanel.ui
        )

# program resources
SET(STANDARD_WORK_PANELS_RESOURCES
        StandardWorkPanels.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_WRAP_UI(STANDARD_WORK_PANELS_SRC ${STANDARD_WORK_PANELS_UIS})
QT5_ADD_RESOURCES(STANDARD_WORK_PANELS_SRC ${STANDARD_WORK_PANELS_RESOURCES})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(standard_work_panels_shared SHARED ${STANDARD_WORK_PANELS_SRC})
ADD_DEPENDENCIES(standard_work_panels_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(standard_work_panels_shared PROPERTIES
                        OUTPUT_NAME StandardWorkPanels
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(standard_work_panels_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(standard_work_panels_shared Core Widgets Gui)

INSTALL(TARGETS
            standard_work_panels_shared
        DESTINATION
            lib
        )

