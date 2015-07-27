/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include "php_foolphp.h"

#include "fool_exception.h"


zend_class_entry* fool_exception_ce;

/*#define ZEND_BEGIN_ARG_INFO_EX(name, pass_rest_by_reference, return_reference, required_num_args)*/
ZEND_BEGIN_ARG_INFO_EX(fool_exception_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

void fool_throw_exception(long code,char* msg TSRMLS_DC)
{
	zend_throw_exception(fool_exception_ce,msg,code TSRMLS_CC);
}

/*{{{ zend_class_entry * fool_get_exception_base(int root TSRMLS_DC)
 * copy from yaf
 */
zend_class_entry * fool_get_exception_base(int root TSRMLS_DC) {
#if can_handle_soft_dependency_on_SPL && defined(HAVE_SPL) && ((PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1))
	if (!root) {
		if (!spl_ce_RuntimeException) {
			zend_class_entry **pce;
			if (zend_hash_find(CG(class_table), "runtimeexception", sizeof("RuntimeException"), (void **) &pce) == SUCCESS) {
				spl_ce_RuntimeException = *pce;
				return *pce;
			}
		} else {
		return spl_ce_RuntimeException;
		}
	}
#endif

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	return zend_exception_get_default();
#else
	return zend_exception_get_default(TSRMLS_C);
#endif
}
/*}}}*/


/*{{{ Fool_Application::__construct(void)
 */
PHP_METHOD(fool_exception,__construct)
{
}
/*}}}*/


zend_function_entry fool_exception_methods[] = {
	ZEND_ME(fool_exception,__construct,fool_exception_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};

FOOL_STARTUP(exception)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Exception",fool_exception_methods);

	fool_exception_ce = zend_register_internal_class_ex(&ce,fool_get_exception_base(0 TSRMLS_CC),NULL TSRMLS_CC);

	zend_declare_property_null(fool_exception_ce,ZEND_STRL(FOOL_EXCEPTION_PROPERTY_NAME_INSTANCE),ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);

	return SUCCESS;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
