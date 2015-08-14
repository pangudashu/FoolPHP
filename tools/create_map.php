<?php
include '__FOOLPHP_TOOLS__ROOT__/config.php';

$obj = new Autoload();

for($i = 0;$i < 10;$i++){
	$r = $obj->run($application_path,$config_path,$scan_dir);
	if(true === $r) break;
}

class Autoload
{
	private $filter = array('views');
	private $map_value_name = '$fool_php_class_map';

	public function run($app_root,$config_root,$scan_dir)
	{/*{{{*/
		$list = array();

		foreach($scan_dir as $_dir){
			$_list = $this->scanFile($app_root . '/' . $_dir);
			if(empty($_list)) continue;
			$list = array_merge($list,$_list);
		}
		$tpl = "\t'%s' => '%s',\n";

		$content_tpl = "<?php\n/*create by : " . $_SERVER['LOGNAME'] . " on : %s*/\n%s = array(\n%s);\n";

		$d = "";
		$n = 1;
		foreach($list as $class_name=>$path){
			$d .= sprintf($tpl,$class_name,$path);
			echo $n,".Find Class [ $class_name ] in \"$path\"\n";
			++$n;
		}

		$content = sprintf($content_tpl,date('Y/m/d H:i:s',time()),$this->map_value_name,$d);

		$key = md5($content);

		$map_path = $config_root . '/' . ini_get('foolphp.class_map');
		$fp = fopen($map_path,"w");
		if(!$fp){
			echo "无法写入文件:" . $map_path . ",请检查目录的读写权限!\n";
			exit;
			return false;
		}
		fwrite($fp,$content);
	
		fclose($fp);

		$_key = md5(file_get_contents($map_path));

		if($key == $_key){
			echo "\n====================[success]===================\nfinish to build class map!\n";
			return true;
		}else{
			return false;
		}
	}/*}}}*/

	public function scanFile($directory) 
	{/*{{{*/
		if(!is_dir($directory)) return array();
		$arr = array();
		$mydir = dir($directory); 
		while($file = $mydir->read())
		{ 
			if((is_dir("$directory/$file")) && $file!="."  && $file!=".." && !in_array($file,$this->filter)) 
			{
				$res = $this->scanFile("$directory/$file"); 
				$arr = array_merge($arr, $res);
			}else if(strpos($file,'.') !== 0){ 
				$file_path = $directory."/".$file;
				$file_info  = pathinfo($file_path);
				if(isset($file_info['extension']) && $file_info['extension'] == 'php') 
				{
					$classes = $this->getClassName($file_path);
					foreach($classes as $class)
					{
						$arr[$class] = $file_info['dirname'].'/'.$file_info['basename'];
					}
				}
			}
		} 
		$mydir->close(); 
		return $arr;
	}/*}}}*/

	public function getClassName($file)
	{/*{{{*/
		$lines = file($file);
		$class = array();
		foreach($lines as $line)
		{
			if(preg_match("/^\s*[cC]lass\s+(\S+)\s*/",$line,$match)) $class[] = trim($match[1],"{");
			if (preg_match("/^\s*abstract\s*class\s+(\S+)\s*/", $line, $match)) $class[] = trim($match[1],"{");
			if (preg_match("/^\s*interface\s+(\S+)\s*/", $line, $match)) $class[] = trim($match[1],"{");
		}
		return $class;
	}/*}}}*/

}
