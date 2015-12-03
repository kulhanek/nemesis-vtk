# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# plusula part
INCLUDE_DIRECTORIES(lib/plusula SYSTEM)

# NemesisCore is common for all part of Nemesis
INCLUDE_DIRECTORIES(lib/NemesisCore SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/common SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/units SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/jscript SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/project SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/project/utils SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/manip SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/manip/drivers SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/selection SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/selection/handlers SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/selection/masks SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/selection/masks/asl SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/selection/masks/vmd SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics/standard SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics/utils SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics/links SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics/primary SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graphics/shadow SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/structure SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/structure/restraints SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/graph SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/job SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/build SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/workpanel SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/widgets SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/windows SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/dialogs SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/properties SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/properties/utils SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/properties/standard SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/trajectory SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/trajectory/segments SYSTEM)
INCLUDE_DIRECTORIES(lib/NemesisCore/batchjob SYSTEM)

# part used by collaborative project
INCLUDE_DIRECTORIES(lib/CollabCore/ SYSTEM)
INCLUDE_DIRECTORIES(lib/CollabCore/communication SYSTEM)
INCLUDE_DIRECTORIES(lib/CollabCore/data/base SYSTEM)

# include subdirectories -------------------------------------------------------

# NemesisCore and plugins
ADD_SUBDIRECTORY(lib)

# programs
ADD_SUBDIRECTORY(bin)

# plugins
ADD_SUBDIRECTORY(plugins)
