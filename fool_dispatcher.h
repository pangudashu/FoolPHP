#ifndef PHP_FOOL_DISPATCHER_H
#define PHP_FOOL_DISPATCHER_H


#define FOOL_DISPATCHER_PROPERTY_NAME_INSTANCE "_instance"
#define FOOL_DISPATCHER_PROPERTY_NAME_REQUEST "_request"


extern zend_class_entry *fool_dispatcher_ce;
zval* fool_dispatcher_instance(TSRMLS_D);
zval* fool_dispatcher_dispatch(zval* dispatcher TSRMLS_DC);
zend_class_entry* fool_dispatcher_get_controller(char* controller_name TSRMLS_DC);

FOOL_STARTUP_MODULE(dispatcher);
#endif
