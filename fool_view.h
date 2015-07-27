#ifndef PHP_FOOL_VIEW_H
#define PHP_FOOL_VIEW_H


#define FOOL_VIEW_PROPERTY_NAME_INSTANCE            "_instance"
#define FOOL_VIEW_PROPERTY_NAME_VAR                 "_var"

#define FOOL_VIEW_TPL_PATH                         "views"
#define FOOL_VIEW_TPL_EXTENSION                    ".tpl.php"



extern zend_class_entry *fool_view_ce;
zval* fool_view_instance(TSRMLS_D);
int fool_view_assign(zval* view,char* key,int key_len,zval* value TSRMLS_DC);
int fool_view_render(zval* view,char* script,zval* ret,int out TSRMLS_DC);

FOOL_STARTUP(view);
#endif
