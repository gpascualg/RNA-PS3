<?php
	$link = mysql_connect('localhost',  'root', '');
	mysql_select_db('rna', $link);
	
	$size = $packet->ReadWord();
	$security = $packet->ReadByte();
	$crc = $packet->ReadByte();

	if(!$packet->perform_crc_check($crc)){
		$packet->send_error();
		exit;		
	}
		
	switch($opcode){
		case 1101:
			$packet_raw = '';
			
			$ulen = $packet->ReadByte();
	
			$name = '';
			for($i = 0; $i < $ulen; $i++)
				$name .= chr($packet->ReadByte());
				
			$name .= NULL;
				
			$md5_pass_array = $packet->ReadArray(16);
			$md5_pass = '';
			
			for($i = 0; $i < 16; $i++)
				$md5_pass .= ((strlen($md5_pass_array[$i]) < 2)?'0'.$md5_pass_array[$i]:$md5_pass_array[$i]);
			
			//Let's query for the user
			$query = mysql_query("SELECT * FROM rna_users WHERE `password`='" . $md5_pass . "' AND `nick`='" . trim($name) . "'");
			if(mysql_num_rows($query) > 0){
				$resp = mysql_fetch_array($query);
				
				$session_id = md5(md5(time()) & $md5_pass);
				
				//handshake
				//0x6000, 0x0005 + 0x2 + DWORD, 0x50, rand()
				//login
				//0x5, 0x1, user_id
				
				list($usec, $sec) = explode(' ', microtime());
 				$security = srand((float) $sec + ((float) $usec * 100000));
				$id = dechex($resp['id']);
				
				while(strlen($id) < 8)
					$id .= '0';
				
				
 				$packet_raw = "0060110050" . dechex( intval($security, 16) & 0xFF );
 				$packet_raw .= "0501" . $id;
			}else{
				//0x5, 0x1, user_id
				$packet_raw = "01A1010010";
			}
			
 			echo $packet_raw;
		break;
	}
	

?>