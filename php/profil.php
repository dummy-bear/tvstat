<?
session_start();

require "const.php";
require "head.php";

drawhead(4);

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_query("SET NAMES 'utf8'");
mysql_select_db($base1);

$sql="SELECT tvlist.name,tvlist.tvk,tvlist.freq,perday.start,perday.stop,perday.comment,users.fullname FROM perday,tvlist,users WHERE perday.kanal=tvlist.ID AND perday.who=users.ID;";
$q=mysql_query($sql,$conn); 

if ($_SESSION["grp"]) echo "<FORM ACTION=\"addprof.php\">\n";

echo "<H1>Расписание профилактических отключений</H1>\n<H3>Ежедневные отключения</H3>";
if ($_SESSION["grp"]) echo "<INPUT TYPE=\"button\" name=\"b1\" value=\"Добавить сведения\"><P>";
echo "<TABLE BORDER=1><TR><TH>ТВК</TH><TH>Частота, МГц</TH><TH>Название программы</TH><TH>Начало отключения</TH><TH>Конец отключения</TH><TH>Комментарий</TH><TH>Кто добавил</TH></TR>";
for ($i=0;$i<mysql_num_rows($q);$i++)
{
	echo "<TR>";
	for ($j=0;$j<7;$j++) echo "<TD>".mysql_result($q,$i,$j)."</TD>";
	echo "</TR>";
}

echo "</TABLE> <P>\n <H3> Разовые отключения </H3> \n ";

if ($_SESSION["grp"]) echo "<INPUT TYPE=\"button\" name=\"b2\" value=\"Добавить сведения\"><P>";

$sql="SELECT tvlist.name,tvlist.tvk,tvlist.freq,profilaktika.start,profilaktika.stop,profilaktika.comment,users.fullname FROM profilaktika,tvlist,users WHERE profilaktika.kanal=tvlist.ID AND profilaktika.who=users.ID ORDER BY profilaktika.start DESC;";
echo $sql;
$q=mysql_query($sql,$conn);

echo "<TABLE BORDER=1><TR><TH>ТВК</TH><TH>Частота, МГц</TH><TH>Название программы</TH><TH>Начало отключения</TH><TH>Конец отключения</TH><TH>Комментарий</TH><TH>Кто добавил</TH></TR>";
for ($i=0;$i<mysql_num_rows($q);$i++)
{
        echo "<TR>";
        for ($j=0;$j<7;$j++) echo "<TD>".mysql_result($q,$i,$j)."</TD>";
	echo "</TR>";
}

echo "</TABLE> <P>\n";
if ($_SESSION["grp"]) echo "</FORM>";


?>
</HTML>
