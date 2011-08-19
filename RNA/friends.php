<?php
	switch($opcode){
		case 1301:{
			$packet = '01A3';
			$packet_raw = '';
			
			$query = mysql_query('SELECT * FROM rna_friends WHERE uid1=' . $uinfo['id'] . ' OR uid2=' . $uinfo['id']);
			
			$num_raw = dechex( mysql_num_rows($query) );
			
			$num = '';
			if(strlen($num_raw) > 1)
				for($i = strlen($num_raw)-1; $i > 0; $i-=2)
					$num .= $num_raw[$i-1] . $num_raw[$i];
			else
				$num = $num_raw;
				
			if((strlen($num) % 2) != 0)
				$num = '0' . $num;
			
			while(strlen($num) < 8)
				$num .= '0';

			$packet_raw .= $num;	
			
			while(($resp = mysql_fetch_array($query)) != NULL){
				if($resp['uid1'] == $uinfo['id'])
					$raw_id = $resp['uid2'];
				else
					$raw_id = $resp['uid1'];

				$id = '';
				$raw_id = dechex($raw_id);
				if(strlen($raw_id) > 1)
					for($i = strlen($raw_id)-1; $i > 0; $i-=2)
						$id .= $raw_id[$i-1] . $raw_id[$i];
				else
					$id = $raw_id;

				if((strlen($id) % 2) != 0)
					$id = '0' . $id;
				
				while(strlen($id) < 8)
					$id .= '0';
					
				$packet_raw .= $id;
			
				$query2 = mysql_query("SELECT * FROM rna_users WHERE id=" . intval($raw_id, 16));
				$resp2 = mysql_fetch_array($query2);
								
				$nicklen = dechex( strlen($resp2['nick']) );
				$nicklen = ((strlen($nicklen) < 2)?'0':'') . $nicklen;
				$packet_raw .= $nicklen;
				
				for($i = 0; $i < intval($nicklen, 16); $i++){
					$buff = dechex( ord($resp2['nick'][$i]) );
					$packet_raw .= ((strlen($buff) < 2)?'0':'') . $buff;
				}
			}
			
			//$packet_raw .= '\0\0\0\0\0\0\0';
			
			$key = unserialize($uinfo['blowfish']);
			$crypt = new pcrypt(MODE_ECB, "BLOWFISH", $key, true);
			$packet_crypt = $crypt->encrypt($packet_raw);
			$packet_add = '';
			
			for($i = 0; $i < count($packet_crypt); $i++){
				$buff = dechex($packet_crypt[$i]);
		   		$packet_add .= ((strlen($buff)<2)?'0':'') . $buff;	
			}
			
			$size = strlen($packet_add);
			if(strlen($size) == 3){
				$p_size = dechex((intval($size,16) & 0xFF0) >> 4);
				$p_size = ((strlen($p_size) < 2)?'0':'') . $p_size;
				$p_size .= 'B' . dechex( intval($size, 16) & 0xF ); 
			}else if(strlen($size) == 2)
				$p_size = $size . 'B0';
			else
				$p_size = '0' . $size . 'B0';
				
			echo $packet . $p_size . $packet_add;
		}break;
		case 1302:{			
			
		}
		default:
		break;
	}
?>