AC_INIT(qahtml,[1.00])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE
AC_PROG_CXX(CC g++)
LT_INIT([disable-static])

case $CXX in
  clang++)
    CXXFLAGS="$CXXFLAGS -Wall -Werror -Wextra -Wshadow -Wno-deprecated-declarations"
  ;;
  *g++)
    CXXFLAGS="$CXXFLAGS -Wall -Werror -pedantic -Wextra -Wshadow -Wno-deprecated-declarations"
  ;;
esac

CINTDEFS=" -noIncludePaths -inlineInputHeader"
AC_SUBST(CINTDEFS)

dnl PLEASE USE ALPHABETICAL ORDER FOR THE MAKEFILE LIST
dnl otherwise noone will be able to find your subsystem
AC_CONFIG_FILES([
  Makefile
  client/Makefile
  database/Makefile
  subsystems/Makefile
  subsystems/bco/Makefile
  subsystems/calo/Makefile
  subsystems/example/Makefile
  subsystems/globalQA/Makefile
  subsystems/intt/Makefile
  subsystems/inttrawhit/Makefile
  subsystems/jets/Makefile
  subsystems/micromegas/Makefile
  subsystems/mvtx/Makefile
  subsystems/mvtxrawhit/Makefile
  subsystems/siliconseeds/Makefile
  subsystems/tpc/Makefile
  subsystems/tpcrawhit/Makefile
  subsystems/tpcseeds/Makefile
  subsystems/tpcsil/Makefile
  subsystems/vertex/Makefile
])

AC_OUTPUT
