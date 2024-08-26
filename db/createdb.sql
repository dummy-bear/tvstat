create database tvstat;
use tvstat;
CREATE TABLE `users` (
  `ID` int(11) default NULL,
  `name` varchar(256) default NULL,
  `password` varchar(256) default NULL,
  `fullname` varchar(1024) default NULL,
  `rs` char(255) default NULL,
  `salt` char(255) default NULL,
  `grp` int(11) default NULL,
  `email` varchar(255) default NULL,
  `phone` varchar(255) default NULL,
  `comment` text,
  `sigoff` tinyint(1) default NULL,
  `sigon` tinyint(1) default NULL,
  `freeze` tinyint(1) default NULL,
  `unfreeze` tinyint(1) default NULL,
  `splash` tinyint(1) default NULL,
  `unsplash` tinyint(1) default NULL,
  `useplan` char(1) default NULL
) CHARSET=utf8;

CREATE TABLE `tvlist` (
  `ID` int(11) default NULL,
  `tvk` int(11) default NULL,
  `freq` float default NULL,
  `name` varchar(256) default NULL,
  `state` char(1) default NULL,
  `kanal` int(11) default NULL,
  `cod` int(11) default NULL
) CHARSET=utf8;

CREATE TABLE `tasks` (
  `ID` int(11) default NULL,
  `start` datetime default NULL,
  `stop` datetime default NULL,
  `typ` char(1) default NULL,
  `freq` float default NULL,
  `userid` int(11) default NULL,
  `result` varchar(1024) default NULL,
  `status` char(1) default NULL
) CHARSET=utf8;

CREATE TABLE `meas` (
  `ID` int(11) default NULL,
  `kogda` datetime default NULL,
  `avs` int(11) default NULL,
  `bright` float default NULL,
  `disp` float default NULL,
  `korr` float default NULL,
  `filename` varchar(256) default NULL
) CHARSET=utf8;

CREATE TABLE `sob` (
  `ID` int(11) NOT NULL,
  `text` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `settings` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  `descr` varchar(8048) DEFAULT NULL,
  `value` varchar(8048) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;



INSERT INTO users VALUES (1,"admin","","tvstat administrator",md5(rand()),"",0,"","","",false,false,false,false,false,false,'a');
UPDATE users SET salt=md5(concat(md5(concat(md5("admin"),rs)),rs));

DROP TABLE IF EXISTS `sob`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sob` (
  `ID` int(11) NOT NULL,
  `text` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sob`
--

LOCK TABLES `sob` WRITE;
/*!40000 ALTER TABLE `sob` DISABLE KEYS */;
INSERT INTO `sob` VALUES (101,'Пропал сигнал обязательного канала '),(102,'Появился сигнал обязательного канала '),(103,'Замерло изображение обязательного канала '),(104,'Снова подвижно изображение обязательного канала '),(111,'Пропал сигнал (профилактика) обязательного канала '),(112,'Появился сигнал (завершилась профилактика) обязательного канала '),(113,'Замерло изображение (профилактика) обязательного канала '),(114,'Снова подвижно изображение (завершилась профилактика) обязательного канала '),(121,'Пропал сигнал телеканала '),(122,'Появился сигнал телеканала '),(123,'Замерло изображение телеканала '),(124,'Снова подвижно изображение телеканала '),(125,'Сменилась кодировка канала '),(131,'Пропал сигнал (профилактика) телеканала '),(132,'Появился сигнал (завершилась профилактика) телеканала '),(133,'Замерло изображение (профилактика) телеканала '),(134,'Снова подвижно изображение (завершилась профилактика) телеканала '),(200,'Не заладилось выполнение задания ');
/*!40000 ALTER TABLE `sob` ENABLE KEYS */;
UNLOCK TABLES;
