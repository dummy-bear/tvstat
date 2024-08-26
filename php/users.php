<?
session_start();

require "const.php";
require "head.php";

drawhead(6);

$tn=array ("ID","login","Полное имя","Электронная почта","Телефон","Комментарий","");

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_select_db($base1);

echo "<FORM METHOD=POST ACTION=\"edituser.php\">";
if ($_SESSION["grp"]==1) 
{
	echo "<H2>Список пользователей:</H2>";
	$sql="SELECT ID,name,fullname,email,phone,comment FROM users ORDER BY ID;";
	$q=mysql_query ($sql,$conn);
	echo "<TABLE BORDER=2><TR><TH>номер</TH><TH>логин</TH><TH>имя пользователя</TH><TH>электронная почта</TH><TH>телефон</TH><TH>комментарий</TH><TH>операции</TH></TR>";
	for ($i=0;$i<mysql_num_rows($q);$i++)
	{
		echo "<TR>";
		for ($j=0;$j<6;$j++) echo "<TD>".mysql_result($q,$i,$j)."</TD>";
		echo "<TD><INPUT TYPE=\"submit\" name=\"edit".$i."\" value=\"изменить данные\">  <INPUT TYPE=\"submit\" name=\"pass".$i."\" value=\"изменить пароль\">  <INPUT TYPE=\"submit\" name=\"del".$i."\" value=\"удалить\"></TD></TR>";
	}
	echo "</TABLE><P>";

echo "<INPUT TYPE=\"submit\" name=\"add\" value=\"Добавить пользователя\">";
}
echo "</FORM></TABLE></BODY>";

?>
