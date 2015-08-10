/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_foolphp.h"

#include "fool_loader.h"

zend_class_entry* fool_loader_ce;

/*{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_loader_instance(TSRMLS_D)
{
	zval* instance;

	instance = zend_read_static_property(fool_loader_ce,ZEND_STRL(FOOL_LOADER_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  ?????????????????
		return instance;
	}

	object_init_ex(instance,fool_loader_ce);

	zend_update_static_property(fool_loader_ce,ZEND_STRL(FOOL_LOADER_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	char* include_path;// = emalloc(strlen(FOOL_G(class_map)) + strlen(FOOL_G(config_path)) + 2);
	
	spprintf(&include_path,0,"%s/%s",FOOLPHP_G(config_path),FOOLPHP_G(class_map));

	if(instance){
		//include class_map
		fool_loader_include(include_path TSRMLS_CC);
		efree(include_path);

		return instance;
	}
	efree(include_path);
	return NULL;
}
/*}}}*/

/*{{{ int fool_loader_register_autoload(zval* loader TSRMLS_DC)
 */
int fool_loader_register_autoload(zval* loader TSRMLS_DC)
{
	//zval* this = fool_loader_instance(TSRMLS_C);
	zval *autoload,*function,*ret = NULL;
	zval **params[1] = {&autoload};
	zval* method_name;

	MAKE_STD_ZVAL(autoload);

	array_init(autoload);

	MAKE_STD_ZVAL(method_name);
	ZVAL_STRING(method_name,"autoload",1);


	Z_ADDREF_P(autoload);
	add_next_index_zval(autoload,loader);
	add_next_index_zval(autoload,method_name);


	MAKE_STD_ZVAL(function);
	ZVAL_STRING(function,"spl_autoload_register",0);

	zend_fcall_info fci = {
		sizeof(fci),
		EG(function_table),
		function,
		NULL,
		&ret,
		1,
		(zval ***)params,
		NULL,
		1
	};

	if(zend_call_function(&fci, NULL TSRMLS_CC) == FAILURE) {
		if(ret){
			zval_ptr_dtor(&ret);
		}
		zval_ptr_dtor(&autoload);
		efree(function);
		return 0;
	}
	if(ret){
		zval_ptr_dtor(&ret);
	}
	efree(function);
	zval_ptr_dtor(&autoload);

	return 1;
}
/*}}}*/

/*{{{ int fool_loader_include(char* path TSRMLS_DC)
 */
int fool_loader_include(char* path TSRMLS_DC)
{
	zend_file_handle file_handle;

	if (php_stream_open_for_zend_ex(path, &file_handle, ENFORCE_SAFE_MODE|IGNORE_URL_WIN|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == FAILURE){
		php_printf("[Warn]include file '%s' not exist!!",path);
		return 0;
	}

	zend_op_array 	*new_op_array;
	uint 			dummy = 1;
	//
	//...

	if (zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path) + 1, (void *)&dummy, sizeof(int), NULL) == SUCCESS) {
		new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE TSRMLS_CC);
		zend_destroy_file_handle(&file_handle TSRMLS_CC);
	} else {
		new_op_array = NULL;
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
		zend_file_handle_dtor(&file_handle TSRMLS_CC);
#else
		zend_file_handle_dtor(&file_handle);
#endif
	}

	if(!new_op_array){
		return 0;
	}

	zval *result;

	//保存原来的执行环境，包括active_op_array,opline_ptr等
	zval ** __old_return_value_pp   = EG(return_value_ptr_ptr);
	zend_op ** __old_opline_ptr     = EG(opline_ptr); 
	zend_op_array * __old_op_array  = EG(active_op_array);
	//

	EG(return_value_ptr_ptr) = &result;
	EG(active_op_array) 	 = new_op_array;

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}
#endif
	zend_execute(new_op_array TSRMLS_CC);
	destroy_op_array(new_op_array TSRMLS_CC);
	efree(new_op_array);

	//通过检查执行期间的全局变量(EG)的exception是否被标记来确定是否有异常
	if (!EG(exception)) {
		if (EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
			zval_ptr_dtor(EG(return_value_ptr_ptr));
		}
	}

	EG(return_value_ptr_ptr) = __old_return_value_pp;
	EG(opline_ptr)           = __old_opline_ptr; 
	EG(active_op_array)      = __old_op_array; 

	return 1;
}
/*}}}*/

/*{{{ Fool_Loader::__construct(void)
 */
PHP_METHOD(fool_loader,__construct)
{
}
/*}}}*/

/*{{{ Fool_Loader::registerAutoload(void)
 */
PHP_METHOD(fool_loader,registerAutoload)
{
	int ret = fool_loader_register_autoload(getThis() TSRMLS_CC);

	if(ret == 0){
		RETURN_FALSE;
	}else{
		RETURN_TRUE;
	}
}
/*}}}*/

/*{{{ Fool_Loader::getInstance(void)
 */
PHP_METHOD(fool_loader,getInstance)
{
	zval* instance = fool_loader_instance(TSRMLS_C);
	
	if(instance){
		RETURN_ZVAL(instance,1,0);
	}
	RETURN_FALSE;
}
/*}}}*/

/*{{{ int fool_loader_autoload(char* class_name TSRMLS_DC)
 */
int fool_loader_autoload(char* class_name TSRMLS_DC)
{
	zval** class_map;
	zval** class_file;

	if(!(&EG(symbol_table))){
		return 0;
	}

	if(zend_hash_find(&EG(symbol_table),FOOL_CLASS_MAP_VAR,strlen(FOOL_CLASS_MAP_VAR) + 1,(void**)&class_map) == FAILURE){
		php_printf("[Warn]Can't find class map!!%s\n",FOOLPHP_G(class_map));
		return 0;
	}

	if(zend_hash_find(Z_ARRVAL_PP(class_map),class_name,strlen(class_name) + 1,(void**)&class_file) == FAILURE){
		php_printf("[Warn]Can't find class file : %s!!\n",class_name);
		return 0;
	}

	fool_loader_include(Z_STRVAL_PP(class_file) TSRMLS_CC);

	return 1;
}
/*}}}*/

/*{{{ Fool_Loader::autoload(string $class_name)
 */
PHP_METHOD(fool_loader,autoload)
{
	char* class_name;
	int class_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
		return;
	}

	//check EG(class_table)

	fool_loader_autoload(class_name TSRMLS_CC);
}
/*}}}*/

zend_function_entry fool_loader_methods[] = {
	ZEND_ME(fool_loader,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(fool_loader,autoload,NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(fool_loader,getInstance,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(fool_loader,registerAutoload,NULL,ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP_MODULE(loader)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_Loader",fool_loader_methods);

	fool_loader_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_loader_ce,ZEND_STRL(FOOL_LOADER_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);

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
