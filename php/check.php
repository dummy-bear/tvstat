<?
session_start();

$conn=mysql_connect ("localhost","root","gjhvtygfh")
	or die ("Невозможно установить соединение: ".mysql_error());

$database="tvstat";
$table_name="users";
mysql_query("SET NAMES 'utf8'");

mysql_select_db($database);

$uname=$_REQUEST["loginUsername"];
$upass=$_REQUEST["loginPassword"];

$sql="SELECT * FROM ".$table_name." WHERE name='".$uname."';";
sleep (1);
$q=mysql_query ($sql,$conn) or die();
$n=mysql_num_rows($q);
if ($n==1) 
{
	
	$tmp1=md5(md5($_REQUEST["loginPassword"]).mysql_result($q,0,"rs"));
//	echo "<BR>".$tmp1."<BR>";
	$p1=md5($tmp1.mysql_result($q,0,"rs"));
//	echo "<BR>".$p1."<BR>";

echo "<HTML><TITLE> Данные для ситуационного центра </TITLE>";
echo "<META http-equiv=\"Content-Type\" content=\"text/html; charset=utf8\">\n";

	if ($p1==mysql_result($q,0,"salt"))
	{	
		//$ck=mysql_result($q,0,'ID')+13;
		$_SESSION["login"]=$_REQUEST["loginUsername"];
		$_SESSION["uname"]=mysql_result($q,0,"fullname");
		$_SESSION["pass"]=$tmp1;
		$_SESSION["uid"]=mysql_result($q,0,"ID");
		$_SESSION["grp"]=mysql_result($q,0,"grp")+1;
		//echo "<META http-equiv=\"Set-Cookie\" content=\"NAME=".$ck."\">\n";
		echo "<META http-equiv=\"Refresh\" content=\"0;url=index.php\">\n";
	}
	else echo "Авторизация не прошла";

//	echo $ck;
//	echo "Авторизация прошла успешно";
}

else if ($n==0) echo "Авторизация не прошла";
else echo "Ошибка в таблице пользователей: видимо, пользователь с таким именем не один";


mysql_close($conn);
?>
</BODY>
</HTML>

