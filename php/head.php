<?


function drawhead ($n)
{
	require "const.php";

$page=array("Обязательные каналы","Все каналы","Статистика","Задания","Профилактики","Настройки tvstat","Пользователи tvstat");
$pname=array ("index","all","stat","task","profil","settings","users");
if ($_SESSION["grp"]) $pag=7;
else $pag=5;

	echo "<HTML>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
	if ($n==0) echo "<META http-equiv=\"Refresh\" content=\"300;url=index.php\">";
	echo "<TITLE>".$title."</TITLE>\n";
	echo "<BODY>\n\n<IMG SRC=\"logo.png\">\n<FONT COLOR=96ceff SIZE=15><B><I>".$title;
	echo "</I></B></FONT>\n<HR>";
	
	echo "<TABLE><TR><TD VALIGN=TOP><TABLE BORDER=3>";
	if (! $_SESSION["login"]) echo "<TR><TD><FORM METHOD=POST ACTION=\"check.php\">\n Имя: <BR><input type=\"text\" size=10 name=\"loginUsername\"><BR>Пароль:<BR><input type=\"password\" size=10 name=\"loginPassword\"><BR><input type=\"submit\" value=\"Вход\"></FORM></TD></TR>";
	else echo "<TR><TD>Вход выполнен.<BR> Вы - ".$_SESSION["uname"].".<BR><FORM METHOD=POST ACTION=\"logout.php\"><input type=\"submit\" value=\"Выход\"></FORM></TD></TR>";
	for ($i=0;$i<$pag;$i++)
	{
		if ($i==$n) echo "<TR><TD BGCOLOR=blue><FONT COLOR=RED>".$page[$n]." </FONT><BR>\n </TD></TR>\n";
		else echo "<TR><TD BGCOLOR='96ceff'> <A HREF=\"".$pname[$i].".php\">".$page[$i]."</A><BR>\n </TD></TR>";
	}
	echo "</TABLE><TD>";

}
?>

