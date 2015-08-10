#ifndef PHP_FOOL_EXCEPTION_H
#define PHP_FOOL_EXCEPTION_H


#define FOOL_EXCEPTION_PROPERTY_NAME_INSTANCE     "_instance"


extern zend_class_entry *fool_exception_ce;
void fool_throw_exception(long code,char* msg TSRMLS_DC);

FOOL_STARTUP_MODULE(exception);
#endif
