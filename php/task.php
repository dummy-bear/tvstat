<?
session_start();

require "const.php";
require "head.php";

drawhead(3);

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_query("SET NAMES 'utf8'");
mysql_select_db($base1);

$sql="SELECT tasks.ID,start,stop,typ,freq,fullname,status,result FROM tasks,users where tasks.userid=users.ID ORDER BY tasks.start;";
$q=mysql_query($sql,$conn); 

//echo "Идёт запись контента по заданию №4366-04/16 от 11.10.2013.";
echo "<H1>Список заданий</H1>";
echo "<TABLE BORDER=1><TR><TH>Время старта</TH><TH>Время завершения</TH><TH>Тип задания</TH><TH>Частота, МГц</TH><TH>Автор</TH><TH>Статус задания</TH></TR>";
for ($i=0;$i<mysql_num_rows($q);$i++)
{
	echo "<TR>";
	for ($j=1;$j<6;$j++) echo "<TD>".mysql_result($q,$i,$j)."</TD>";
	if (mysql_result($q,$i,'status')=="w") echo "<TD>Ожидается выполнение.<BR><A HREF=\"edittask.php?task=".mysql_result($q,$i,'ID')."\">Редактировать</A>  <A HREF=\"deletetask.php?task=".mysql_result($q,$i,'ID')."\">Удалить</A></TD>";
	else if (mysql_result($q,$i,'status')=="r") 
	{
//		echo "<TD>Выполняется сейчас. <BR>";
		if (mysql_result($q,$i,'typ')=='b') echo "<TD BGCOLOR=YELLOW>Идёт вещание. Откройте в vlc адрес http://85.233.84.169:8080 </TD>";
		else echo "<TD BGCOLOR=YELLOW>Идёт запись</TD>";
	}
	else if (mysql_result($q,$i,'status')=="s") 
	{
		if (mysql_result($q,$i,'typ')=='b') echo "<TD>Вещание уже завершено. </TD>";
                else echo "<TD>Записан файл <A HREF=\"video/".mysql_result($q,$i,'result')."\">".mysql_result($q,$i,'result')."</A></TD>";
	}
	echo "</TR>";
}
echo "</TABLE> <A HREF=\"addtask.php\">Добавить задание</A>";
?>
</HTML>
