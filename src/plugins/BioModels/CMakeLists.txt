# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(BIOMODELS_SRC
        BioModelsMainHeader.cpp
        BioModelsModule.cpp
        )

# program windows --------------------------------------------------------------
SET(BIOMODELS_UIS
        )

# program resources ------------------------------------------------------------
SET(BIOMODELS_RESOURCES
        BioModelsModule.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(BIOMODELS_SRC ${BIOMODELS_RESOURCES})
#QT5_WRAP_UI(BIOMODELS_SRC ${BUILD_PROJECT_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(biomodels_shared SHARED ${BIOMODELS_SRC})
ADD_DEPENDENCIES(biomodels_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(biomodels_shared PROPERTIES
                        OUTPUT_NAME BioModels
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(biomodels_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

INSTALL(TARGETS
            biomodels_shared
        DESTINATION
            lib
        )

