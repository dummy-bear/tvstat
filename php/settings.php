<?
session_start();

require "const.php";
require "head.php";

drawhead(5);

$adm_set=array("smtp-сервер","почтовый домен","имя пользователя почты","пароль от почты","резервный smtp-сервер","почтовый домен","имя пользователя почты","пароль от почты","mysql-cервер","логин mysql","пароль mysql","резервный mysql-сервер","логин mysql","пароль mysql");
$uset=array("Полное имя","E-mail (для оповещений)","Телефон (для смс-оповещений, в формате +7XXXXXXXXXX). Если вы не хотите получать смс-оповещения оставьте поле пустым или замените \"+\" перед номером на \"-\".");
$event=array("Пропадание сигнала","Появление сигнала","Застывшее изображение","Окончание замороженности изображения","Чёрное/белое поле","Окончание чёрного/белого поля");




$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_select_db($base1);
mysql_query("SET NAMES 'utf8'");


/*
if ($_REQUEST["button1"]=="Применить изменения личных настроек")
{
	$sql="UPDATE users SET fullname=\"".$_REQUEST["uset0"]."\",email=\"".$_REQUEST["uset1"]."\",phone=\"".$_REQUEST["uset2"]."\" WHERE ID=".$_SESSION["uid"].";";
	$q=mysql_query ($sql,$conn) or die("Не удалось сохранить настройки");
}
else if ($_REQUEST["button1"]=="Применить изменения оповещений")
{
	$tn=array("sigoff","sigon","freeze","unfreeze","splash","unsplash");

	$sql="UPDATE users SET ";
	for ($i=0;$i<6;$i++) if ($_REQUEST["event".$i]=="on") $sql=$sql.$tn[$i]."=true,";
			     else $sql=$sql.$tn[$i]."=false,";
	$sql=$sql."useplan='a' WHERE ID=".$_SESSION["uid"].";";
	//echo $sql;
	$q=mysql_query ($sql,$conn);
}

*/
echo "<FORM METHOD=POST ACTION=\"settings.php\">";
if ($_SESSION["grp"]==1) 
{
	$sql="SELECT ID,descr,value FROM settings ORDER BY ID;";
	$q=mysql_query ($sql,$conn) or die("Не удалось сохранить настройки");

	for ($i=0;$i<mysql_num_rows($q);$i++) echo mysql_result($q,$i,"descr").": <INPUT TYPE=\"text\" NAME=\"s".mysql_result($q,$i,"ID")."\" VALUE=\"".mysql_result($q,$i,"value")."\"><BR>\n";

	echo "<H2>Настройки видео</H2><BR>";
	echo "Устройство для сканирования тв <INPUT TYPE=\"text\" NAME=\"devscan\" VALUE=\"/dev/video1\"><BR>";	
	echo "Устройство для заданий (запись/вещание видео контента) <INPUT TYPE=\"text\" NAME=\"dev1\" VALUE=\"/dev/video0\"><BR>";
	echo "<INPUT TYPE=\"submit\" name=\"button1\" value=\"Применить изменения устройств\">";

	echo "<H2>Общие настройки системы</H2>";
	//$sql="SELECT fullname,email,phone FROM users WHERE ID=".$_SESSION["uid"].";";
	//$q=mysql_query ($sql,$conn);
	for ($i=0;$i<14;$i++) echo $adm_set[$i].": <INPUT TYPE=\"text\" NAME=\"admset".$i."\" VALUE=\""/*.mysql_result($q,0,$i)*/."\"><P>";
	echo "<INPUT TYPE=\"submit\" name=\"button1\" value=\"Применить изменения общих настроек\">";


	echo "<H2>Список сканируемых каналов</H2>";

	$sql="SELECT * FROM tvlist ORDER BY ID;";
	$q=mysql_query ($sql,$conn);
//echo session_id();
	echo "<TABLE BORDER=2> <TR><TH>порядковый номер</TH><TH>Номер ТВК </TH><TH>Частота (МГц)</TH><TH>Программа вещания</TH><TH>Общеобязательный?</TH></TR>";
	for ($i=0;$i<mysql_num_rows($q);$i++) 
	{
		echo "<TR>";
		for ($j=0;$j<4;$j++) echo "<TD><INPUT TYPE=\"text\" NAME=\"ch".$i.$j."\" VALUE=\"".mysql_result($q,$i,$j)."\"></TD>";
		if (mysql_result ($q,$i,"state")=="O") echo "<TD><INPUT TYPE=\"checkbox\" NAME=\"ch".$i.$j."\" CHECKED></TD>";
		else echo "<TD><INPUT TYPE=\"checkbox\" NAME=\"ch".$i.$j."\"></TD>";
		echo "</TR>";
	}
	echo "</TABLE><BR>";
	echo "<INPUT TYPE=\"submit\" name=\"button1\" value=\"Применить изменения списка каналов\">";
}
if ($_SESSION["uid"])
{
	echo "<H2>Пользовательские настройки</H2>";
	$sql="SELECT fullname,email,phone FROM users WHERE ID=".$_SESSION["uid"].";";
	$q=mysql_query ($sql,$conn);
	for ($i=0;$i<3;$i++) echo $uset[$i]."<BR><INPUT TYPE=\"text\" NAME=\"uset".$i."\" VALUE=\"".mysql_result($q,0,$i)."\"><P>";
	echo "<INPUT TYPE=\"submit\" name=\"button1\" value=\"Применить изменения личных настроек\">";

	echo "<H2>Оповещения по электронной почте и смс</H2>";

	$sql="SELECT sigoff,sigon,freeze,unfreeze,splash,unsplash,useplan FROM users WHERE ID=".$_SESSION["uid"].";";
	$q=mysql_query ($sql,$conn);
	for ($i=0;$i<6;$i++) if (mysql_result($q,0,$i)) echo "<INPUT TYPE=\"checkbox\" NAME=\"event".$i."\" CHECKED>".$event[$i]."<P>";
			     else echo "<INPUT TYPE=\"checkbox\" NAME=\"event".$i."\">".$event[$i]."<P>";
	//echo "В случае запланированных отключений";
	echo "<INPUT TYPE=\"submit\" name=\"button1\" value=\"Применить изменения оповещений\"></FORM>";

/*
	$sql1="SELECT * FROM meas WHERE ID=".$i." ORDER BY kogda DESC limit 0,1;";
	$q1=mysql_query ($sql1,$conn) or die();
	if (!($i%2)) echo "<TR>";
	echo "<TD><A HREF=stat.php?channel=".$i."><IMG SRC=\"img/".mysql_result($q1,0,"filename")."\" WIDTH=180 HEIGHT=135 ></A></TD>";
	echo "<TD>".mysql_result($q,$i,"name")."<BR> Частота ".mysql_result ($q,$i,"freq");
	echo "МГц<BR>Время скриншота: ".mysql_result($q1,0,"kogda")."<BR> Уровень: ".mysql_result($q1,0,"avs")."</TD>";
	if ($i%2) echo "</TR>";
}*/
}
echo "</TD></TR></TABLE></BODY>\n</HTML>";


?>
