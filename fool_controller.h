#ifndef PHP_FOOL_CONTROLLER_H
#define PHP_FOOL_CONTROLLER_H


#define FOOL_CONTROLLER_PROPERTY_NAME_INSTANCE            "_instance"
#define FOOL_CONTROLLER_PROPERTY_NAME_REQUEST             "_request"
#define FOOL_CONTROLLER_PROPERTY_NAME_VIEW                "_view"


extern zend_class_entry *fool_controller_ce;

int fool_controller_dispatch_handler(zend_class_entry* ce,zval* controller,zval* request,zval* view TSRMLS_DC);

FOOL_STARTUP_MODULE(controller);
#endif
