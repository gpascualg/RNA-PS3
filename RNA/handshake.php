<?php		
	//Blowfish private key 4 bytes
	
	list($usec, $sec) = explode(' ', microtime());
 	srand((float) $sec + ((float) $usec * 100000));
 	
 	$byte0 = rand();
 	$byte0 = dechex( intval($byte0, 16) & 0xFF );
 	if(strlen($byte0) < 2) $byte0 = '0' . $byte0;
 	
 	$byte1 = rand();
 	$byte1 = dechex( intval($byte1, 16) & 0xFF );
 	if(strlen($byte1) < 2) $byte1 = '0' . $byte1;
 	
 	$byte2 = rand();
 	$byte2 = dechex( intval($byte2, 16) & 0xFF );
 	if(strlen($byte2) < 2) $byte2 = '0' . $byte2;
 	
 	$byte3 = rand();
 	$byte3 = dechex( intval($byte3, 16) & 0xFF );
 	if(strlen($byte3) < 2) $byte3 = '0' . $byte3;
 	
 	$chunk1 = $byte0 . $byte1 . $byte2 . $byte3;
 	$chunk2 = $packet_parser->ReadDWord();
 	
 	while(strlen($chunk2) < 8)
 		$chunk2 = '0' . $chunk2;
 	 		
 	//Blowfish priavate key
 	$key = array(intval($byte0, 16), 
 		intval($byte1, 16), 
 		intval($byte2, 16), 
 		intval($byte3, 16),
 		intval((intval($chunk2[0], 16) << 4) + intval($chunk2[1], 16), 16),
 		intval((intval($chunk2[2], 16) << 4) + intval($chunk2[3], 16), 16),
 		intval((intval($chunk2[4], 16) << 4) + intval($chunk2[5], 16), 16),
		intval((intval($chunk2[6], 16) << 4) + intval($chunk2[7], 16), 16));
 		 	
 	mysql_query('UPDATE rna_users SET `blowfish`=\'' . serialize($key) . '\' WHERE `session_id`=\'' . $session_id . '\'');
 	
 	$packet_header = '006007B055' . $chunk1;
  	
 	$crypt = new pcrypt(MODE_ECB, "BLOWFISH", $key, true);
    $packet_crypt = $crypt->encrypt('00133700');
        
    $packet = $packet_header;
        
	$len = $crypt->getlen(4);	
	for($i = 0; $i < $len; $i++){
		$buff = dechex($packet_crypt[$i]);
   		$packet .= ((strlen($buff)<2)?'0':'') . $buff;	
	}
   		
   	echo $packet;
?>