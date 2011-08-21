<?php
	error_reporting(E_ERROR | E_WARNING | E_PARSE);
	
	include_once ("crypt.php");
	 	
	$link = mysql_connect('localhost',  'root', '');
	mysql_select_db('rna', $link);
		
	class Packet{
		var $packet;
		var $count;
		var $packet_len;
		
		function Packet($packet){
			$this->packet = $packet;
			$this->packet_len = strlen($this->packet);
			$this->count = 0;
		}
		
		function perform_crc_check($crc){
			$crc_packet = strtoupper($this->packet);
			
			$crc_packet[10] = "0";
			$crc_packet[11] = "0";
			
			$crc32_raw =  strtoupper( dechex( crc32($crc_packet) ) );

			$crc32_raw = md5($crc32_raw);
			$crc32 = '';
			
			for($i = 0; $i < 16*2; $i+=2)
				$crc32 ^= ((intval($crc32_raw[$i],16) << 4) + intval($crc32_raw[$i + 1], 16));				

			if(dechex($crc32) != $crc)
				return 0;
			else
				return 1;		
		}
		
		function perform_security_check($security, $session_id, $s = false){
			$query = mysql_query('SELECT `security` FROM rna_users WHERE `session_id`=\'' . $session_id . '\'');
			$resp = mysql_fetch_array($query);
			
			$constant =  $resp['security'];
			$result = intval(0x3F*((~intval($constant, 16))+0x34), 16);
			$result = $result ^ ( $result >> 4 );
			$result = $result & 0xFF;
			
			$result = dechex($result);
			$result = ((strlen($result) < 2)?'0'.$result:$result);
			
			mysql_query('UPDATE rna_users SET `security`=\'' . $result . '\' WHERE `session_id`=\'' . $session_id . '\'');
			
			if(strtoupper($result) != strtoupper($security))
				return 0;
			else
				return 1;	
		}
		
		function decrypt($un_blowfish_key){
			$un_crypt = new pcrypt(MODE_ECB, "BLOWFISH", $un_blowfish_key, true);
			$packet_raw = '';
			$packet_crypt = '';
						
			for($i = 0; $i < $this->count; $i++)
				$packet_raw .= $this->packet[$i];

			for($i = $this->count; $i < $this->packet_len; $i+=2)
				$packet_crypt .= chr( ( intval( $this->packet[$i] , 16) << 4) + intval( ($this->packet[$i+1]) , 16) );
			
			$packet_decrypt = $un_crypt->decrypt($packet_crypt);
			
			$this->packet = $packet_raw;
			
			for($i = 0; $i < count($packet_decrypt); $i++){
				$buff = dechex($packet_decrypt[$i]);
				$this->packet .= ((strlen($buff)<2)?'0':'') . $buff;
			}
			 				
			unset($un_crypt);
		}
		
		function send_error(){
			if(isset($uinfo) && is_array($uinfo))
				mysql_query("UPDATE rna_users SET dc=1 WHERE id=" . $uinfo['id']);
			echo "CCCC";	
			exit;
		}
		
		function ByteReverse($block, $block_size, $hex=false){
			if(!$hex)
				$block = dechex($block);
			$reversed_block = '';
			
			if((strlen($block) % 2) != 0)
				$block = '0' . $block;
	
			$block_len = strlen($block);
			for($i = 0; $i < $block_size*2; $i+=2){
				if($block_len > $i){
					$reversed_block .= $block[$block_len - 2 - $i];
					$reversed_block .= $block[$block_len - 1 - $i];
				}else
					$reversed_block .= '00';						
			}	
				
			return $reversed_block;
		}
		
		function ReadByte(){
			if(!isset($this->packet) || $this->packet == '')
				return -1;
				
			$byte = dechex(((intval($this->packet[$this->count],16) << 4) + intval($this->packet[$this->count + 1], 16)));
			$this->count += 1*2;
			
			return $byte;
		}
		
		function ReadWord(){
			if(!isset($this->packet) || $this->packet == '')
				return -1;
				
			$word = dechex(((intval($this->packet[$this->count], 16) << 4) + intval($this->packet[$this->count + 1],16)) + (((intval($this->packet[$this->count + 2], 16) << 4) + intval($this->packet[$this->count + 3], 16)) << 8));
			$this->count += 2*2;
			
			return $word;
		}
		
		function ReadDWord(){
			if(!isset($this->packet) || $this->packet == '')
				return -1;
				
			$dword = dechex(((intval($this->packet[$this->count],16) << 4) + intval($this->packet[$this->count + 1],16)) + (((intval($this->packet[$this->count + 2],16) << 4) + intval($this->packet[$this->count + 3],16)) << 8) + (((intval($this->packet[$this->count + 4],16) << 4) + intval($this->packet[$this->count + 5],16)) << 16) + (((intval($this->packet[$this->count + 6],16) << 4) + intval($this->packet[$this->count + 7], 16) << 24)));
			$this->count += 4*2;
			
			return $dword;
		}
		
		function ReadArray($len){
			$ret = array();
						
			for($i = 0; $i < $len*2; $i += 2)				
				$ret[intval($i/2)] = dechex(intval((intval($this->packet[$this->count + $i],16) << 4) + intval($this->packet[$this->count + $i + 1], 16), 16));
								
			$this->count += $len*2;
			
			return $ret;	
		}
	};
	
	$packet_data = mysql_escape_string($_GET['packet']);
	$packet_data = htmlspecialchars($packet_data);
	
	$packet_parser = new Packet($packet_data);
	
	$opcode = $packet_parser->ReadWord();
	$size = $packet_parser->ReadWord();
	$security = $packet_parser->ReadByte();
	$crc = $packet_parser->ReadByte();

	//OPCODES QUE NO UTILIZAN SESSION_ID SIEMPRE
	switch( intval((intval($opcode, 16) & 0xFF00), 16) >> 8 ){
		//LOGIN
		case 0x11: // -> A1
			include_once('login.php');
			exit;
		break;
		default:
			break;
	}
	
	//OPCODES CON SESSION_ID Y CHECKS
	
	$session_id = '';	
	$uinfo = NULL;
	
	$session_id_raw = $packet_parser->ReadArray(16);
	for($i = 0; $i < 16; $i++)
		$session_id .= ((strlen($session_id_raw[$i]) < 2)?'0'.$session_id_raw[$i]:$session_id_raw[$i]);
	$session_id = strtoupper($session_id);
		
	if(!$packet_parser->perform_security_check($security, $session_id)){
		$packet_parser->send_error();
		exit;		
	}
	
	if(!$packet_parser->perform_crc_check($crc)){
		$packet_parser->send_error();
		exit;		
	}
	
	$query = mysql_query('SELECT * FROM rna_users WHERE session_id=\'' . $session_id . '\'');
	if(mysql_num_rows($query) <= 0){
		$packet_parser->send_error();
		exit;
	}
	$uinfo = mysql_fetch_array($query);
		
	if( intval(intval($size, 16) & 0xF000, 16) == 0xB000 ){				
		$un_blowfish_key = unserialize($uinfo['blowfish']);		
		$packet_parser->decrypt($un_blowfish_key);		
	}
	
	
	switch($opcode){
		case 6000: //handshake
			include_once ('handshake.php');
			exit;
		break;
	}
	
	if((int)$uinfo['dc'] == 1){
		$packet_parser->send_error();
		break;
	}
	
	switch( intval((intval($opcode, 16) & 0xFF00), 16) >> 8 ){
		case 0x12:
			include_once('trophy.php');
			exit;
		break;
		case 0x13:
			include_once('friends.php');
			exit;
		break;
		default:
			break;
	}
	
	echo "CCCC";
	
	exit;
?>