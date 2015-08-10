#ifndef PHP_FOOL_REQUEST_H
#define PHP_FOOL_REQUEST_H


#define FOOL_REQUEST_PROPERTY_NAME_INSTANCE            "_instance"
#define FOOL_REQUEST_PROPERTY_NAME_CONTROLLER          "controller"  //handler控制器
#define FOOL_REQUEST_PROPERTY_NAME_ACTION              "action"      //handler操作
#define FOOL_REQUEST_PROPERTY_NAME_CONTROLLER_NAME     "controller_name"  //handler控制器
#define FOOL_REQUEST_PROPERTY_NAME_ACTION_NAME         "action_name"      //handler操作

#define FOOL_REQUEST_CONTROLLER_SUFIX    "Controller"  //控制器后缀
#define FOOL_REQUEST_ACTION_SUFIX        "Action"      //操作后缀
#define FOOL_REQUEST_CONTROLLER_DEFAULT  "Index"       //默认控制器
#define FOOL_REQUEST_ACTION_DEFAULT      "index"       //默认操作
#define FOOL_REQUEST_SEPARATOR           "."           //分隔符
#define FOOL_REQUEST_METHOD_PARAM        "m"





extern zend_class_entry *fool_request_ce;
zval* fool_request_instance(TSRMLS_D);
zval* fool_request_get_controller(zval* request TSRMLS_DC);
zval* fool_request_get_controller_name(zval* request TSRMLS_DC);
zval* fool_request_get_action(zval* request TSRMLS_DC);
zval* fool_request_get_action_name(zval* request TSRMLS_DC);

FOOL_STARTUP_MODULE(request);
#endif
