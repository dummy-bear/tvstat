<?
session_start();

require "const.php";
require "head.php";

drawhead(1);

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_select_db($base1);
mysql_query("SET NAMES 'utf8'");
$sql="SELECT * FROM tvlist ORDER BY ID;";
$q=mysql_query ($sql,$conn);
$num_c=mysql_num_rows($q);
echo "<TABLE BORDER=2>";
for ($i=0;$i<$num_c;$i++)
{
	$sql1="SELECT * FROM meas WHERE ID=".$i." ORDER BY kogda DESC limit 0,1;";
	$q1=mysql_query ($sql1,$conn) or die();
	if (!($i%3)) echo "<TR>";
	echo "<TD><A HREF=stat.php?channel=".$i."><IMG SRC=\"img/".mysql_result($q1,0,"filename")."\" WIDTH=180 HEIGHT=135 ></A></TD>";
	echo "<TD>".mysql_result($q,$i,"name")." - ".mysql_result($q,$i,"tvk")."твк <BR> Частота ".mysql_result ($q,$i,"freq");
	echo "МГц<BR>Время скриншота: ".mysql_result($q1,0,"kogda")."<BR> Уровень: ".mysql_result($q1,0,"avs")."<BR>Система цветности: ";
	if (mysql_result($q1,0,"cod")==4) echo "PAL";
	else echo "SECAM";
	echo "</TD>";
	if (($i%3==2)||(i==$num_c-1)) echo "</TR>";
}
echo "</TABLE><P>";

?>
</BODY>
</HTML>
