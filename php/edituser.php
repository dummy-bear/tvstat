<?
session_start();

require "const.php";
require "head.php";

drawhead(5);

$tn=array ("ID","login","Полное имя","Электронная почта","Телефон","Комментарий","");

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_select_db($base1);

if ($_REQUEST["add"]=="Добавить пользователя") 
{
	echo "Добавляем пользователя\n";
	echo "<FORM METHOD=POST ACTION=\"users.php\">\n";
	echo "Login <INPUT TYPE=text name=\"inpu0\"><P>";
 	echo "Пароль <INPUT TYPE=password name=\"inpu1\"><P>";
	for ($i=2;$i<6;$i++) echo $tn[$i]." <INPUT TYPE=text name=\"inpu".$i."\"><P>";
	echo "<INPUT TYPE=\"submit\" name=\"add\" value=\"Добавить пользователя\"></FORM>";
}
else
{
	$sql="SELECT ID,name,fullname,email,phone,comment FROM users ORDER BY ID;";
	$q=mysql_query ($sql,$conn);
	for ($i=0;$i<mysql_num_rows($q);$i++)
	{
		if ($_REQUEST["edit".$i]=="изменить данные")
		{
			echo "aa";
			echo "<FORM METHOD=POST ACTION=\"users.php\">\n";
			echo "Login <INPUT TYPE=text name=\"inpu0\"><P>";
 			//echo "Пароль <INPUT TYPE=password name=\"inpu1\"><P>";
			for ($i=2;$i<6;$i++) echo $tn[$i]." <INPUT TYPE=text name=\"inpu".$i."\"><P>";
			echo "Введите свой пароль для подтверждения прав на внесение указанных изменений. <P><INPUT TYPE=\"password\" name=\"pass\">";
			echo "<INPUT TYPE=\"submit\" name=\"add\" value=\"Применить\"><INPUT TYPE=\"submit\" name=\"cancel\" value=\"Отменить\"></FORM>";

		}
		else if ($_REQUEST["pass".$i]=="изменить пароль")
		{
			echo "Пароль <INPUT TYPE=password name=\"inpu1\"><P>";
			echo "Введите свой пароль для подтверждения прав на внесение указанных изменений. <P><INPUT TYPE=\"password\" name=\"pass\">";
			echo "<INPUT TYPE=\"submit\" name=\"add\" value=\"Применить\"><INPUT TYPE=\"submit\" name=\"cancel\" value=\"Отменить\"></FORM>";
		}
		else if ($_REQUEST["del".$i]=="удалить") ;
	}
}

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
		echo "<TD><INPUT TYPE=\"submit\" name=\"edit\"".$i." value=\"изменить данные\">  <INPUT TYPE=\"submit\" name=\"pass\"".$i." value=\"изменить пароль\">  <INPUT TYPE=\"submit\" name=\"del\"".$i." value=\"удалить\"></TD></TR>";
	}
	echo "</TABLE><P>";

echo "<INPUT TYPE=\"submit\" name=\"add\" value=\"Добавить пользователя\">";
}
echo "</FORM></TABLE></BODY>";

?>