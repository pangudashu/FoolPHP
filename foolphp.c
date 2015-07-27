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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_foolphp.h"

#include "fool_application.h"
#include "fool_dispatcher.h"
#include "fool_loader.h"
#include "fool_request.h"
#include "fool_controller.h"

ZEND_DECLARE_MODULE_GLOBALS(foolphp);

const zend_function_entry foolphp_functions[] = {
	PHP_FE_END	/* Must be the last line in fool_functions[] */
};

zend_module_entry foolphp_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"foolphp",
	foolphp_functions,
	PHP_MINIT(foolphp),
	PHP_MSHUTDOWN(foolphp),
	PHP_RINIT(foolphp),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(foolphp),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(foolphp),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_FOOLPHP_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FOOLPHP
ZEND_GET_MODULE(foolphp)
#endif


/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("foolphp.class_map","fool_php_class.map", PHP_INI_ALL,OnUpdateString,class_map, zend_foolphp_globals, foolphp_globals)
    STD_PHP_INI_ENTRY("foolphp.config_name","config.inc.php", PHP_INI_ALL,OnUpdateString,config_name, zend_foolphp_globals, foolphp_globals)
PHP_INI_END();
/* }}} */

/* {{{ PHP_GINIT_FUNCTION(fool)
 */
PHP_GINIT_FUNCTION(foolphp)
{
	foolphp_globals->application_path = NULL;
	foolphp_globals->config_path = NULL;
}
/* }}} */

PHP_MINIT_FUNCTION(foolphp)
{
	//register php.ini
	REGISTER_INI_ENTRIES();

	FOOL_STARTUP(application);
	FOOL_STARTUP(dispatcher);
	FOOL_STARTUP(loader);
	FOOL_STARTUP(request);
	FOOL_STARTUP(controller);
	FOOL_STARTUP(object);
	FOOL_STARTUP(view);
	FOOL_STARTUP(config);
	FOOL_STARTUP(exception);

	return SUCCESS;
}

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(foolphp)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(foolphp)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(foolphp)
{
	if(FOOLPHP_G(application_path)){
		efree(FOOLPHP_G(application_path));
	}
	if(FOOLPHP_G(config_path)){
		efree(FOOLPHP_G(config_path));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(foolphp)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Foolphp Support", "enabled");
	php_info_print_table_header(2, "Foolphp Version", PHP_FOOLPHP_VERSION);
	php_info_print_table_header(2, "Foolphp Class_map", FOOLPHP_G(class_map));
	php_info_print_table_header(2, "Foolphp Config_name", FOOLPHP_G(config_name));
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
