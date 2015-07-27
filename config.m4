dnl $Id$
dnl config.m4 for extension foolphp

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(foolphp, for foolphp support,
Make sure that the comment is aligned:
[  --with-foolphp             Include foolphp support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(foolphp, whether to enable foolphp support,
dnl Make sure that the comment is aligned:
dnl [  --enable-foolphp           Enable foolphp support])

if test "$PHP_FOOLPHP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-foolphp -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/foolphp.h"  # you most likely want to change this
  dnl if test -r $PHP_FOOLPHP/$SEARCH_FOR; then # path given as parameter
  dnl   FOOLPHP_DIR=$PHP_FOOLPHP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for foolphp files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       FOOLPHP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$FOOLPHP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the foolphp distribution])
  dnl fi

  dnl # --with-foolphp -> add include path
  dnl PHP_ADD_INCLUDE($FOOLPHP_DIR/include)

  dnl # --with-foolphp -> check for lib and symbol presence
  dnl LIBNAME=foolphp # you may want to change this
  dnl LIBSYMBOL=foolphp # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FOOLPHP_DIR/lib, FOOLPHP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_FOOLPHPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong foolphp lib version or lib not found])
  dnl ],[
  dnl   -L$FOOLPHP_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(FOOLPHP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(foolphp, foolphp.c fool_application.c fool_dispatcher.c fool_loader.c fool_request.c fool_controller.c fool_object.c fool_view.c fool_config.c fool_exception.c, $ext_shared)
fi
