/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include "php_foolphp.h"

#include "fool_application.h"
#include "fool_dispatcher.h"
#include "fool_loader.h"
#include "fool_config.h"


zend_class_entry* fool_application_ce;

/*ZEND_BEGIN_ARG_INFO_EX(name, pass_rest_by_reference, return_reference, required_num_args)*/
ZEND_BEGIN_ARG_INFO_EX(fool_application_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_application_getinstance_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, application_path)
	ZEND_ARG_INFO(0, config_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_application_run_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()


/*{{{ zval* fool_application_instance(TSRMLS_D)
 */
zval* fool_application_instance(TSRMLS_D)
{
	zval* instance;
	zval* dispatcher;
	zval* loader;

	instance = zend_read_static_property(fool_application_ce,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);
		return instance;
	}

	object_init_ex(instance,fool_application_ce);

	zend_update_static_property(fool_application_ce,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		//init dispatcher
		dispatcher = fool_dispatcher_instance(TSRMLS_C);
		if(!dispatcher){
			return NULL;
		}
	
		loader = fool_loader_instance(TSRMLS_C);	
		if(!loader){
			return NULL;
		}
		fool_loader_register_autoload(loader TSRMLS_CC);

		zend_update_property(fool_application_ce,instance,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_DISPATCHER),dispatcher TSRMLS_CC);
		
		//init config
		fool_config_init(TSRMLS_C);
		//init object
		fool_object_instance(TSRMLS_C);

		return instance;
	}
	return NULL;
}
/*}}}*/

/*{{{ Fool_Application::__construct(void)
 */
PHP_METHOD(fool_application,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Application::getInstance(string $application_path,string $config_path)
 */
PHP_METHOD(fool_application,getInstance)
{
	char* application_path;
	char* config_path;
	int application_path_len;
	int config_path_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ss",&application_path,&application_path_len,&config_path,&config_path_len) == FAILURE){
		fool_throw_exception(104,"[Warn]Fool_Application::getInstance(string $application_path,string $config_path)" TSRMLS_CC);
		RETURN_FALSE;
	}
	
	FOOLPHP_G(application_path) = estrndup(application_path,application_path_len);
	FOOLPHP_G(config_path)      = estrndup(config_path,config_path_len);

	zval* instance = fool_application_instance(TSRMLS_C);
	
	if(instance){
		RETURN_ZVAL(instance,1,0);
	}
	RETURN_FALSE;
}
/*}}}*/

/*{{{ public Fool_Application::run(void)
 */
PHP_METHOD(fool_application,run)
{
	zval* response;
	zval* dispatcher;

	zval* self = getThis();


	dispatcher = zend_read_property(fool_application_ce,self,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_DISPATCHER),1 TSRMLS_CC);
	if((response = fool_dispatcher_dispatch(dispatcher TSRMLS_CC))){
		RETURN_ZVAL(response,1,1);
	}
	RETURN_FALSE;
}
/*}}}*/


zend_function_entry fool_application_methods[] = {
	ZEND_ME(fool_application,__construct,fool_application_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_application,getInstance,fool_application_getinstance_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(fool_application,run,fool_application_run_arginfo,ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};

FOOL_STARTUP_MODULE(application)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Application",fool_application_methods);

	fool_application_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_application_ce,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_INSTANCE),ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_application_ce,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_DISPATCHER),ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(fool_application_ce,ZEND_STRL(FOOL_APPLICATION_PROPERTY_NAME_LOADER),ZEND_ACC_PRIVATE TSRMLS_CC);

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
