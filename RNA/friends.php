<?php
	switch($opcode){
		case 1301:{
			$packet = '01A3';
			$packet_raw = '';
			
			$query = mysql_query('SELECT * FROM rna_friends WHERE uid1=' . $uinfo['id'] . ' OR uid2=' . $uinfo['id']);
			
			$num_raw = dechex( mysql_num_rows($query) );
			
			$num = $packet_parser->ByteReverse($num_raw, 4, true);

			$packet_raw .= $num;	
			
			while(($resp = mysql_fetch_array($query)) != NULL){
				if($resp['uid1'] == $uinfo['id'])
					$raw_id = $resp['uid2'];
				else
					$raw_id = $resp['uid1'];
									
				$id = $packet_parser->ByteReverse($raw_id, 4);
					
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
			
			$key = unserialize($uinfo['blowfish']);
			$crypt = new pcrypt(MODE_ECB, "BLOWFISH", $key, true);
			$packet_crypt = $crypt->encrypt($packet_raw);
			$packet_add = '';
			
			for($i = 0; $i < count($packet_crypt); $i++){
				$buff = dechex($packet_crypt[$i]);
		   		$packet_add .= ((strlen($buff)<2)?'0':'') . $buff;	
			}
			
			$size = dechex(strlen($packet_add));
			
			if(strlen($size) == 3)
				$size = 'B' . $size;
			else if(strlen($size) == 2)
				$size = 'B0' . $size;
			else
				$size = 'B00' . $size;
				
			$p_size = $packet_parser->ByteReverse($size, 2, true);
			
			echo $packet . $p_size . $packet_add;
		}break;
		case 1302:{			
			$nicklen = $packet_parser->ReadByte();
			$nick = '';
						
			for($i = 0; $i < $nicklen; $i++)
				$nick .= chr(hexdec($packet_parser->ReadByte()));
			
			$packet = '02A30100';	
				
			$query = mysql_query("SELECT id FROM rna_users WHERE nick='" . $nick . "'");
			if(mysql_num_rows($query) <= 0)
				$packet .= '00';
			else{
				list($id) = mysql_fetch_row($query);
				
				$query2 = mysql_query("SELECT id FROM rna_friends WHERE (uid1=" . $uinfo['id'] . " AND uid2=" . $id . ") OR (uid1=" . $id . " AND uid2=" . $uinfo['id'] . ")");

				if(mysql_num_rows($query2) > 0)
					$packet .= '02';
				else{
					mysql_query("INSERT INTO rna_friends (uid1, uid2) VALUES (" . $uinfo['id'] . "," . $id . ")");
					$packet .= '01';
				}
			} 
			
			echo $packet;
			exit;
		}break;
		case 1303:{
			$uid = $packet_parser->ReadDword();
			
			$packet = '03A30000';
			$game_array = array();
			$game_i = 0;
			$query = mysql_query("SELECT * FROM rna_user_trophies WHERE uid" . $uid);
			
			if(mysql_num_rows($query) <= 0){
				$packet_parser->send_error();
				exit;
			}
			
			$num = $packet_parser->ByteReverse(mysql_num_rows($query), 4);
			$packet .= $num;
			
			while(($resp = mysql_fetch_array($query)) != NULL){
				if(!in_array($resp['gid'], $game_array)){
					$gid = $packet_parser->ByteReverse($resp['gid'], 4);

					$packet .= $gid;
			
					$game_array[$game_i++] = $resp['gid'];
				}
			}
		}break;
		default:
		break;
	}
?>