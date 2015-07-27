#ifndef PHP_FOOL_OBJECT_H
#define PHP_FOOL_OBJECT_H


#define FOOL_OBJECT_PROPERTY_NAME_INSTANCE            "_instance"
#define FOOL_OBJECT_PROPERTY_NAME_LIST                "_obj_list"



extern zend_class_entry *fool_object_ce;
zval* fool_object_instance(TSRMLS_D);

FOOL_STARTUP(object);
#endif
