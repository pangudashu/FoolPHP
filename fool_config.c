#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_interfaces.h"
#include "php_foolphp.h"

#include "fool_config.h"
#include "fool_dispatcher.h"

zend_class_entry* fool_config_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_config_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fool_config_find_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()


/*{{{ int fool_config_get_defined_vars(char* config_path,zval* var_list TSRMLS_DC)
 */
int fool_config_get_defined_vars(char* config_path,zval* var_list TSRMLS_DC)
{
	HashTable *calling_symbol_table;
	//保存当前符号表
	if (EG(active_symbol_table)) {
		calling_symbol_table = EG(active_symbol_table);
	} else {
		calling_symbol_table = NULL;
	}

	//重置符号表
	ALLOC_HASHTABLE(EG(active_symbol_table));
	zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

	int res = fool_loader_include(config_path);

	/*遍历active_symbol_table存入var_list*/
	zval **entry;
	char *var_name;
	long num_key;
	uint var_name_len;
	HashPosition pos;

	for(zend_hash_internal_pointer_reset_ex(EG(active_symbol_table), &pos);
		zend_hash_get_current_data_ex(EG(active_symbol_table), (void **)&entry, &pos) == SUCCESS;
		zend_hash_move_forward_ex(EG(active_symbol_table), &pos)) {
		
		if (zend_hash_get_current_key_ex(EG(active_symbol_table), &var_name, &var_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
			continue;
		}

		ZEND_SET_SYMBOL_WITH_LENGTH(Z_ARRVAL_P(var_list), var_name, var_name_len,
			*entry, Z_REFCOUNT_P(*entry) + 1, PZVAL_IS_REF(*entry));
	}
	/*end*/

	if (calling_symbol_table) {
		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = calling_symbol_table;
	}

	return 1;
}
/*}}}*/

/**{{{ int fool_config_init(TSRMLS_D)
 */
int fool_config_init(TSRMLS_D)
{
	zval* instance = NULL;
	zval* var_list;

	instance = zend_read_static_property(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(instance && Z_BVAL_P(instance)){
		return 1;
	}

	ZVAL_TRUE(instance);
	zend_update_static_property(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);
	
	if(instance){
		//初始化obj_list
		MAKE_STD_ZVAL(var_list);
		array_init(var_list);

		char* config_path;
		spprintf(&config_path,0,"%s/%s",FOOLPHP_G(config_path),FOOLPHP_G(config_name));
	
		fool_config_get_defined_vars(config_path,var_list TSRMLS_CC);

		zend_update_static_property(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_VAR),var_list TSRMLS_CC);

		efree(config_path);
	}
	return 1;
}
/*}}}*/

/*{{{ zval* fool_config_get(zval* object,char* class_name,int name_len TSRMLS_DC)
 */
zval* fool_config_get(zval* object,char* class_name,int name_len TSRMLS_DC)
{
	zval* obj_list = zend_read_property(fool_config_ce,object,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_VAR),1 TSRMLS_CC);
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

/*{{{ public Fool_Config::__construct(void)
 */
PHP_METHOD(fool_config,__construct)
{
}
/*}}}*/

/*{{{ public Fool_Config::find(string $class)
 */
PHP_METHOD(fool_config,find)
{
	char* var_name;
	int var_name_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&var_name,&var_name_len) == FAILURE){
		RETURN_FALSE;
	}

	zval* config = zend_read_static_property(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_VAR),1 TSRMLS_CC);
	if(!config){
		RETURN_FALSE;
	}


	zval** value;
	if(zend_hash_find(Z_ARRVAL_P(config),var_name,var_name_len+1,(void**)&value) == FAILURE){
		RETURN_FALSE;
	}
	RETURN_ZVAL(*value,1,0);
}
/*}}}*/


zend_function_entry fool_config_methods[] = {
	ZEND_ME(fool_config,__construct,fool_config_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_config,find,fool_config_find_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP(config)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Config",fool_config_methods);

	fool_config_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_config_ce,ZEND_STRL(FOOL_CONFIG_PROPERTY_NAME_VAR),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);

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
