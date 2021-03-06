<?php
	if(!$packet_parser->perform_crc_check($crc)){
		$packet_parser->send_error();
		exit;		
	}
		
	switch($opcode){
		case 1101:
			$packet_raw = '';
			
			$ulen = $packet_parser->ReadByte();
	
			$name = '';
			for($i = 0; $i < $ulen; $i++)
				$name .= chr(hexdec($packet_parser->ReadByte()));
				
			$name .= NULL;
				
			$md5_pass_array = $packet_parser->ReadArray(16);
			$md5_pass = '';
			
			for($i = 0; $i < 16; $i++)
				$md5_pass .= ((strlen($md5_pass_array[$i]) < 2)?'0'.$md5_pass_array[$i]:$md5_pass_array[$i]);
				
			$md5_pass = strtolower($md5_pass);
				
			//Let's query for the user
			$query = mysql_query("SELECT * FROM rna_users WHERE `password`='" . $md5_pass . "' AND `nick`='" . trim($name) . "'");
			if(mysql_num_rows($query) > 0){
				$resp = mysql_fetch_array($query);
				
				mysql_query("UPDATE rna_users SET dc=0 WHERE id=" . $resp['id']);
				
				$session_id = md5(md5(time()) & $md5_pass);
				$n = 0;
				while(mysql_num_rows( mysql_query('SELECT id FROM rna_users WHERE `session_id`=\'' . $session_id . '\'') ) > 0){
					$session_id = md5(md5(time()) & $md5_pass + $n);
					$n++;
				}
							
				//handshake
				//0x6000, 0x0005 + 0x2 + DWORD + 16, 0x50, rand()
				//login
				//0x5, 0x1, user_id
				
				list($usec, $sec) = explode(' ', microtime());
 				srand((float) $sec + ((float) $usec * 100000));
 				$security = rand();
 				$security = dechex( intval($security, 16) & 0xFF );
								
				$id = $packet_parser->ByteReverse($resp['id'], 4); 
									
				while(strlen($session_id) < 32)
					$session_id = '0' . $session_id;	
					
				mysql_query('UPDATE rna_users SET `session_id`=\'' . strtoupper($session_id) . '\', `security`=\'' . $security . '\'  WHERE `password`=\'' . $md5_pass . '\' AND `nick`=\'' . trim($name) . '\'');
								
 				$packet_raw = "0060210050" . ((strlen($security)<2)?'0':'') . $security;
 				$packet_raw .= "0501" . $id . $session_id;
			}else{
				//0x5, 0x1, user_id
				$packet_raw = "01A1010010";
			}
			
 			echo $packet_raw;
 			exit;
		break;
	}	
?>