# FoolPHP
PHP扩展实现的轻量级MVC框架

[php7版本>>](https://github.com/pangudashu/FoolPHP7)

### 说明

FoolPHP是在学习PHP扩展开发的过程中作为练习作业同时阅读参考Yaf而实现的，所以有些代码就是Yaf的。[@Laruence](http://www.laruence.com/) [@Walu](http://www.walu.cc/)

这个框架的比较简洁，仅实现最核心的dispatcher。

### 特点

* 简洁
* 不限制任何类名、文件名、目录，通过预处理的方式将类名及其所在的文件加入autoload
* 类自动加载
* 原生php模板引擎

### 安装
	从github下载源码后解压，如解压后目录:FoolPHP-1.0.0
	cd FoolPHP-1.0.0
	phpize
	./configure
	make && make install

	最后将extension=foolphp.so加入php.ini，重启php-fpm或者其他fastcgi

### 初始化项目

1.新建一个目录，如/websites，将FoolPHP源码下tools目录复制到/websites目录下

2.执行sh tools/install.sh

成功后/websites下多了一个src目录，/websites/src结构:

├── application<br />
│   ├── controllers<br />
│   │   └── DemoController.php<br />
│   ├── models<br />
│   │   └── DemoModel.php<br />
│   └── views<br />
│       ├── Common<br />
│       │   ├── footer.tpl.php<br />
│       │   └── header.tpl.php<br />
│       └── Demo<br />
│           └── index.tpl.php<br />
├── config<br />
│   ├── config.inc.php<br />
│   └── fool_php_class.map<br />
└── www<br />
|    └── index.php<br />

3./websites/src/www为web访问目录，配置web服务器root ：/websites/src/www，访问：http://Domain/index.php?m=demo.index

### tools工具包
1. install.sh新项目初始化脚本

2. config.php预处理类配置文件，$application_path值为application目录：/your_websites/src/application，$config_path为配置文件目录：/your_websites/src/config，$scan_dir为自动加载类扫描目录，相对于$application_path的目录名，如controllers、models、librarys

3. create_map.php预处理类生成脚本，它会扫描config.php中配置的$scan_dir目录下的php文件，生成"类名 => 文件路径"的记录到$config_path/fool_php_class.map文件中，这将是使用最频繁的文件，当新增/修改/删除文件、类、目录时都要重新执行此脚本，否则将会运行出错

	使用此脚本执行 php create_map.php即可

# 使用手册

### 入口文件
	//index.php,使用install.sh初始化的项目此文件位于/website/src/www/index.php
	<?php
	define('APPLICATION_PATH','/search/web/foolphp/src/application');
	define('CONFIG_PATH','/search/web/foolphp/src/config');

	$application = Fool_Application::getInstance(APPLICATION_PATH,CONFIG_PATH);
	$application->run();

APPLICATION_PATH : 应用目录
CONFIG_PATH : 配置文件目录

### 路由
框架提供最简单的通过GET参数分发路由(/?m=controller.action)，没有实现rewrite、正则等路由协议

### 控制器
控制器名以Controller为后缀，文件名没有要求，可以是任意名称，只要继承了Fool_Controller的类就是一个控制器，控制器中操作(Action)作为请求处理的handler，通过/?m=controller.action访问将dispatch到此Action，操作的命名以Action为后缀

	<?php
	class DemoController extends Fool_Controller{
		public function indexAction(){
			$this->assign("content",$this->getContent());
			$this->display();
		}

		private function getContent(){
			return "Hi~";
		}
	}
	?>

在控制中有两个前后置函数:preDispatch()、afterDispatch()分别在Action执行前后调用

控制器方法

	assign(string $key,void $value)  模板赋值
	render([string $tpl_script])     渲染模板，将结果返回，不传参数默认调用views路径下"控制器/操作.tpl.php"
	display([string $tpl_script])    渲染模板,结果直接输出
	getController(void)              获取当前控制器名
	getMethod(void)                  获取当前操作名

### 配置
CONFIG_PATH下有两个配置文件:项目配置(config.inc.php)、自动加载类配置(fool_php_class.map)，其中fool_php_class.map由tools/create_map.php脚本自动更新

config.inc.php配置文件格式为普通的key=>value：

	<?php
	$title = "Welcome to FoolPHP~";
	$db = array(
		'host' => '127.0.0.1',
		'user' => 'root',
		'pass' => '',
		'port' => 3306,
	);

在项目中通过Fool_Config::find(string $key)获取配置

### 视图
框架使用php作为模板引擎，视图文件后缀.tpl.php，目录:/websites/src/application/views，这个值在模板中可以通过$VIEW_PATH变量获取(嵌套模板时将会用到)

views目录下可以按控制器划分子目录

### 自动加载
FoolPHP使用spl_autoload_register实现自动加载，当实例化一个类时，如果在EG(class_table)哈希表中找不到这个类将调用注册的钩子函数，根据CONFIG_PATH下的fool_php_class.map找到这个类所在的文件，然后include

所以当项目中新增、修改类时都需要执行"php tools/create_map.php"重新生成fool_php_class.map，这种方式使得项目的目录设置更灵活

在项目中可以通过普通的方式实例化一个类:$user = new UserModel();

同时框架提供了一个方法生成单例:Fool_Object::find(string $className)

### 异常
框架中一些错误通过异常的方式返回，可以在入口文件中捕捉到进行处理:

	try{
		$application = Fool_Application::getInstance(APPLICATION_PATH,CONFIG_PATH);
		$application->run();
	}catch(Fool_Exception $e){
		echo $e->getCode(),$e->getMessage();
	}



