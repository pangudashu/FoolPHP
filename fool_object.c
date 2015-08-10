#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_interfaces.h"
#include "php_foolphp.h"

#include "fool_object.h"
#include "fool_dispatcher.h"

zend_class_entry* fool_object_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_object_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_object_find_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()


/**{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_object_instance(TSRMLS_D)
{
	zval* instance;
	zval* obj_list;

	instance = zend_read_static_property(fool_object_ce,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  //?????????????????
		return instance;
	}

	object_init_ex(instance,fool_object_ce);

	zend_update_static_property(fool_object_ce,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		//初始化obj_list
		MAKE_STD_ZVAL(obj_list);
		array_init(obj_list);

		//add_next_index_long(obj_list,12345678);
		zend_update_property(fool_object_ce,instance,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_LIST),obj_list TSRMLS_CC);

		return instance;
	}
}
/*}}}*/

/*{{{ zval* fool_object_get(zval* object,char* class_name,int name_len TSRMLS_DC)
 */
zval* fool_object_get(zval* object,char* class_name,int name_len TSRMLS_DC)
{
	zval* obj_list = zend_read_property(fool_object_ce,object,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_LIST),1 TSRMLS_CC);
	if(!obj_list){
		return NULL;
	}

	zval** _obj;
	if(zend_hash_find(Z_ARRVAL_P(obj_list),class_name,name_len+1,(void**)&_obj) == FAILURE){
		return NULL;
	}
	return *_obj;
}
/*}}}*/

/*{{{ int fool_object_set(zval* object,char* class_name,int name_len,zval* value TSRMLS_DC)
 */
int fool_object_set(zval* object,char* class_name,int name_len,zval* value TSRMLS_DC)
{
	zval* obj_list = zend_read_property(fool_object_ce,object,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_LIST),1 TSRMLS_CC);
	if(!obj_list){
		return 0;
	}

	Z_ADDREF_P(value);	
	zend_hash_add(Z_ARRVAL_P(obj_list),class_name,name_len + 1, &value, sizeof(zval*), NULL);
	return 1;
}
/*}}}*/

/*{{{ public Fool_Object::__construct(void)
 */
PHP_METHOD(fool_object,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Object::find(string $class)
 */
PHP_METHOD(fool_object,find)
{
	char* class_name;
	int class_name_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&class_name,&class_name_len) == FAILURE){
		RETURN_FALSE;
	}

	zval* object = zend_read_static_property(fool_object_ce,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);
	zval* find_obj;
	find_obj = fool_object_get(object,class_name,class_name_len);

	if(find_obj){
		RETURN_ZVAL(find_obj,1,0);
	}

	zend_class_entry* find_class_ce = fool_dispatcher_get_controller(class_name TSRMLS_CC);
	if(!find_class_ce){
		RETURN_FALSE;
	}
	MAKE_STD_ZVAL(find_obj);
	object_init_ex(find_obj,find_class_ce);

	//save object
	fool_object_set(object,class_name,class_name_len,find_obj);

	RETURN_ZVAL(find_obj,1,0);
}
/*}}}*/


zend_function_entry fool_object_methods[] = {
	ZEND_ME(fool_object,__construct,fool_object_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_object,find,fool_object_find_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP_MODULE(object)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Object",fool_object_methods);

	fool_object_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_object_ce,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_object_ce,ZEND_STRL(FOOL_OBJECT_PROPERTY_NAME_LIST),ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);

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
