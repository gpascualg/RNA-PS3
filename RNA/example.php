<?php
$text = trim($_POST["text"]);
$key = $_POST["key"];


//Wurde kein Key &uuml;bertragen? Wenn nein, 8 stelligen Key zuf&auml;llig erstellen
if(!isset($key))
   {
	mt_srand(crc32(microtime()));
      $buchstaben = array("","a","b","c","d","e","f","g","h","i","j","k","m","n","p","q","r","s","t","u","v","w","x","y","z",1,2,3,4,5,6,7,8,9);
	$array_max = count($buchstaben)-1;

	for($i=0;$i < 8;$i++)
   	{
      $rand_num = mt_rand(1, $array_max);
      $key .= $buchstaben[$rand_num];
      $a++;
      }
   }

?>

<form action="#ausgabe" method="post">

Key:<br>
<input type="Text" name="key" value="<?php echo htmlentities($key); ?>" maxlength="16"><br>      <br>

Text:<br>
<textarea name="text" cols="45" rows="9"></textarea> <br> <br>


<input name="crypt" type="Submit" value="Verschluesseln"> &nbsp;&nbsp;&nbsp;
<input name="decrypt" type="Submit" value="Entschluesseln">

</form>


<a name="ausgabe"></a>
<?php
include("blowfish.class.php"); 


if(isset($_POST["crypt"]))
	{
	$start = (double)microtime()+time();
	$blowfish = new Blowfish($key);
      $ausgabe = $blowfish->Encrypt($text);
	$diff = (double)microtime()+time() - $start;

   echo "<br><b>Verschl&uuml;sselt</b> sieht der Text so aus:<br>
   <textarea name=\"ausgabe\" cols=\"45\" rows=\"9\">".
   $ausgabe."</textarea><br>Benötigte Zeit: ".round($diff, 3)." Sekunden";
	}

else if(isset($_POST["decrypt"]))
	{
	$start = (double)microtime()+time();
	$blowfish = new Blowfish($key);
      $ausgabe =$blowfish->Decrypt($text);
	$diff = (double)microtime()+time() - $start;

      echo  "<b>Entschl&uuml;sselt</b> sieht der Text so aus:<br>".
	nl2br(htmlentities(stripslashes($ausgabe)))."<br><hr><br>Benötigte Zeit: ".round($diff, 3)." Sekunden";
	}
?>