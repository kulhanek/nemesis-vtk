# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# work panels ------------------------------------
ADD_SUBDIRECTORY(GlobalWorkPanels)
ADD_SUBDIRECTORY(StandardWorkPanels)

# conversion, force fields -----------------------
ADD_SUBDIRECTORY(OpenBabel)
ADD_SUBDIRECTORY(ExtraImpex)

# dedicated projects -----------------------------
ADD_SUBDIRECTORY(BuildProject)
ADD_SUBDIRECTORY(SketchProject)
ADD_SUBDIRECTORY(TrajectoryProject)

# special stuff ----------------------------------
# peptide/nucleic acid builders, etc.
ADD_SUBDIRECTORY(BioTools)
# RMSD fit and isomorphism
ADD_SUBDIRECTORY(RMSDTools)
# crystal and symmetry tools
ADD_SUBDIRECTORY(CrystalTools)
# extra tools
ADD_SUBDIRECTORY(ExtraTools)

# program interfaces -----------------------------
# amber related tools and projects
ADD_SUBDIRECTORY(Amber)
# gaussian export/import
ADD_SUBDIRECTORY(Gaussian)
# turbomole export/import
#ADD_SUBDIRECTORY(Turbomole)
# cpmd export/import
ADD_SUBDIRECTORY(CPMD)
