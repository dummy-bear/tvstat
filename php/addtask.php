<HTML>
<META http-equiv="Content-Type" content="text/html"; charset=utf-8>
<TITLE>Создание задания на запись или вещание ТВ</TITLE>
<BODY>

<H1>Добавить задание</H1>

Время старта и время окончания задания указываются с точностью до минуты. <P>

<FORM METHOD=POST ACTION="updtask.php" ENCTYPE="application/x-www-form-urlencoded">
<INPUT TYPE=hidden NAME="ID" VALUE="-1">
Введите имя и пароль, чтобы подтвердить своё право добавлять задания: <BR>
Имя: <INPUT TYPE=TEXT NAME="login"> Пароль: <INPUT TYPE=PASSWORD NAME="passwd"> <P>

Тип задания: 
<SELECT NAME="typzad">
	<OPTION VALUE="r">Запись ТВ
	<OPTION VALUE="b">Вещание ТВ
	<OPTION VALUE="A">Запись РВ
</SELECT> <P>

Частота: <INPUT TYPE=text NAME="freq1"> МГц<P>
Время выполнения задания:<BR>
<?
echo "Старт в <INPUT TYPE=text NAME=\"tstart\" VALUE=\"".date("H:i d.m.Y")."\"> завершение в <INPUT TYPE=text NAME=\"tstop\" VALUE=\"".date("H:i d.m.Y")."\"> <P>";
?>
Кодировка сигнала:
<SELECT NAME="cod">
        <OPTION VALUE=17>SECAM</OPTION>
        <OPTION VALUE=8>PAL</OPTION>
</SELECT><P>
<INPUT TYPE=HIDDEN NAME="bitrate" VALUE=10>
Комментарий к заданию:<BR>
<INPUT TYPE=text NAME="comment" size=80><BR>

<INPUT TYPE=submit NAME="submit" VALUE="Добавить">
</FORM>
</HTML>
