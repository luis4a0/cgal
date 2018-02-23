#.rst:
# CGAL_SetupCGAL_CoreDependencies
# -------------------------------
#
# The module searchs for the dependencies of the `CGAL_Core` library:
#   - the `GMP/MPFR` couple,
#
# and defines the variable :variable:`CGAL_Core_FOUND` and the function
# :command:`CGAL_setup_CGAL_Core_dependencies`.
#
# Module Input Variables
# ^^^^^^^^^^^^^^^^^^^^^^
# - :variable:`CGAL_DISABLE_GMP`

if(CGAL_SetupCGAL_CoreDependencies_included)
  return()
endif()
set(CGAL_SetupCGAL_CoreDependencies_included TRUE)

#.rst:
# Used Modules
# ^^^^^^^^^^^^
#   - :module:`CGAL_SetupGMP`
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# .. variable:: CGAL_Core_FOUND
#
#    Set to `TRUE` if the dependencies of `CGAL_Core` were found.

if(NOT CGAL_DISABLE_GMP)
  include(${CMAKE_CURRENT_LIST_DIR}/CGAL_SetupGMP.cmake)
  if(GMP_FOUND)
    set(CGAL_Core_FOUND TRUE)
  endif()
endif()

#.rst:
#
# Provided Functions
# ^^^^^^^^^^^^^^^^^^
#
# .. command:: CGAL_setup_CGAL_Core_dependencies
#
#   Link the target with the dependencies of `CGAL_Core`::
#
#     CGAL_setup_CGAL_Core_dependencies( target [INTERFACE] )
#
#   If the option ``INTERFACE`` is passed, the dependencies are
#   added using :command:`target_link_libraries` with the ``INTERFACE``
#   keyword, or ``PUBLIC`` otherwise.
#

# See the release notes of CGAL-4.10: CGAL_Core now requires
# Boost.Thread, with all compilers but MSVC.
if (NOT MSVC)
  find_package( Boost 1.48 REQUIRED thread system )
endif()

function(CGAL_setup_CGAL_Core_dependencies target)
  if(ARGV1 STREQUAL INTERFACE)
    set(keyword INTERFACE)
  else()
    set(keyword PUBLIC)
  endif()
  use_CGAL_GMP_support(CGAL_Core ${keyword})
  target_compile_definitions(${target} ${keyword} CGAL_USE_CORE=1)
  target_link_libraries( CGAL_Core ${keyword} CGAL::CGAL ${Boost_LIBRARIES})
endfunction()
