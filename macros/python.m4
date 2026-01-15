# Configure paths for Python
# Owen Taylor     97-11-3

dnl AM_PATH_PYTHON([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Python, and define PYTHON variables
dnl
AC_DEFUN([AM_PATH_PYTHON],
 [
  dnl Find a Python interpreter.  Python versions prior to 2.0 are not supported.
  m4_define_default([_AM_PYTHON_INTERPRETER_LIST],
[python python3 python3.9 python3.8 python3.7 python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python3.0 python2.7 python2.6 python2.5 python2.4 python2.3 python2.2 python2.1 python2.0])

  AC_ARG_VAR([PYTHON], [the Python interpreter])

  m4_if([$1],[],[
    dnl No version check is needed.
    # Find any Python interpreter.
    if test -z "$PYTHON"; then
      AC_PATH_PROGS([PYTHON], _AM_PYTHON_INTERPRETER_LIST, :)
    fi
    am_display_PYTHON=python
  ], [
    dnl A version check is needed.
    if test -n "$PYTHON"; then
      # If the user set $PYTHON, use it and don't search something else.
      AC_MSG_CHECKING([whether $PYTHON version is >= $1])
      AM_PYTHON_CHECK_VERSION([$PYTHON], [$1],
			      [AC_MSG_RESULT([yes])],
			      [AC_MSG_RESULT([no])
			       AC_MSG_ERROR([Python interpreter is too old])])
      am_display_PYTHON=$PYTHON
    else
      # Otherwise, try each interpreter until we find one that satisfies
      # VERSION.
      AC_CANONICAL_HOST
      m4_foreach_default([am_python_interpreter], [_AM_PYTHON_INTERPRETER_LIST], [
	if test -z "$PYTHON"; then
	  AC_PATH_PROG([PYTHON], [am_python_interpreter], [])
	fi
	if test -n "$PYTHON"; then
	  AM_PYTHON_CHECK_VERSION([$PYTHON], [$1], [break])
	fi
      ])
      am_display_PYTHON=$PYTHON
    fi
  ])

  if test "$PYTHON" = :; then
  dnl Run any user-specified action, or abort.
    m4_default([$3], [AC_MSG_ERROR([no suitable Python interpreter found])])
  else

  dnl Query Python for its version number.  Getting [:3] seems to be
  dnl the best way to do this; it's the "trunc" program that
  dnl strings(1) would recognize for foreign objects, and it's
  dnl compatible with non-ASCII characters.
  AC_CACHE_CHECK([for $am_display_PYTHON version], [am_cv_python_version],
    [am_cv_python_version=`$PYTHON -c "import sys; sys.stdout.write(sys.version[[:3]])"`])
  AC_SUBST([PYTHON_VERSION], [$am_cv_python_version])

  dnl Use the values of $prefix and $exec_prefix for the corresponding
  dnl values of PYTHON_PREFIX and PYTHON_EXEC_PREFIX.  These are made
  dnl distinct variables so they can be overridden if need be.  However,
  dnl general consensus is that you shouldn't need this ability.

  AC_SUBST([PYTHON_PREFIX], ['${prefix}'])
  AC_SUBST([PYTHON_EXEC_PREFIX], ['${exec_prefix}'])

  dnl At times (like when building shared libraries) you may want to
  dnl know which OS platform Python thinks this is.

  AC_CACHE_CHECK([for $am_display_PYTHON platform], [am_cv_python_platform],
    [am_cv_python_platform=`$PYTHON -c "import sys; sys.stdout.write(sys.platform)"`])
  AC_SUBST([PYTHON_PLATFORM], [$am_cv_python_platform])

  dnl Set up 4 directories:

  dnl pythondir -- where to install python scripts.  This is the
  dnl   directory that Python code can use to locate python scripts.
  dnl   Also set in the standard way to install python scripts (by
  dnl   distutils) as $(PYTHON_PREFIX)/lib/python/site-packages
  dnl
  dnl pkgpythondir -- $(PACKAGE) subdirectory of pythondir.  Was
  dnl   $(PYTHON_SITE_PACKAGES)/$(PACKAGE) by default.  Now we can
  dnl   control the installation directory independently.  This becomes
  dnl   useful when people want to use $(PACKAGE) from a directory
  dnl   other than $(PYTHON_SITE_PACKAGES) and in some other
  dnl   situations.
  dnl
  dnl pyexecdir -- directory for installing python extension modules
  dnl   (shared libraries)
  dnl   Was $(PYTHON_EXEC_PREFIX)/lib/python/site-packages by default.
  dnl   Also controlled by PYTHON_EXEC_PREFIX.  This becomes useful
  dnl   when people want to use $(PACKAGE) from a directory other
  dnl   than $(PYTHON_EXEC_PREFIX)/lib/python/site-packages and in
  dnl   some other situations.
  dnl
  dnl pkgpyexecdir -- $(PACKAGE) subdirectory of pyexecdir
  dnl
  dnl For more details, see the python documentation:
  dnl http://docs.python.org/extending/installing.html
  dnl
  dnl If no PYTHON_PREFIX (and PYTHON_EXEC_PREFIX) are set, and
  dnl we are in GNU, and the installation is GNU
  dnl (PREFIX=/usr/local), then use relative paths for python
  dnl scripts and libraries.  Take the GNU coding standards into
  dnl account.  If the user has specified a prefix (including an
  dnl empty one) then use that and that only.
  dnl
  dnl Also, if the user specified --enable-shared, then use that
  dnl and that only.

  dnl bwells, 2002-01-07: logic is:
  dnl
  dnl  if there is no $PYTHON_PREFIX and $PYTHON_EXEC_PREFIX (unset)
  dnl  and we are using GNU values (prefix == /usr/local)
  dnl  and this is an installed package, not a CVS, etc, tree
  dnl  then use relative install paths, otherwise use absolute paths
  dnl
  dnl  the logic for pyexecdir is similar except that it depends on
  dnl  $PYTHON_EXEC_PREFIX which may have a different value.

  if test "x$prefix" = xNONE; then
    am__using_gnu_prefix=yes
  else
    if test "x$prefix" = x/usr/local; then
      am__using_gnu_prefix=yes
    else
      am__using_gnu_prefix=no
    fi
  fi

  if test "x$PYTHON_PREFIX" = xNONE; then
    am__py_prefix=$prefix
  else
    am__py_prefix=$PYTHON_PREFIX
  fi

  if test "x$PYTHON_EXEC_PREFIX" = xNONE; then
    am__py_exec_prefix=$exec_prefix
  else
    am__py_exec_prefix=$PYTHON_EXEC_PREFIX
  fi

  am__py_site_package="\${pyexecdir}"
  if test "x$prefix" = xNONE; then
    am__py_site_package="\${pythondir}"
  else
    if test "x$am__using_gnu_prefix" = xyes; then
      am__py_site_package="\${pythondir}"
    else
      am__py_site_package="\${pyexecdir}"
    fi
  fi

  AC_SUBST([pythondir], [$am__py_site_package])
  AC_SUBST([pkgpythondir], [\${pythondir}/$PACKAGE])

  AC_SUBST([pyexecdir], [$am__py_exec_prefix])
  AC_SUBST([pkgpyexecdir], [\${pyexecdir}/$PACKAGE])

  dnl Run any user-specified action.
  $2
  fi

])

# AM_PYTHON_CHECK_VERSION(PROG, VERSION, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ---------------------------------------------------------------------------
# Run ACTION-IF-TRUE if the Python interpreter PROG has version >= VERSION.
# Run ACTION-IF-FALSE otherwise.
# This test uses sys.hexversion instead of the string equivalent (first
# word of sys.version), in order to cope with version names such as
# "2.2c1" which may not be lexically greater than "2.2".
#
# This implementation of the macro is sensitive to that of the outer
# AC_TRY_LINK, the call to which is quoted from the Autoconf
# version.  It is OK to not quote, and change the AC_TRY_LINK calls
# to use the newer _AC_LINK_IFELSE function.
AC_DEFUN([AM_PYTHON_CHECK_VERSION],
 [prog="import sys
# split strings by '.' and convert to numeric.  Append some zeros
# because we need at least 4 digits for the hex conversion.
# map returns an iterator in Python 3.0 and a list in 2.x
minver = list(map(int, '$2'.split('.'))) + [[0, 0, 0]]
minverhex = 0
# xrange is not present in Python 3.0 (range is, though).
for i in list(range(0, 4)): minverhex = (minverhex << 8) + minver[[i]]
sys.exit(sys.hexversion < minverhex)"
  AS_IF([AM_RUN_LOG([$1 -c "$prog"])], [$3], [$4])]) 