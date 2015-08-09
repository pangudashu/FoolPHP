#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "php_foolphp.h"

#include "fool_dispatcher.h"
#include "fool_request.h"
#include "fool_controller.h"
#include "fool_view.h"
#include "fool_exception.h"

zend_class_entry* fool_dispatcher_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_dispatcher_getinstance_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_dispatcher_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()


/**{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_dispatcher_instance(TSRMLS_D)
{
	zval* instance;

	instance = zend_read_static_property(fool_dispatcher_ce,ZEND_STRL(FOOL_DISPATCHER_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  //?????????????????
		return instance;
	}

	object_init_ex(instance,fool_dispatcher_ce);

	zend_update_static_property(fool_dispatcher_ce,ZEND_STRL(FOOL_DISPATCHER_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		return instance;
	}
}
/*}}}*/

/*{{{ zend_class_entry* fool_dispatcher_get_controller(char* controller_name TSRMLS_DC)
 */
zend_class_entry* fool_dispatcher_get_controller(char* controller_name TSRMLS_DC)
{
	zend_class_entry **ce = NULL;
	int clen = strlen(controller_name);
	char* controller_name_lower = estrdup(controller_name);
	
	php_strtolower(controller_name_lower,clen);

	if(zend_hash_find(EG(class_table),controller_name_lower,clen+1,(void**)&ce TSRMLS_CC) == SUCCESS){
		return *ce;
	}

	fool_loader_autoload(controller_name TSRMLS_CC);

	if(zend_hash_find(EG(class_table),controller_name_lower,clen+1,(void**)&ce TSRMLS_CC) == SUCCESS){
		return *ce;
	}

	efree(controller_name_lower);
	return NULL;
}
/*}}}*/

/*{{{ zval* fool_dispatcher_dispatch(zval* dispatcher TSRMLS_DC)
 */
zval* fool_dispatcher_dispatch(zval* dispatcher TSRMLS_DC)
{
	zval* response;
	zval* request;
	zval* view;

	request = fool_request_instance(TSRMLS_C);
	if(!request){
		fool_throw_exception(101,"[Error]Can't get the instance of Fool_Request" TSRMLS_CC);
		return NULL;
	}
	zend_update_property(fool_dispatcher_ce,dispatcher,ZEND_STRL(FOOL_DISPATCHER_PROPERTY_NAME_REQUEST),request TSRMLS_CC);
	
	int parse_method_res = fool_request_parse_method(request TSRMLS_CC);
	if(!parse_method_res){
		fool_throw_exception(102,"[Error]Can't parse the variable of GET['method']" TSRMLS_CC);
		return NULL;
	}
	//init Fool_View
	view = fool_view_instance(TSRMLS_C);

	zval *controller_name,*controller,*action;
	zend_class_entry* ce;

	controller_name = fool_request_get_controller(request TSRMLS_CC);
	ce = fool_dispatcher_get_controller(Z_STRVAL_P(controller_name) TSRMLS_CC);

	if(!ce){
		char* msg;
		spprintf(&msg,0,"[Error]Can't find the class of condtroller : %s",Z_STRVAL_P(controller_name));
		fool_throw_exception(100,msg TSRMLS_CC);
		efree(msg);
		return NULL;
	}

	MAKE_STD_ZVAL(controller);
	//实例化对象
	object_init_ex(controller, ce);

	//action handler
	fool_controller_dispatch_handler(ce,controller,request,view TSRMLS_CC);

	zval_ptr_dtor(&controller);
	return NULL;//response;
}
/*}}}*/

/*{{{ public Fool_Application::getInstance(void)
 */
PHP_METHOD(fool_dispatcher,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Application::getInstance(void)
 */
PHP_METHOD(fool_dispatcher,getInstance)
{
	zval* instance = fool_dispatcher_instance(TSRMLS_C);
	
	if(instance){
		RETURN_ZVAL(instance,1,0);
	}
	RETURN_FALSE;
}
/*}}}*/


zend_function_entry fool_dispatcher_methods[] = {
	ZEND_ME(fool_dispatcher,__construct,fool_dispatcher_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_dispatcher,getInstance,fool_dispatcher_getinstance_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP(dispatcher)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Dispatcher",fool_dispatcher_methods);

	fool_dispatcher_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_dispatcher_ce,ZEND_STRL(FOOL_DISPATCHER_PROPERTY_NAME_INSTANCE),ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_dispatcher_ce,ZEND_STRL(FOOL_DISPATCHER_PROPERTY_NAME_REQUEST),ZEND_ACC_PRIVATE TSRMLS_CC);

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
