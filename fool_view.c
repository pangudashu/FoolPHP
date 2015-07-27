#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/php_output.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "php_foolphp.h"

#include "fool_view.h"
#include "fool_loader.h"

zend_class_entry* fool_view_ce;


ZEND_BEGIN_ARG_INFO_EX(fool_view_construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()



/**{{{ zval* get_instance(TSRMLS_D)
 */
zval* fool_view_instance(TSRMLS_D)
{
	zval* instance;
	zval* var;

	instance = zend_read_static_property(fool_view_ce,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_INSTANCE),1 TSRMLS_CC);

	if(Z_TYPE_P(instance) == IS_OBJECT){
		//Z_ADDREF_P(instance);  //?????????????????
		return instance;
	}

	object_init_ex(instance,fool_view_ce);

	zend_update_static_property(fool_view_ce,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_INSTANCE),instance TSRMLS_CC);

	
	if(instance){
		MAKE_STD_ZVAL(var);
		array_init(var);
		zend_update_property(fool_view_ce,instance,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_VAR),var TSRMLS_CC);
		return instance;
	}
	return NULL;
}
/*}}}*/

/*{{{ int fool_view_assign(zval* view,char* key,int key_len,zval* value TSRMLS_DC)
 */
int fool_view_assign(zval* view,char* key,int key_len,zval* value TSRMLS_DC)
{
	zval* var = zend_read_property(fool_view_ce,view,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_VAR),1 TSRMLS_CC);

	Z_ADDREF_P(value);	
	zend_hash_update(Z_ARRVAL_P(var),key,key_len+1,(void**)&value,sizeof(zval*), NULL);
	return 1;
}
/*}}}*/

/*{{{ int fool_view_extract(zval* tpl_vars TSRMLS_DC)
 */
int fool_view_extract(zval* tpl_vars TSRMLS_DC)
{
	zval **entry;
	char *var_name;
	long num_key;
	uint var_name_len;
	HashPosition pos;


#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
	if (!EG(active_symbol_table)) {
		/*zend_rebuild_symbol_table(TSRMLS_C);*/
		return 1;
	}
#endif

	if(!tpl_vars || Z_TYPE_P(tpl_vars) != IS_ARRAY){
		return 1;
	}

	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(tpl_vars), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(tpl_vars), (void **)&entry, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(tpl_vars), &pos)) {
		
		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(tpl_vars), &var_name, &var_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
			continue;
		}

		if (var_name_len == sizeof("GLOBALS") && !strcmp(var_name, "GLOBALS")) {
			continue;
		}

		if (var_name_len == sizeof("this")  && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
			continue;
		}

		ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), var_name, var_name_len,
			*entry, Z_REFCOUNT_P(*entry) + 1, PZVAL_IS_REF(*entry));
	}

	return 1;
}
/*}}}*/

/*{{{ int fool_view_render(zval* view,char* script,zval* ret,int out TSRMLS_DC)
 */
int fool_view_render(zval* view,char* script,zval* ret,int out TSRMLS_DC)
{
	char* view_path = NULL;
	char* tpl = NULL;
	zval* z_view_path;

	spprintf(&view_path,0,"%s/%s/",FOOLPHP_G(application_path),FOOL_VIEW_TPL_PATH);
	spprintf(&tpl,0,"%s%s%s",view_path,script,FOOL_VIEW_TPL_EXTENSION);

	MAKE_STD_ZVAL(z_view_path);
	ZVAL_STRING(z_view_path,view_path,0);
	fool_view_assign(view,"VIEW_PATH",strlen("VIEW_PATH"),z_view_path TSRMLS_CC);
	
	zval* var = zend_read_property(fool_view_ce,view,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_VAR),1 TSRMLS_CC);

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

	//将模板变量提取到当前符号表-引用
	fool_view_extract(var TSRMLS_CC);

	//open out buffer
	//
	if (php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == FAILURE) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "failed to create buffer");
		return 0;
	}

	int res = fool_loader_include(tpl TSRMLS_CC);

	efree(tpl);
	efree(view_path);

	zval_ptr_dtor(&z_view_path);

	if (calling_symbol_table) {
		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = calling_symbol_table;
	}

	if(out){
		php_output_flush(TSRMLS_C);
	}else{
		if (php_output_get_contents(ret TSRMLS_CC) == FAILURE) {
			php_output_end(TSRMLS_C);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fetch ob content");
			return 0;
		}
	}
	

	if (php_output_discard(TSRMLS_C) != SUCCESS ) {
		return 0;
	}

	return 1;
}
/*}}}*/

/*{{{ public Fool_Requst::__construct(void)
 */
PHP_METHOD(fool_view,__construct)
{
}
/*}}}*/


zend_function_entry fool_view_methods[] = {
	ZEND_ME(fool_view,__construct,fool_view_construct_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_FE_END	/* Must be the last line in qpclass_functions[] */
};


FOOL_STARTUP(view)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Fool_View",fool_view_methods);

	fool_view_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(fool_view_ce,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_INSTANCE),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(fool_view_ce,ZEND_STRL(FOOL_VIEW_PROPERTY_NAME_VAR),ZEND_ACC_PUBLIC TSRMLS_CC);

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
