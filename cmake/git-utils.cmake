# Author: Younguk Kim (bluekyu)
# Date  : 2017-08-18

# get_git_head_hash(project_directory)
#
# Get hash string of HEAD point from git project.
function(get_git_head_hash project_directory out_hash_var)
    set(git_dir_path "${project_directory}/.git")
    if(EXISTS ${git_dir_path})
        if(NOT EXISTS "${git_dir_path}/HEAD")
            message(ERROR "[git-utils] HEAD file does NOT exist in .git directory!")
            set(${out_hash_var} "" PARENT_SCOPE)
            return()
        endif()
        file(STRINGS "${git_dir_path}/HEAD" git_HEAD_contents)
        string(REGEX MATCH "^[0-9a-f]+$" git_HEAD_hash ${git_HEAD_contents})
        # it is ref, not hash
        if(NOT git_HEAD_hash)
            string(REGEX REPLACE "^ref: (.*)$" "\\1" git_HEAD_ref_path ${git_HEAD_contents})
            # try to use file in refs/heads
            if (EXISTS "${git_dir_path}/${git_HEAD_ref_path}")
                file(STRINGS "${git_dir_path}/${git_HEAD_ref_path}" git_HEAD_hash REGEX "^[0-9a-f]+$")
            # try to use info/refs file if refs/heads is clean (ex, by "git gc")
            elseif(EXISTS "${git_dir_path}/info/refs")
                file(STRINGS "${git_dir_path}/info/refs" git_info_ref_contents
                    REGEX "[0-9a-f]+[ \t]+${git_HEAD_ref_path}$"
                )
                string(SUBSTRING "${git_info_ref_contents}" 0 40 git_HEAD_hash)
            endif()
        endif()

        if(git_HEAD_hash)
            set(${out_hash_var} "${git_HEAD_hash}" PARENT_SCOPE)
        else()
            message(WARNING "[git-utils] Cannot find git-commit hash.")
            set(${out_hash_var} "" PARENT_SCOPE)
        endif()
    else()
        message(ERROR "[git-utils] '.git' directory does NOT exist!")
    endif()
endfunction()
