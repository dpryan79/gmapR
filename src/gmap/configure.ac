# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# $Id: configure.ac,v 1.46 2010-07-21 22:05:35 twu Exp $

AC_PREREQ(2.57)

## We want to get the version number from file 'VERSION' (rather than
## specifying the version info in 'configure.ac'.  Hence, we need a bit
## of M4 magic.  Note that M4 esyscmd has a trailing newline because the
## shell command output has one, hence the patsubst() trick.
#m4_define([PKG_VERSION],
#          m4_bpatsubst(m4_esyscmd([cat VERSION]),
#                       [\([0-9.]*\)\(\w\|\W\)*],
#                       [\1]))
#m4_define([PKG_VERSION],m4_esyscmd([cat VERSION]))
m4_define([PKG_VERSION],esyscmd([cat VERSION]))

AC_INIT([gmap],PKG_VERSION,[Thomas Wu <twu@gene.com>])
AC_MSG_CHECKING(package version)
AC_MSG_RESULT(PKG_VERSION)

AM_MAINTAINER_MODE([enable])

### Read defaults
AC_MSG_CHECKING(CONFIG_SITE)
if test -z "$CONFIG_SITE"; then
  CONFIG_SITE="./config.site"
else
  CONFIG_SITE="$CONFIG_SITE"
fi
AC_MSG_RESULT($CONFIG_SITE)

for ac_site_file in $CONFIG_SITE; do
  if test -r "$ac_site_file"; then
    echo "loading script $ac_site_file"
    . "$ac_site_file"
  fi
done


# Set default CFLAGS if not already set by user
AC_MSG_CHECKING(CFLAGS)
AC_ARG_VAR([CFLAGS], [Compiler flags (default: -O3)])
if test x"$CFLAGS" = x; then
        AC_MSG_RESULT(not set by user so using default -O3)
        ACX_EXPAND(CFLAGS,'-O3')
else
	AC_MSG_RESULT($CFLAGS)
fi



AC_CONFIG_SRCDIR([src/gmap.c])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_AUX_DIR([config])
# For autotest: AC_CONFIG_TESTDIR([tests],[tests:src:util])

AC_CONFIG_MACRO_DIR([config])

AC_CANONICAL_SYSTEM
AC_SYS_LARGEFILE
AC_ARG_PROGRAM

#AM_INIT_AUTOMAKE([no-dependencies])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AM_CONDITIONAL(FULLDIST,test "x$enable_fulldist" = xyes)
AC_ARG_ENABLE([fulldist],
	       AC_HELP_STRING([--enable-fulldist],
                              [For use by program maintainer]),
	      [enable_fulldist="yes"],
              [enable_fulldist="no"])

AM_CONDITIONAL(MAINTAINER,test "x$enable_maintainer" = xyes)
AC_ARG_ENABLE([maintainer],
	       AC_HELP_STRING([--enable-maintainer],
                              [For use by program maintainer]),
	      [enable_maintainer="yes"],
              [enable_maintainer="no"])

# For Perl scripts, because otherwise configure inserts 
# @bindir@ => "${exec_prefix}/bin";
AC_MSG_CHECKING(bindir)
ACX_EXPAND(BINDIR,$bindir)
AC_SUBST(BINDIR)
AC_MSG_RESULT($BINDIR)

# Works in conjunction with AC_PROG_LIBTOOL -- Commented out because no libraries being built
#AC_MSG_CHECKING(whether to link statically)
#AC_ARG_ENABLE([static-linking],
#               AC_HELP_STRING([--enable-static-linking],
#                              [Link binaries statically (default=no)]),
#              [answer="$enableval"],
#              [answer=""])
#case x"$answer" in
#	xyes)
#	AC_MSG_RESULT(enabled)
#	STATIC_LDFLAG="-all-static"
#	;;
#	
#	xno)
#	AC_MSG_RESULT(disabled)
#	;;
#
#	x)
#	AC_MSG_RESULT(not specified so disabled by default)
#	;;
#esac	
#AC_SUBST(STATIC_LDFLAG)


# Checks for programs.
#AC_PATH_PROG([PERL],[perl])
ACX_PATH_PERL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB(m, rint)

#AC_CHECK_LIB(popt, poptGetContext, [answer="yes"], [answer="no"])
#if test x"$answer" == xyes; then
#	AC_DEFINE([HAVE_LIBPOPT],[1],[Define to 1 if you have the 'popt' library (-lpopt)])
#	POPT_LIBS="-lpopt"
#else
#	POPT_LIBS=""
#fi
#AC_SUBST(POPT_LIBS)

# In call to ACX_PTHREAD, don't provide ACTION-IF-FOUND; otherwise,
# HAVE_PTHREAD won't be defined
AC_MSG_CHECKING(for pthreads feature)
AC_ARG_ENABLE([pthreads],
              AC_HELP_STRING([--enable-pthreads],
                             [Enable pthreads (default=yes)]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
	xyes)
        AC_MSG_RESULT(enabled)
	ACX_PTHREAD([],[pthread_warning=yes])
	;;
	
	xno)
	AC_MSG_RESULT(disabled)
	;;

	x)
	AC_MSG_RESULT([not specified so enabled by default])
	ACX_PTHREAD([],[pthread_warning=yes])
	;;
esac	


# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdlib.h string.h strings.h unistd.h sys/types.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_C_VOLATILE
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_CHECK_TYPES([caddr_t])
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned long long)
AC_CHECK_SIZEOF(off_t)


# Checks for library functions.
#AC_FUNC_MALLOC -- Causes use of rpl_malloc
#AC_FUNC_REALLOC -- Causes use of rpl_realloc
#AC_FUNC_CLOSEDIR_VOID -- We are checking return value

AC_FUNC_FSEEKO

#AC_FUNC_MMAP        # Checks only private fixed mapping of already-mapped memory


AC_MSG_CHECKING(whether mmap is enabled)
AC_ARG_ENABLE([mmap],
	      AC_HELP_STRING([--enable-mmap],
                             [Enable mmap (default=yes).  Recommend that you do not disable this unless absolutely necessary.]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
     xyes)
     AC_MSG_RESULT(enabled)
     ACX_MMAP_FIXED   # Checks fixed mapping
     ACX_MMAP_VARIABLE   # Checks variable mapping
     ;;

     xno)
     AC_MSG_RESULT(disabled)
     ;;

     x)
     AC_MSG_RESULT([not specified so enabled by default])
     ACX_MMAP_FIXED   # Checks fixed mapping
     ACX_MMAP_VARIABLE   # Checks variable mapping
     ;;
esac	


if test x"$acx_mmap_fixed_ok" = xyes; then
  AC_DEFINE(HAVE_MMAP,1,[Define to 1 if you have a working 'mmap' system call.])
else
  if test x"$acx_mmap_variable_ok" = xyes; then
    AC_DEFINE(HAVE_MMAP,1,[Define to 1 if you have a working 'mmap' system call.])
  fi
fi

ACX_MMAP_FLAGS
ACX_MADVISE_FLAGS

AC_CHECK_FUNCS([ceil floor index log madvise memcpy memmove memset munmap pow rint stat64 strtoul sysconf sysctl sigaction])

ACX_STRUCT_STAT64
ACX_PAGESIZE

ACX_FUNC_FOPEN_BINARY
ACX_FUNC_FOPEN_TEXT


AC_MSG_CHECKING(for popcnt feature)
AC_ARG_ENABLE([popcnt],
              AC_HELP_STRING([--enable-popcnt],
                             [Enable -mpopcnt if it compiles and runs (default=yes).  May want to disable if compiling on one machine and running on another.]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
	xyes)
        AC_MSG_RESULT(enabled)
	ACX_BUILTIN
	;;
	
	xno)
	AC_MSG_RESULT(disabled)
	;;

	x)
	AC_MSG_RESULT([not specified so enabled by default])
	ACX_BUILTIN
	;;
esac	

ACX_ASM_BSR

AC_MSG_CHECKING(whether sse2 is enabled)
AC_ARG_ENABLE([sse2],
	      AC_HELP_STRING([--enable-sse2],
                             [Enable sse2 simd commands if they compile and run (default=yes).]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
     xyes)
     AC_MSG_RESULT(enabled)
     ax_cv_want_sse2_ext=yes
     ;;

     xno)
     AC_MSG_RESULT(disabled)
     ax_cv_want_sse2_ext=no
     ;;

     x)
     AC_MSG_RESULT([not specified so enabled by default])
     ax_cv_want_sse2_ext=yes
     ;;
esac	

if test "$ax_cv_want_sse2_ext" = yes; then
AC_MSG_CHECKING(whether user has declared a defective SSE2 compiler)
AC_ARG_WITH([defective-sse2-compiler],
            AC_HELP_STRING([--with-defective-sse2-compiler],
                           [Using an old compiler that requires an immediate shift in slli and srli]),
            [answer="$withval"],
            [answer=""])
if test x"$answer" = x; then
	AC_MSG_RESULT(no)
else
	AC_DEFINE(DEFECTIVE_SSE2_COMPILER,1,[Define to 1 if your old compiler requires an immediate shift in slli and srli.])
	AC_MSG_RESULT(yes)
fi
fi


AC_MSG_CHECKING(whether sse4.1 is enabled)
AC_ARG_ENABLE([sse4.1],
	      AC_HELP_STRING([--enable-sse4.1],
                             [Enable sse4.1 simd commands if they compile and run (default=yes).  Requires that sse2 be enabled.]),
              [answer="$enableval"],
              [answer=""])
if test "$ax_cv_want_sse2_ext" = no; then
   AC_MSG_RESULT([disabled because the user disabled sse2])
   ax_cv_want_sse41_ext=no
else
    case x"$answer" in
	 xyes)
	 AC_MSG_RESULT(enabled)
	 ax_cv_want_sse41_ext=yes
	 ;;

	 xno)
	 AC_MSG_RESULT(disabled)
	 ax_cv_want_sse41_ext=no
	 ;;

	 x)
	 AC_MSG_RESULT([not specified so enabled by default])
	 ax_cv_want_sse41_ext=yes
	 ;;
    esac	
fi

AC_MSG_CHECKING(whether simd is enabled)
AC_ARG_ENABLE([simd],
	      AC_HELP_STRING([--enable-simd],
                             [Enable simd commands in general if they compile and run (default=yes).]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
     xyes)
     AC_MSG_RESULT(enabled)
     AX_EXT   # Sets SIMD_FLAGS, and HAVE_ALTIVEC,HAVE_MMX,HAVE_SSE,HAVE_SSE2,HAVE_SSE3,HAVE_SSSE3,HAVE_SSE4_1,HAVE_SSE4_2,HAVE_AVX
     ;;

     xno)
     AC_MSG_RESULT(disabled)
     ;;

     x)
     AC_MSG_RESULT([not specified so enabled by default])
     AX_EXT   # Sets SIMD_FLAGS, and HAVE_ALTIVEC,HAVE_MMX,HAVE_SSE,HAVE_SSE2,HAVE_SSE3,HAVE_SSSE3,HAVE_SSE4_1,HAVE_SSE4_2,HAVE_AVX
     ;;
esac	

# Genome directory
AC_MSG_CHECKING(gmapdb)
AC_ARG_WITH([gmapdb],
            AC_HELP_STRING([--with-gmapdb=DIR],
                           [Default GMAP database directory]),
            [answer="$withval"],
            [answer=""])
if test x"$answer" = x; then
	ACX_EXPAND(GMAPDB,'${prefix}/share')
else
	GMAPDB=$answer
fi
AC_SUBST(GMAPDB)
AC_MSG_RESULT($GMAPDB)


# MAX_READLENGTH
AC_MSG_CHECKING(MAX_READLENGTH)
AC_ARG_VAR([MAX_READLENGTH], [Maximum read length for GSNAP (default 250)])
if test x"$MAX_READLENGTH" = x; then
        ACX_EXPAND(MAX_READLENGTH,'250')
fi
AC_MSG_RESULT($MAX_READLENGTH)


# zlib package

AC_MSG_CHECKING(for zlib support)
AC_ARG_ENABLE([zlib],
              AC_HELP_STRING([--enable-zlib],
                             [Enable zlib support (option needed for uncompressing gzip files) (default=yes)]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
   xno)
   AC_MSG_RESULT(disabled)
   ZLIB_LIBS=""
   have_zlib=no
   ;;

   *)
   AC_MSG_RESULT(enabled)
   failed=0;
   passed=0;
   AC_CHECK_HEADER(zlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(z,gzopen,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(z,gzeof,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(z,gzgetc,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(z,gzgets,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(z,gzclose,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_MSG_CHECKING(if zlib package is complete)
   if test $failed -gt 0
   then
      AC_MSG_RESULT(no -- some components failed test)
      ZLIB_LIBS=""
      have_zlib=no
   else
      AC_MSG_RESULT(working)
      AC_DEFINE(HAVE_ZLIB,1,[Define to 1 if you have a working zlib library.])
      ZLIB_LIBS="-lz"
      have_zlib=yes

      AC_CHECK_LIB(z,gzbuffer,have_gzbuffer=1,have_gzbuffer=0)
      if test $have_gzbuffer -gt 0
      then
        AC_DEFINE(HAVE_ZLIB_GZBUFFER,1,[Define to 1 if your zlib library has a gzbuffer function.])
      fi
   fi
   ;;

esac
AC_SUBST(ZLIB_LIBS)


# bzip2 package

AC_MSG_CHECKING(for bzlib support)
AC_ARG_ENABLE([bzlib],
              AC_HELP_STRING([--enable-bzlib],
                             [Enable bzlib support (option needed for uncompressing bzip2 files) (default=yes)]),
              [answer="$enableval"],
              [answer=""])
case x"$answer" in
   xno)
   AC_MSG_RESULT(disabled)
   BZLIB_LIBS=""
   have_bzlib=no
   ;;

   *)
   AC_MSG_RESULT(enabled)
   failed=0;
   passed=0;
   AC_CHECK_HEADER(bzlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(bz2,BZ2_bzReadOpen,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(bz2,BZ2_bzRead,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_CHECK_LIB(bz2,BZ2_bzReadClose,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
   AC_MSG_CHECKING(if bzlib package is complete)
   if test $failed -gt 0
   then
      AC_MSG_RESULT(no -- some components failed test)
      BZLIB_LIBS=""
      have_bzlib=no
   else
      AC_MSG_RESULT(working)
      AC_DEFINE(HAVE_BZLIB,1,[Define to 1 if you have a working bzlib library.])
      BZLIB_LIBS="-lbz2"
      have_bzlib=yes
   fi
   ;;

esac
AC_SUBST(BZLIB_LIBS)


# Goby package

AC_MSG_CHECKING(for goby library)
AC_ARG_WITH([goby],
            AC_HELP_STRING([--with-goby=DIR],
                           [Location of Goby header files (in DIR/include) and library files (in DIR/lib) (optional)]),
            [answer="$withval"],
            [answer=""])
if test x"$answer" = x; then
            GOBY_CFLAGS=""
            GOBY_LDFLAGS=""
	    GOBY_LIBS=""
            AC_MSG_RESULT(disabled)
else
            AC_DEFINE(HAVE_GOBY,1,[Define to 1 if you have a working Goby library.])
            GOBY_CFLAGS="-I$withval/include"
            GOBY_LDFLAGS="-L$withval/lib"
	    GOBY_LIBS="-lgoby"
            AC_MSG_RESULT(enabled)
fi
AC_SUBST(GOBY_CFLAGS)
AC_SUBST(GOBY_LDFLAGS)
AC_SUBST(GOBY_LIBS)



AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([util/Makefile])
AC_CONFIG_FILES([util/gmap_compress.pl])
AC_CONFIG_FILES([util/gmap_uncompress.pl])
AC_CONFIG_FILES([util/gmap_process.pl])
AC_CONFIG_FILES([util/gmap_setup.pl])
AC_CONFIG_FILES([util/gmap_build.pl])
AC_CONFIG_FILES([util/gmap_reassemble.pl])
AC_CONFIG_FILES([util/md_coords.pl])
AC_CONFIG_FILES([util/fa_coords.pl])
AC_CONFIG_FILES([util/psl_splicesites.pl])
AC_CONFIG_FILES([util/psl_introns.pl])
AC_CONFIG_FILES([util/psl_genes.pl])
AC_CONFIG_FILES([util/gtf_splicesites.pl])
AC_CONFIG_FILES([util/gtf_introns.pl])
AC_CONFIG_FILES([util/gtf_genes.pl])
AC_CONFIG_FILES([util/gff3_splicesites.pl])
AC_CONFIG_FILES([util/gff3_introns.pl])
AC_CONFIG_FILES([util/gff3_genes.pl])
AC_CONFIG_FILES([util/dbsnp_iit.pl])
AC_CONFIG_FILES([util/gvf_iit.pl])
AC_CONFIG_FILES([util/vcf_iit.pl])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([tests/align.test],[chmod +x tests/align.test])
AC_CONFIG_FILES([tests/coords1.test],[chmod +x tests/coords1.test])
AC_CONFIG_FILES([tests/setup1.test],[chmod +x tests/setup1.test])
AC_CONFIG_FILES([tests/iit.test],[chmod +x tests/iit.test])

AC_OUTPUT

if test x"$perl_warning" = xyes; then
  AC_MSG_WARN([

WARNING: You don't seem to have perl installed with working
versions of the IO::File and Getopt::Std modules.  If you know
where such a version of Perl exists, please specify its full
path under PERL in the config.site file and run configure again.
])
fi

if test x"$pthread_warning" = xyes; then
  AC_MSG_WARN([

WARNING: You don't seem to have headers and libraries for pthreads.
GMAP will work without these, but the program can run faster on
multiprocessor machines if POSIX threads are available.  If you wish
to enable pthreads, please specify the appropriate values for CC,
PTHREAD_CFLAGS, and PTHREAD_LIBS in the config.site file and run
configure again.  Otherwise, you may proceed with the rest of the
installation.
])
fi

if test x"$acx_mmap_fixed_ok" != xyes; then
  if test x$"acx_mmap_variable_ok" != xyes; then
    AC_MSG_WARN([

WARNING: You don't seem to have memory mapping.  GMAP will work
without this, but the program can run faster if memory mapping is
available.  If you wish to enable memory mapping, please specify the
appropriate values for CC, PTHREAD_CFLAGS, and PTHREAD_LIBS in the
config.site file and run configure again.  Otherwise, you may proceed
with the rest of the installation.
])
  fi
fi
