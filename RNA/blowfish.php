<?php
//Usage of the blowfish class
include("blowfish.class.php");



$blowfish = new Blowfish("secret Key");
$cipher = $blowfish->Encrypt("Hello World");
$plain = $blowfish->Decrypt($cipher);

echo "Encrypted: $cipher <br>";
echo "Decrypted: $plain";

?>