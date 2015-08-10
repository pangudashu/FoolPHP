#ifndef PHP_FOOL_CONFIG_H
#define PHP_FOOL_CONFIG_H


#define FOOL_CONFIG_PROPERTY_NAME_INSTANCE            "_instance"
#define FOOL_CONFIG_PROPERTY_NAME_VAR	              "_config"



extern zend_class_entry *fool_config_ce;

int fool_config_get_defined_vars(char* config_path,zval* var_list TSRMLS_DC);
int fool_config_init(TSRMLS_D);
zval* fool_config_get(zval* object,char* class_name,int name_len TSRMLS_DC);

FOOL_STARTUP_MODULE(config);
#endif
