#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_interfaces.h"
#include "php_foolphp.h"

#include "fool_controller.h"
#include "fool_request.h"
#include "fool_exception.h"

zend_class_entry* fool_controller_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_controller_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_controller_getcontroller_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_controller_getmethod_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_controller_assign_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/*{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_controller_instance(TSRMLS_D)
{
	zval* instance;

	instance = zend_read_static_property(fool_controller_ce,ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  //?????????????????
		return instance;
	}

	object_init_ex(instance,fool_controller_ce);

	zend_update_static_property(fool_controller_ce,ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		return instance;
	}
}
/*}}}*/

/*{{{ public Fool_Controller::__construct(void)
 */
PHP_METHOD(fool_controller,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Controller::getInstance(void)
 */
/*
PHP_METHOD(fool_controller,getInstance)
{
	zval* instance = fool_controller_instance(TSRMLS_C);
	
	if(instance){
		RETURN_ZVAL(instance,1,0);
	}
	RETURN_FALSE;
}
*/
/*}}}*/

/*{{{ int fool_controller_dispatch_handler(zend_class_entry* ce,zval* controller,zval* request,zval* view TSRMLS_DC)
 */
int fool_controller_dispatch_handler(zend_class_entry* ce,zval* controller,zval* request,zval* view TSRMLS_DC)
{
	zend_update_property(fool_controller_ce,controller, ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),request TSRMLS_CC);
	zend_update_property(fool_controller_ce,controller, ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_VIEW),view TSRMLS_CC);

	zval* action = fool_request_get_action(request TSRMLS_CC);
	char* action_lower = estrdup(Z_STRVAL_P(action));

	php_strtolower(action_lower,strlen(action_lower));


	if(!zend_hash_exists(&(ce->function_table),action_lower,strlen(action_lower)+1)){
		efree(action_lower);

		char* msg;
		spprintf(&msg,0,"[Error]Can't find function %s in class %s",Z_STRVAL_P(action),ce->name);
		fool_throw_exception(103,msg TSRMLS_CC);
		efree(msg);

		return 0;
	}
	zval* ret;
	zval* method_name;
	MAKE_STD_ZVAL(method_name);
	ZVAL_STRING(method_name,action_lower,0);


	zend_call_method_with_0_params(&controller,ce,NULL,"predispatch",NULL);

	do{
		if(zend_hash_exists(&(ce->function_table),"__construct",strlen("__construct") + 1)){
			zend_call_method_with_0_params(&controller,ce,NULL,"__construct",NULL);
		}

		call_user_function_ex(&(ce)->function_table,&controller,method_name,&ret,0,NULL,1, NULL TSRMLS_CC);

		if(zend_hash_exists(&(ce->function_table),"__destruct",strlen("__destruct") + 1)){
			zend_call_method_with_0_params(&controller,ce,NULL,"__destruct",NULL);
		}
	}while(0);

	zend_call_method_with_0_params(&controller,ce,NULL,"afterdispatch",NULL);

	efree(action_lower);
	zval_ptr_dtor(&method_name);

	return 1;
}
/*}}}*/

/*{{{ public Fool_Controller::getController(void)
 */
PHP_METHOD(fool_controller,getController)
{
	zval* request = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),1 TSRMLS_CC);
	zval* res = fool_request_get_controller(request TSRMLS_CC);

	RETURN_STRING(Z_STRVAL_P(res),1);
}
/*}}}*/

/*{{{ public Fool_Controller::getMethod(void)
 */
PHP_METHOD(fool_controller,getMethod)
{
	zval* request = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),1 TSRMLS_CC);
	zval* res = fool_request_get_action(request TSRMLS_CC);

	RETURN_STRING(Z_STRVAL_P(res),1);
}
/*}}}*/

/*{{{ public Fool_Controller::preDispatch(void)
 * 前置操作
 */
PHP_METHOD(fool_controller,preDispatch)
{
}
/*}}}*/

/*{{{ public Fool_Controller::afterDispatch(void)
 * 后置操作
 */
PHP_METHOD(fool_controller,afterDispatch)
{
}
/*}}}*/

/*{{{ public Fool_controller::assign(string $key,void $value)
 */
PHP_METHOD(fool_controller,assign)
{
	char* key;
	int len;
	zval* value;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"sz",&key,&len,&value) == FAILURE){
		RETURN_FALSE;
	}

	zval* view = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_VIEW),1 TSRMLS_CC);
	int res = fool_view_assign(view,key,len,value TSRMLS_CC);

	if(res){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}
/*}}}*/

/*{{{ public Fool_Controller::render([string $tpl_script])
 */
PHP_METHOD(fool_controller,render)
{
	char* script = NULL;
	int len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"|s",&script,&len) == FAILURE){
		RETURN_FALSE;
	}

	if(!script){
		zval* request = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),1 TSRMLS_CC);
		zval* controller_name = fool_request_get_controller_name(request TSRMLS_CC);
		zval* action_name = fool_request_get_action_name(request TSRMLS_CC);
		spprintf(&script,0,"%s/%s",Z_STRVAL_P(controller_name),Z_STRVAL_P(action_name));
	}

	zval* view = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_VIEW),1 TSRMLS_CC);
	fool_view_render(view,script,return_value,0 TSRMLS_CC);

	efree(script);

	RETURN_ZVAL(return_value,1,0);
}
/*}}}*/

/*{{{ public Fool_Controller::display([string $tpl_script])
 */
PHP_METHOD(fool_controller,display)
{
	char* script = NULL;
	int len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"|s",&script,&len) == FAILURE){
		RETURN_FALSE;
	}

	if(!script){
		zval* request = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),1 TSRMLS_CC);
		zval* controller_name = fool_request_get_controller_name(request TSRMLS_CC);
		zval* action_name = fool_request_get_action_name(request TSRMLS_CC);
		spprintf(&script,0,"%s/%s",Z_STRVAL_P(controller_name),Z_STRVAL_P(action_name));
	}

	zval* view = zend_read_property(fool_controller_ce,getThis(),ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_VIEW),1 TSRMLS_CC);
	fool_view_render(view,script,return_value,1 TSRMLS_CC);

	efree(script);

	RETURN_ZVAL(return_value,1,0);
}
/*}}}*/


zend_function_entry fool_controller_methods[] = {
	ZEND_ME(fool_controller,__construct,fool_controller_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_controller,getController,fool_controller_getcontroller_arginfo,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,getMethod,fool_controller_getmethod_arginfo,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,preDispatch,NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,afterDispatch,NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,assign,fool_controller_assign_arginfo,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,render,NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_controller,display,NULL,ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP_MODULE(controller)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Controller",fool_controller_methods);

	fool_controller_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_controller_ce,ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_controller_ce,ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_REQUEST),ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(fool_controller_ce,ZEND_STRL(FOOL_CONTROLLER_PROPERTY_NAME_VIEW),ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(fool_controller_ce,ZEND_STRL("view_path"),ZEND_ACC_PRIVATE);

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
