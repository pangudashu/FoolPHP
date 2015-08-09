#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "php_foolphp.h"

#include "fool_request.h"

zend_class_entry* fool_request_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_request_getinstance_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_request_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_request_getcontroller_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_request_getaction_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

/**{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_request_instance(TSRMLS_D)
{
	zval* instance;

	instance = zend_read_static_property(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  //?????????????????
		return instance;
	}

	object_init_ex(instance,fool_request_ce);

	zend_update_static_property(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		return instance;
	}
}
/*}}}*/

/*{{{ int fool_request_parse_method(zval* request TSRMLS_DC)
 * 根据$_GET[FOOL_REQUEST_METHOD_PARAM]解析controller、action
 */
int fool_request_parse_method(zval* request TSRMLS_DC)
{
	zval** value;
	zval** get_arr;

	zval zdelim, zstr;
	char* c = NULL;
	char* a = NULL;
	
	zval** zc;
	zval** za;

	if(zend_hash_find(&EG(symbol_table),"_GET",sizeof("_GET"),(void**)&get_arr) == FAILURE){
		return 0;
	}

	if(zend_hash_find(Z_ARRVAL_PP(get_arr),FOOL_REQUEST_METHOD_PARAM,strlen(FOOL_REQUEST_METHOD_PARAM)+1,(void**)&value) == FAILURE || IS_STRING != Z_TYPE_PP(value)){
		//set default controller & action
		c = emalloc(strlen(FOOL_REQUEST_CONTROLLER_DEFAULT) + strlen(FOOL_REQUEST_CONTROLLER_SUFIX) + 1);
		strcpy(c,FOOL_REQUEST_CONTROLLER_DEFAULT);

		a = emalloc(strlen(FOOL_REQUEST_ACTION_DEFAULT) + strlen(FOOL_REQUEST_ACTION_SUFIX) + 1);
		strcpy(a,FOOL_REQUEST_ACTION_DEFAULT);
	}else{
		zval* res;
		MAKE_STD_ZVAL(res);

		array_init(res);

		ZVAL_STRINGL(&zstr,Z_STRVAL_PP(value),strlen(Z_STRVAL_PP(value)),0);
		ZVAL_STRINGL(&zdelim,FOOL_REQUEST_SEPARATOR,strlen(FOOL_REQUEST_SEPARATOR),0);

		//php function explode
		php_explode(&zdelim, &zstr, res, 3/*LONG_MAX*/);

		int arg_num = zend_hash_num_elements(Z_ARRVAL_P(res));

		if(arg_num == 1){
			c = emalloc(strlen(Z_STRVAL_PP(value)) + strlen(FOOL_REQUEST_CONTROLLER_SUFIX) + 1);
			strcpy(c,Z_STRVAL_PP(value));

			a = emalloc(strlen(FOOL_REQUEST_ACTION_DEFAULT) + strlen(FOOL_REQUEST_ACTION_SUFIX) + 1);
			strcpy(a,FOOL_REQUEST_ACTION_DEFAULT);
		}else{
			zend_hash_index_find(Z_ARRVAL_P(res),0,(void**)&zc);
			zend_hash_index_find(Z_ARRVAL_P(res),1,(void**)&za);

			c = emalloc(strlen(Z_STRVAL_PP(zc)) + strlen(FOOL_REQUEST_CONTROLLER_SUFIX) + 1);
			strcpy(c,Z_STRVAL_PP(zc));

			a = emalloc(strlen(Z_STRVAL_PP(za)) + strlen(FOOL_REQUEST_ACTION_SUFIX) + 1);
			strcpy(a,Z_STRVAL_PP(za));
		}
		zval_ptr_dtor(&res);

		//将method param从符号表中删除
		zend_hash_del(Z_ARRVAL_PP(get_arr),FOOL_REQUEST_METHOD_PARAM,strlen(FOOL_REQUEST_METHOD_PARAM)+1);
	}

	c = php_strtolower(c,strlen(c));
	*c = toupper((unsigned char) *c);
	a = php_strtolower(a,strlen(a));

	char* c_name = estrdup(c);
	char* a_name = estrdup(a);

	//添加后缀	
	strcat(c,FOOL_REQUEST_CONTROLLER_SUFIX);
	strcat(a,FOOL_REQUEST_ACTION_SUFIX);

	zend_update_property_string(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER),c);
	zend_update_property_string(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION),a);
	zend_update_property_string(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER_NAME),c_name);
	zend_update_property_string(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION_NAME),a_name);

	efree(c_name);
	efree(a_name);
	efree(c);
	efree(a);

	return 1;
}
/*}}}*/

/*{{{ zval* fool_request_get_controller(zval* request TSRMLS_DC)
 */
zval* fool_request_get_controller(zval* request TSRMLS_DC)
{
	zval* controller = zend_read_property(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER),1 TSRMLS_CC);
	return controller;
}
/*}}}*/

/*{{{ zval* fool_request_get_controller_name(zval* request TSRMLS_DC)
 */
zval* fool_request_get_controller_name(zval* request TSRMLS_DC)
{
	zval* controller = zend_read_property(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER_NAME),1 TSRMLS_CC);
	return controller;
}
/*}}}*/

/*{{{ zval* fool_request_get_action(zval* request TSRMLS_DC)
 */
zval* fool_request_get_action(zval* request TSRMLS_DC)
{
	zval* action = zend_read_property(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION),1 TSRMLS_CC);
	return action;
}
/*}}}*/

/*{{{ zval* fool_request_get_action_name(zval* request TSRMLS_DC)
 */
zval* fool_request_get_action_name(zval* request TSRMLS_DC)
{
	zval* action = zend_read_property(fool_request_ce,request,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION_NAME),1 TSRMLS_CC);
	return action;
}
/*}}}*/

/*{{{ public Fool_Requst::__construct(void)
 */
PHP_METHOD(fool_request,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Request::getInstance(void)
 */
PHP_METHOD(fool_request,getInstance)
{
	zval* instance = fool_request_instance(TSRMLS_C);
	
	if(instance){
		RETURN_ZVAL(instance,1,0);
	}
	RETURN_FALSE;
}
/*}}}*/

/*{{{ public Fool_Reqest::getController(void)
 */
PHP_METHOD(fool_request,getController)
{
	zval* controller = fool_request_get_controller(getThis() TSRMLS_CC);
	RETURN_ZVAL(controller,1,0);
}
/*}}}*/

/*{{{ public Fool_Request::getAction(void)
 */
PHP_METHOD(fool_request,getAction)
{
	zval* action = fool_request_get_action(getThis() TSRMLS_CC);
	RETURN_ZVAL(action,1,0);
}
/*}}}*/


zend_function_entry fool_request_methods[] = {
	ZEND_ME(fool_request,__construct,fool_request_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_request,getInstance,fool_request_getinstance_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(fool_request,getController,fool_request_getcontroller_arginfo,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_request,getAction,fool_request_getaction_arginfo,ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP(request)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Request",fool_request_methods);

	fool_request_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER),ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION),ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_CONTROLLER_NAME),ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(fool_request_ce,ZEND_STRL(FOOL_REQUEST_PROPERTY_NAME_ACTION_NAME),ZEND_ACC_PRIVATE TSRMLS_CC);

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
