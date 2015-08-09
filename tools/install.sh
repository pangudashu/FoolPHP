#!/bin/bash

root=`pwd`
application_path=$root"/src/application"
config_path=$root"/src/config"
web_path=$root"/src/www"
tool_root=$root"/tools"


black='\E[0m\c'
red='\E[31m\c'
c_notify='\E[1;36m\c'

PHP=`command -v php`
if [ -z "${PHP}" ];then
	echo "php not found"
	exit 1
fi


cprint()
{
	color=${2:-$red}
	echo -e "$color"
	echo "$1"
	echo -e "$black"
	return
}

build_index()
{
cprint "[4/6]创建入口文件:index.php" $c_notify
cat>"${root}/src/www/index.php"<<INDEX
<?php
define('APPLICATION_PATH','${root}/src/application');
define('CONFIG_PATH','${root}/src/config');

\$application = Fool_Application::getInstance(APPLICATION_PATH,CONFIG_PATH);
\$application->run();
INDEX
	chmod 755 "${root}/src/www/index.php"
	echo "build: ${root}/src/www/index.php -----finish"
}

build_config()
{
cat>"${config_path}/config.inc.php"<<CONF
<?php
\$title = "Welcome to FoolPHP~";
CONF
	chmod 755 "${config_path}/config.inc.php"
	echo "build: ${config_path}/config.inc.php -----finish"
}

build_controller()
{
cat>"${application_path}/controllers/DemoController.php"<<EOF
<?php
class DemoController extends Fool_Controller
{
	public function indexAction()
	{
		\$demo = new DemoModel();
		\$now = \$demo->GetTime();

		\$this->assign('time',\$now);
		\$this->assign('web_title',Fool_Config::find("title"));
		\$this->display();
	}
}
EOF
	echo "build: ${application_path}/controllers/DemoController.php -----finish"
}

build_model()
{
cat>"${application_path}/models/DemoModel.php"<<EOF
<?php
class DemoModel
{
	public function GetTime()
	{
		return time();
	}
}
EOF
	echo "build: ${application_path}/models/DemoModel.php -----finish"
}

build_app_config()
{
cat>"${tool_path}/config.php"<<EOF
<?php
\$application_path = '${application_path}';
//自动加载类扫描目录:{\$application_root}/application下目录名称
\$scan_dir = array(
    'controllers',
	'models',
);
EOF
	echo "build: ${tool_path}/config.php -----finish"
}

build_view()
{
cat>"${application_path}/views/Common/header.tpl.php"<<HEADER
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title><?php echo \$web_title;?></title>
<style>
	body{font-family:"微软雅黑";}
	h3{font-size: 26px;color:#666666;}
	p{font-size: 14px;color:#77818e;}
</style>
<body>
HEADER

cat>"${application_path}/views/Common/footer.tpl.php"<<FOOTER
</body>
</html>
FOOTER

cat>"${application_path}/views/Demo/index.tpl.php"<<EOF
<?php include \$VIEW_PATH . '/Common/header.tpl.php';?>
	<h3>Welcome to FoolPHP~</h3>
    <p>This is a MVC framework implemented by PHP extension.</p>
	<p>Now time is <?php echo date("Y-m-d H:i:s",\$time);?></p>
<?php include \$VIEW_PATH . '/Common/footer.tpl.php';?>
EOF
	echo "build: ${application_path}/views/Common/header.tpl.php -----finish"
	echo "build: ${application_path}/views/Common/footer.tpl.php -----finish"
	echo "build: ${application_path}/views/Demo/index.tpl.php -----finish"
}

#=================================[start]==================================

cprint "[1/6]检查扩展是否已安装..." $c_notify
#check if foolphp had been installed
ext_foolphp_installed="$PHP -m|grep foolphp"
if [ -z "$ext_foolphp_installed" ];then
	cprint "Notice:foolphp扩展未安装!!!"
	exit 1
fi

if [ "$0" != "tools/install.sh" ];then
	cprint "Notice:请在tools上级目录执行命令:sh tools/install.sh"
	exit 1
fi

cprint "[2/6]检查当前目录..." $c_notify
if [ -d "${root}/src" ];then
	cprint "Notice: 项目目录:${root}/src不是一个空目录!!!如果你想初始化一个FoolPHP项目请确保目录下除tools外无其他目录及文件"
	exit 1
fi

cprint "[3/6]开始创建项目目录..." $c_notify
#build application directory
mkdir -p "${config_path}"
echo "mkdir: ${config_path} -----finish"
mkdir -p "${web_path}"
echo "mkdir: ${web_path} -----finish"
mkdir -p "${application_path}"
echo "mkdir: ${application_path} -----finish"
mkdir -p "${application_path}/controllers"
echo "mkdir: ${application_path}/controllers -----finish"
mkdir -p "${application_path}/models"
echo "mkdir: ${application_path}/models -----finish"
mkdir -p "${application_path}/views/Demo"
echo "mkdir: ${application_path}/views/Demo -----finish"
mkdir -p "${application_path}/views/Common"
echo "mkdir: ${application_path}/views/Common -----finish"

chmod -R 755 "${root}/src"
cprint "[3/6]目录创建成功!" $c_notify


#build入口文件index.php
build_index


cprint "[5/6]创建项目示例文件..." $c_notify
#build config.inc.php
build_config
#build demo controller
build_controller
#build demo model
build_model
#build demo view
build_view

build_app_config

cprint "[6/6]创建类库生成脚本..." $c_notify
var=${tool_root//\//\\\/}
sed -i "s/__FOOLPHP_TOOLS__ROOT__/${var}/" "tools/create_map.php"
echo "build: ${tool_root}/create_map.php -----finish"
#build class map
$PHP "${tool_root}/create_map.php" >/dev/null 2>&1

echo ""
cprint "=========================[success to install foolphp application]==========================" $c_notify
cprint "+ 配置web服务器 root : ${web_path}" $c_notify
cprint "+ 访问:http://Domain/index.php?m=demo.index" $c_notify


