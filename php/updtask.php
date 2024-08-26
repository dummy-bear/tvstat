<HTML>

<?

$id=$_REQUEST["ID"];
$user=$_REQUEST["login"];
$pass=$_REQUEST["passwd"];
$typzad=$_REQUEST["typzad"];
$freq=$_REQUEST["freq1"];
$tstart=strtotime($_REQUEST["tstart"]);
$tstop=strtotime($_REQUEST["tstop"]);
$bitrate=$_REQUEST["bitrate"];
$comment=$_REQUEST["comment"];

$conn=mysql_connect ("localhost","root","gjhvtygfh") or die ("Невозможно установить соединение с БД ".mysql_error());
$base="tvstat";
mysql_select_db($base);

$sqlu="SELECT ID FROM users WHERE name=\"".$_REQUEST["login"]."\" AND password=\"".$_REQUEST["passwd"]."\";";
$qu=mysql_query($sqlu,$conn);
if (mysql_num_rows($qu)==1)
{
	$uid=mysql_result($qu,0,'ID');
	echo $uid." - ".$tstart." - ".$tstop;
	if ($tstart<$tstop)
	{
		$sql="SELECT tasks.ID,start,stop,typ,freq,fullname,status,result FROM tasks,users where tasks.userid=users.ID AND start<\"".date("Y-m-d H:i:s",$tstop)."\" AND stop>\"".date("Y-m-d H:i:s",$tstart)."\" AND tasks.ID!=".$id." ORDER BY start;";
		echo "\n".$sql."\n";
		$q=mysql_query($sql,$conn);
		if (mysql_num_rows($q)>0)
		{
			echo "<H1>Этому заданию будут мешать следующие:</H1>\n<TABLE BORDER=1><TR><TH>Время старта</TH><TH>Время завершения</TH><TH>Тип задания</TH><TH>Частота, МГц</TH><TH>Автор</TH><TH>Статус задания</TH></TR>";

			for ($i=0;$i<mysql_num_rows($q);$i++)
			{
				echo "<TR>";
				for ($j=1;$j<6;$j++) echo "<TD>".mysql_result($q,$i,$j)."</TD>";
				if (mysql_result($q,$i,'status')=="w") echo "<TD>Ожидается выполнение.<BR><A HREF=\"edittask.php?task=".mysql_result($q,$i,'ID')."\">Редактировать</A>  <A HREF=\"deletetask.php?task=".mysql_result($q,$i,'ID')."\">Удалить</A></TD>";
				else if (mysql_result($q,$i,'status')=="r") 
				{
			//		echo "<TD>Выполняется сейчас. <BR>";
					if (mysql_result($q,$i,'typ')=='b') echo "<TD BGCOLOR=YELLOW>Идёт вещание. Откройте в vlc адрес http://192.168.16.172:9001 </TD>";
					else echo "<TD BGCOLOR=YELLOW>Идёт запись</TD>";
				}
				else if (mysql_result($q,$i,'status')=="s") 
				{
					if (mysql_result($q,$i,'typ')=='b') echo "<TD>Вещание уже завершено. </TD>";
                			else echo "<TD>Записан файл <A HREF=\"video/".mysql_result($q,$i,'result')."\">".mysql_result($q,$i,'result')."</A></TD>";
				}
				echo "</TR>";
			}
			echo "</TABLE>";
		}
		else
		{
//Добавляем задание
			if ($id==-1)
			{
				$sql="SELECT ID FROM tasks ORDER BY ID DESC LIMIT 0,1;";
				$q=mysql_query($sql,$conn);
				$id=mysql_result($q,$i,'ID')+1;
				$sql="INSERT INTO tasks VALUES (".$id.", '".date("Y-m-d H:i:s",$tstart)."', '".date("Y-m-d H:i:s",$tstop)."', '".$typzad."', ".$freq.", ".$uid.", \"\", 'w',".$bitrate.",\"".$comment."\", ".$_REQUEST["cod"].");";
				echo "<P>".$sql."\n <P>";
				$q=mysql_query($sql,$conn);
			}
			else
			{
				$sql="SELECT userid FROM tasks WHERE ID=".$id.";";
				$q=mysql_query($sql,$conn);
				if (mysql_result($q,0,0)>$uid) 
				{
					$sql="DELETE FROM tasks WHERE ID=".$id.";";
					$q=mysql_query($sql,$conn);
					$sql="INSERT INTO tasks VALUES (".$id.", '".date("Y-m-d H:i:s",$tstart)."', '".date("Y-m-d H:i:s",$tstop)."', '".$typzad."', ".$freq.", ".$uid.", \"\", 'w',".$bitrate.",\"".$comment."\", ".$_REQUEST["cod"].");";
					$q=mysql_query($sql,$conn);
				}
				else echo "Это задание добавлено более приоритетным пользователем!";
			}
			echo "Задание добавлено(изменено). <A HREF=task.php>Перейти к списку.</A>";
			
		}
	}
	else echo "Время старта должно быть раньше, чем время остановки!";
}
else if (mysql_num_rows($qu)==0) echo "Не прошла авторизация";
else if (mysql_num_rows($qu)>1) echo "Ошибка проверки авторизации. Сообщите об этом администратору.";
?>
</HTML>
