# ==============================================================================
# NEMESIS CMake File
# ==============================================================================

INSTALL(FILE
            nemesis
        DESTINATION
            bin
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ OTHER_EXECUTE OTHER_READ
        )
