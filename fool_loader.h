#ifndef PHP_FOOL_LOADER_H
#define PHP_FOOL_LOADER_H


#define FOOL_LOADER_PROPERTY_NAME_INSTANCE "_instance"


extern zend_class_entry *fool_loader_ce;
zval* fool_loader_instance(TSRMLS_D);
int fool_loader_register_autoload(zval* loader TSRMLS_DC);

FOOL_STARTUP_MODULE(loader);
#endif
