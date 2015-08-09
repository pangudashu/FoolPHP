# FoolPHP
PHP扩展实现的轻量级MVC框架

### 说明
在php框架漫天飞的今天为什么还要自己再写一个呢？

首先，笔者写这个项目的主要目的很单纯：学习！笔者本人还很年轻，入行两年一直以二手程序员自居，还有很多很多需要学习的地方，我始终坚信阅读前辈的代码是一个非常不错的学习方式。FoolPHP是我在学习PHP扩展开发的过程中作为练习作业同时阅读参考Yaf而实现的，所以有些代码就是Yaf的。[@Laruence](http://www.laruence.com/) [@Walu](http://www.walu.cc/)

其次，虽然已经有很多优秀的框架供开发者选择，但是我仍然认为一个团队实现自己项目的框架是非常有意义的。笔者迄今为止待过的三个团队无一例外的都是在使用自己开发的框架，这能够让开发更好的掌控项目。所以，FoolPHP最本质的目的是想给那些没有实现过框架的团队、开发者们提供一个理解框架、学习php扩展的参考，从而实现属于他们自己的东西。

最后，这个框架的设计很简单，仅实现最核心的dispatcher。笔者最早接触的php框架是ThinkPHP，实在难以接受它"无所不能"的做事方式，后来在360用到了QFrame，被其简洁灵活的设计所吸引，以后陆续用过几个其它的框架，但是还是最欣赏QFrame，再后来看过了Yaf的代码，惊喜的发现本质上QFrame就是Yaf的php实现版，相比于Yaf，我更喜欢QFrame不限制目录、类名的设计方式，让项目完全用php最朴素的写法实现,所以FoolPHP也十分自豪的抄袭了这点。

### 特点

* 非常简洁
* 不限制任何类名、文件名、目录，通过预处理的方式将类名及其所在的文件加入autoload
* 类自动加载
* 原生php模板引擎

### 安装
	从github下载源码后解压，如解压后目录:FoolPHP-1.0.0<br />
	cd FoolPHP-1.0.0
	phpize
	./configure
	make && make install

	最后将extension=foolphp.so加入php.ini

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
    └── index.php<br />

3./websites/src/www为web访问目录，配置web服务器root ：/websites/src/www，访问：http://Domain/index.php?m=demo.index

### tools工具包

1. install.sh新项目初始化脚本 <br />
2. config.php预处理类配置文件，$application_path值为application目录：/your_websites/src/application，$config_path为配置文件目录：/your_websites/src/config，$scan_dir为自动加载类扫描目录，相对于$application_path的目录名，如controllers、models、librarys <br />
3.create_map.php预处理类生成脚本，它会扫描config.php中配置的$scan_dir目录下的php文件，生成"类名 => 文件路径"的记录到$config_path/fool_php_class.map文件中，这将是使用最频繁的文件，当新增/修改/删除文件、类、目录时都要重新执行此脚本，否则将会运行出错，使用此脚本执行 php create_map.php即可


