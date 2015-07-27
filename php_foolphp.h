/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FOOLPHP_H
#define PHP_FOOLPHP_H

extern zend_module_entry foolphp_module_entry;
#define phpext_foolphp_ptr &foolphp_module_entry

#define PHP_FOOLPHP_VERSION "1.0.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_FOOLPHP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_FOOLPHP_API __attribute__ ((visibility("default")))
#else
#	define PHP_FOOLPHP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define FOOL_STARTUP(module) ZEND_MODULE_STARTUP_N(foolphp_##module)(INIT_FUNC_ARGS_PASSTHRU)

#define FOOL_CLASS_MAP_VAR "fool_php_class_map"


PHP_MINIT_FUNCTION(foolphp);
PHP_MSHUTDOWN_FUNCTION(foolphp);
PHP_RINIT_FUNCTION(foolphp);
PHP_RSHUTDOWN_FUNCTION(foolphp);
PHP_MINFO_FUNCTION(foolphp);




ZEND_BEGIN_MODULE_GLOBALS(foolphp)
	char *application_path;
	char *config_path;
	char *class_map;
	char *config_name;
ZEND_END_MODULE_GLOBALS(foolphp)

extern ZEND_DECLARE_MODULE_GLOBALS(foolphp);
extern PHPAPI void php_debug_zval_dump(zval **struc, int level TSRMLS_DC);




/* In every utility function you add that needs to use variables 
   in php_foolphp_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as FOOLPHP_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define FOOLPHP_G(v) TSRMG(foolphp_globals_id, zend_foolphp_globals *, v)
#else
#define FOOLPHP_G(v) (foolphp_globals.v)
#endif

#endif	/* PHP_FOOLPHP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
