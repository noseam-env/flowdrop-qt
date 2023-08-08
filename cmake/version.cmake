# This file is part of flowdrop-qt.
#
# For license and copyright information please follow this link:
# https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL

function(app_parse_version file)
    #file(STRINGS ${file} lines)
    file(READ ${file} file_content)
    string(STRIP "${file_content}" trimmed_content)

    string(REPLACE "." ";" components ${trimmed_content})
    list(LENGTH components components_count)
    list(GET components 0 major)
    list(GET components 1 minor)
    if (${components_count} GREATER 2)
        list(GET components 2 patch)
        if (${components_count} GREATER 3)
            list(GET components 3 alpha)
            if (${alpha} STREQUAL beta)
                set(beta 1)
                set(beta_bool "true")
                set(alpha 0)
            endif()
        else ()
            set(alpha 0)
        endif ()
    else()
        set(patch 0)
    endif()

    set(version_dot ${major}.${minor}.${patch}.${alpha})
    set(version_dot ${major}.${minor}.${patch}.${alpha})
    string(REPLACE "." "," version_comma ${version_dot})

    set(str_major ${major}.${minor})
    set(str_normal ${str_major}.${patch})
    if (${patch} STREQUAL 0)
        set(str_small ${str_major})
    else()
        set(str_small ${str_normal})
    endif()

    set(app_version_string ${str_normal} PARENT_SCOPE)
    set(app_version_string_small ${str_small} PARENT_SCOPE)
    set(app_version_dot ${version_dot} PARENT_SCOPE)
    set(app_version_comma ${version_comma} PARENT_SCOPE)
    set(app_version_cmake ${version_dot} PARENT_SCOPE)

    message("Version: ${version_dot}")
endfunction()
