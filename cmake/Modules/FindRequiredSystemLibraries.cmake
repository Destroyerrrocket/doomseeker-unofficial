# [Zalewa]
# This is a modified "InstallRequiredSystemLibraries.cmake" file.
# The original file is normally included with CMake. This modification
# includes:
# - the libraries and manifests are no longer forcibly installed. 
#   Instead, the module sets REQUIRED_SYSTEM_LIBRARIES variable to all files 
#   which need installation.
# - a warning message about the screwed up manifest in MSVC90 is displayed 
#   so that user can fix it:
#   http://www.cmake.org/pipermail/cmake/2008-September/023822.html

# By including this file, all files in the CMAKE_INSTALL_DEBUG_LIBRARIES,
# will be installed with INSTALL_PROGRAMS into /bin for WIN32 and /lib
# for non-win32. If CMAKE_SKIP_INSTALL_RULES is set to TRUE before including
# this file, then the INSTALL command is not called.  The user can use 
# the variable CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS to use a custom install 
# command and install them into any directory they want.
# If it is the MSVC compiler, then the microsoft run
# time libraries will be found and automatically added to the
# CMAKE_INSTALL_DEBUG_LIBRARIES, and installed.  
# If CMAKE_INSTALL_DEBUG_LIBRARIES is set and it is the MSVC
# compiler, then the debug libraries are installed when available.
# If CMAKE_INSTALL_MFC_LIBRARIES is set then the MFC run time
# libraries are installed as well as the CRT run time libraries.

#=============================================================================
# Copyright (c) 2006-2009, Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

IF(MSVC)
  FILE(TO_CMAKE_PATH "$ENV{SYSTEMROOT}" SYSTEMROOT)

  IF(MSVC70)
    SET(__install__libs
      "${SYSTEMROOT}/system32/msvcp70.dll"
      "${SYSTEMROOT}/system32/msvcr70.dll"
      )
  ENDIF(MSVC70)

  IF(MSVC71)
    SET(__install__libs
      "${SYSTEMROOT}/system32/msvcp71.dll"
      "${SYSTEMROOT}/system32/msvcr71.dll"
      )
  ENDIF(MSVC71)

  IF(CMAKE_CL_64)
    SET(CMAKE_MSVC_ARCH amd64)
  ELSE(CMAKE_CL_64)
    SET(CMAKE_MSVC_ARCH x86)
  ENDIF(CMAKE_CL_64)

  GET_FILENAME_COMPONENT(devenv_dir "${CMAKE_MAKE_PROGRAM}" PATH)
  GET_FILENAME_COMPONENT(base_dir "${devenv_dir}/../.." ABSOLUTE)

  IF(MSVC80)
    # Find the runtime library redistribution directory.
    FIND_PATH(MSVC80_REDIST_DIR NAMES ${CMAKE_MSVC_ARCH}/Microsoft.VC80.CRT/Microsoft.VC80.CRT.manifest
      PATHS
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;InstallDir]/../../VC/redist"
        "${base_dir}/VC/redist"
      )
    MARK_AS_ADVANCED(MSVC80_REDIST_DIR)
    SET(MSVC80_CRT_DIR "${MSVC80_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC80.CRT")

    # Install the manifest that allows DLLs to be loaded from the
    # directory containing the executable.
    SET(__install__libs
      "${MSVC80_CRT_DIR}/Microsoft.VC80.CRT.manifest"
      "${MSVC80_CRT_DIR}/msvcm80.dll"
      "${MSVC80_CRT_DIR}/msvcp80.dll"
      "${MSVC80_CRT_DIR}/msvcr80.dll"
      )

    IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
      SET(MSVC80_CRT_DIR
        "${MSVC80_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC80.DebugCRT")
      SET(__install__libs ${__install__libs}
        "${MSVC80_CRT_DIR}/Microsoft.VC80.DebugCRT.manifest"
        "${MSVC80_CRT_DIR}/msvcm80d.dll"
        "${MSVC80_CRT_DIR}/msvcp80d.dll"
        "${MSVC80_CRT_DIR}/msvcr80d.dll"
        )
    ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES)

  ENDIF(MSVC80)

  IF(MSVC90)
    # Find the runtime library redistribution directory.
    FIND_PATH(MSVC90_REDIST_DIR NAMES ${CMAKE_MSVC_ARCH}/Microsoft.VC90.CRT/Microsoft.VC90.CRT.manifest
      PATHS
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0;InstallDir]/../../VC/redist"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\9.0;InstallDir]/../../VC/redist"
        "${base_dir}/VC/redist"
      )
    MARK_AS_ADVANCED(MSVC90_REDIST_DIR)
    SET(MSVC90_CRT_DIR "${MSVC90_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC90.CRT")

    # Install the manifest that allows DLLs to be loaded from the
    # directory containing the executable.
    SET(MSVC90_MANIFEST_PATH "${MSVC90_CRT_DIR}/Microsoft.VC90.CRT.manifest")
    SET(__install__libs
      "${MSVC90_CRT_DIR}/Microsoft.VC90.CRT.manifest"
      "${MSVC90_CRT_DIR}/msvcm90.dll"
      "${MSVC90_CRT_DIR}/msvcp90.dll"
      "${MSVC90_CRT_DIR}/msvcr90.dll"
      ) 
    IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
      SET(MSVC90_CRT_DIR
        "${MSVC90_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC90.DebugCRT")
      SET(MSVC90_MANIFEST_PATH "${MSVC90_CRT_DIR}/Microsoft.VC90.DebugCRT.manifest")
      SET(__install__libs ${__install__libs}
        "${MSVC90_CRT_DIR}/Microsoft.VC90.DebugCRT.manifest"
        "${MSVC90_CRT_DIR}/msvcm90d.dll"
        "${MSVC90_CRT_DIR}/msvcp90d.dll"
        "${MSVC90_CRT_DIR}/msvcr90d.dll"
        )
    ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES)
    MESSAGE(WARNING "Microsoft Visual Studio 9.0 was detected\n"
        "This version of MSVC contains a bug. The version in Microsoft.VC90.CRT.manifest file is set to 9.0.30729.1. "
        "It should be 9.0.21022.8. Fix the file or 'install' and 'package' targets will produce invalid packages.\n"
        "The path to currently used file is:\n${MSVC90_MANIFEST_PATH}"
    )
  ENDIF(MSVC90) 

  IF(MSVC10)
    # Find the runtime library redistribution directory.
    FIND_PATH(MSVC10_REDIST_DIR NAMES ${CMAKE_MSVC_ARCH}/Microsoft.VC100.CRT
      PATHS
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0;InstallDir]/../../VC/redist"
        "${base_dir}/VC/redist"
      )
    MARK_AS_ADVANCED(MSVC10_REDIST_DIR)
    SET(MSVC10_CRT_DIR "${MSVC10_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC100.CRT")

    # Install the manifest that allows DLLs to be loaded from the
    # directory containing the executable.
    SET(__install__libs
      "${MSVC10_CRT_DIR}/Microsoft.VC100.CRT.manifest"
      "${MSVC10_CRT_DIR}/msvcp100.dll"
      "${MSVC10_CRT_DIR}/msvcr100.dll"
      ) 
    IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
      SET(MSVC10_CRT_DIR
        "${MSVC10_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC90.DebugCRT")
      SET(__install__libs ${__install__libs}
        "${MSVC10_CRT_DIR}/Microsoft.VC100.DebugCRT.manifest"
        "${MSVC10_CRT_DIR}/msvcp100d.dll"
        "${MSVC10_CRT_DIR}/msvcr100d.dll"
        )
    ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES) 
  ENDIF(MSVC10)

  IF(CMAKE_INSTALL_MFC_LIBRARIES)
    IF(MSVC70)
      SET(__install__libs ${__install__libs}
        "${SYSTEMROOT}/system32/mfc70.dll"
        )
    ENDIF(MSVC70)
    IF(MSVC71)
      SET(__install__libs ${__install__libs}
        "${SYSTEMROOT}/system32/mfc71.dll"
        )
    ENDIF(MSVC71)
    IF(MSVC80)
      IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        SET(MSVC80_MFC_DIR
          "${MSVC80_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC80.DebugMFC")
        SET(__install__libs ${__install__libs}
          "${MSVC80_MFC_DIR}/Microsoft.VC80.DebugMFC.manifest"
          "${MSVC80_MFC_DIR}/mfc80d.dll"
          "${MSVC80_MFC_DIR}/mfc80ud.dll"
          "${MSVC80_MFC_DIR}/mfcm80d.dll"
          "${MSVC80_MFC_DIR}/mfcm80ud.dll"
          )
      ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        
      SET(MSVC80_MFC_DIR "${MSVC80_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC80.MFC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC80_MFC_DIR}/Microsoft.VC80.MFC.manifest"
        "${MSVC80_MFC_DIR}/mfc80.dll"
        "${MSVC80_MFC_DIR}/mfc80u.dll"
        "${MSVC80_MFC_DIR}/mfcm80.dll"
        "${MSVC80_MFC_DIR}/mfcm80u.dll"
        )
      # include the language dll's for vs8 as well as the actuall dll's
      SET(MSVC80_MFCLOC_DIR "${MSVC80_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC80.MFCLOC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC80_MFCLOC_DIR}/Microsoft.VC80.MFCLOC.manifest"
        "${MSVC80_MFCLOC_DIR}/mfc80chs.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80cht.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80enu.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80esp.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80deu.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80fra.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80ita.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80jpn.dll"
        "${MSVC80_MFCLOC_DIR}/mfc80kor.dll"
        )
    ENDIF(MSVC80)

    IF(MSVC90)
      IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        SET(MSVC90_MFC_DIR
          "${MSVC90_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC90.DebugMFC")
        SET(__install__libs ${__install__libs}
          "${MSVC90_MFC_DIR}/Microsoft.VC90.DebugMFC.manifest"
          "${MSVC90_MFC_DIR}/mfc90d.dll"
          "${MSVC90_MFC_DIR}/mfc90ud.dll"
          "${MSVC90_MFC_DIR}/mfcm90d.dll"
          "${MSVC90_MFC_DIR}/mfcm90ud.dll"
          )
      ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        
      SET(MSVC90_MFC_DIR "${MSVC90_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC90.MFC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC90_MFC_DIR}/Microsoft.VC90.MFC.manifest"
        "${MSVC90_MFC_DIR}/mfc90.dll"
        "${MSVC90_MFC_DIR}/mfc90u.dll"
        "${MSVC90_MFC_DIR}/mfcm90.dll"
        "${MSVC90_MFC_DIR}/mfcm90u.dll"
        )
      # include the language dll's for vs9 as well as the actuall dll's
      SET(MSVC90_MFCLOC_DIR "${MSVC90_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC90.MFCLOC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC90_MFCLOC_DIR}/Microsoft.VC90.MFCLOC.manifest"
        "${MSVC90_MFCLOC_DIR}/mfc90chs.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90cht.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90enu.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90esp.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90deu.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90fra.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90ita.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90jpn.dll"
        "${MSVC90_MFCLOC_DIR}/mfc90kor.dll"
        )
    ENDIF(MSVC90)

    IF(MSVC10)
      IF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        SET(MSVC10_MFC_DIR
          "${MSVC10_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC100.DebugMFC")
        SET(__install__libs ${__install__libs}
          "${MSVC10_MFC_DIR}/Microsoft.VC100.DebugMFC.manifest"
          "${MSVC10_MFC_DIR}/mfc100d.dll"
          "${MSVC10_MFC_DIR}/mfc100ud.dll"
          "${MSVC10_MFC_DIR}/mfcm100d.dll"
          "${MSVC10_MFC_DIR}/mfcm100ud.dll"
          )
      ENDIF(CMAKE_INSTALL_DEBUG_LIBRARIES)
        
      SET(MSVC10_MFC_DIR "${MSVC10_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC100.MFC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC10_MFC_DIR}/Microsoft.VC100.MFC.manifest"
        "${MSVC10_MFC_DIR}/mfc100.dll"
        "${MSVC10_MFC_DIR}/mfc100u.dll"
        "${MSVC10_MFC_DIR}/mfcm100.dll"
        "${MSVC10_MFC_DIR}/mfcm100u.dll"
        )
      # include the language dll's for vs10 as well as the actuall dll's
      SET(MSVC10_MFCLOC_DIR "${MSVC10_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC100.MFCLOC")
      # Install the manifest that allows DLLs to be loaded from the
      # directory containing the executable.
      SET(__install__libs ${__install__libs}
        "${MSVC10_MFCLOC_DIR}/Microsoft.VC100.MFCLOC.manifest"
        "${MSVC10_MFCLOC_DIR}/mfc100chs.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100cht.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100enu.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100esp.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100deu.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100fra.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100ita.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100jpn.dll"
        "${MSVC10_MFCLOC_DIR}/mfc100kor.dll"
        )
    ENDIF(MSVC10)

  ENDIF(CMAKE_INSTALL_MFC_LIBRARIES)

  FOREACH(lib
      ${__install__libs}
      )
    IF(EXISTS ${lib})
      SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
        ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} ${lib})
    ENDIF(EXISTS ${lib})
  ENDFOREACH(lib)
ENDIF(MSVC)

# Include system runtime libraries in the installation if any are
# specified by CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS.
IF(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
  IF(NOT CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP)
      set(REQUIRED_SYSTEM_LIBRARIES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
  ENDIF(NOT CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP)
ENDIF(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
