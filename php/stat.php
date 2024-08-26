<?
session_start();

require "const.php";
require "head.php";

drawhead(2);

$conn=mysql_connect ($dbhost,$dbuser,$dbpass) or die ("Невозможно установить соединение с БД ".mysql_error());
mysql_select_db($base1);
mysql_query("SET NAMES 'utf8'");

if ($_REQUEST["day1"])$day1=$_REQUEST["day1"];
else $day1=date("Y-m-d H:i:s",strtotime("today"));
if ($_REQUEST["day2"]) $day2=$_REQUEST["day2"];
else $day2=date("Y-m-d H:i:s",strtotime("tomorrow"));
//echo "Testing now:".$chan;
$sql="SELECT * FROM tvlist;";
//$sql1="SELECT * FROM meas WHERE ID=".$chan." AND kogda>'".date("Y-m-d H:i:s",strtotime($day1))."' AND kogda<'".date("Y-m-d H:i:s",strtotime($day2))."' ORDER BY kogda;";
//echo $sql1;
//echo $sql1;
$q=mysql_query ($sql,$conn);
//$q1=mysql_query ($sql1,$conn);
$num_c=mysql_num_rows($q);
echo "<FORM METHOD=GET ACTION=\"stat.php\" ENCTYPE=\"application/x-www-urlencoded\">";

echo "Время: с ";
echo "<INPUT TYPE=TEXT NAME=\"day1\" VALUE=\"".$day1."\"> до <INPUT TYPE=TEXT NAME=\"day2\" VALUE=\"".$day2."\">";
echo "<INPUT TYPE=SUBMIT VALUE=\"Применить\"></FORM><P><PRE>";
echo "В первой строке - анализ уровня сигнала. I - сигнал присутствует, . - сигнал отсутствует, i - неуверенный приём сигнала. Во второй строке - анализ замирания сигнала.<P>";
echo "<TABLE BORDER=1>";
for ($chan=0;$chan<$num_c;$chan++)
{
	$sql="SELECT * FROM tvlist WHERE ID=".$chan.";";
	$sql1="SELECT * FROM meas WHERE ID=".$chan." AND kogda>'".date("Y-m-d H:i:s",strtotime($day1))."' AND kogda<'".date("Y-m-d H:i:s",strtotime($day2))."' ORDER BY kogda;";
	$q=mysql_query ($sql,$conn);
	$q1=mysql_query ($sql1,$conn);
	echo "<TR><TD>".($chan+1).". ".mysql_result($q,0,"name").". ".mysql_result($q,0,"tvk")." твк. Частота ".mysql_result($q,0,"freq")." МГц.</TD></TR><TR><TD><PRE>";
	for ($i=0;$i<mysql_num_rows($q1);$i++)
	{
		echo "<A HREF=img/".mysql_result($q1,$i,"filename");
		if (mysql_result($q1,$i,"avs")>70) echo ">I</A>";
		else if (mysql_result($q1,$i,"avs")<30) echo ">.</A>";
		else echo ">i</A>";
	}
	echo "</PRE><PRE>";
	for ($i=0;$i<mysql_num_rows($q1);$i++)
	{
	        echo "<A HREF=img/".mysql_result($q1,$i,"filename");
	        if (mysql_result($q1,$i,"korr")>1e8) echo ">I</A>";
	        else echo ">.</A>";
	}
	echo "</PRE></TD></TR>";
}
echo "</TABLE> </BODY>";

?>
</PRE>
</HTML>
