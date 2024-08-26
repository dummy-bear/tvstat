<?
session_start();

require "const.php";

/*session_unregister("login");
session_unregister("uid");
session_unregister("uname");
session_unregister("pass");
session_unregister("grp");
*/
session_destroy();
echo "<META http-equiv=\"Refresh\" content=\"0;url=index.php\">";
?>
