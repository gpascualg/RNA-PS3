<?php
	error_reporting(E_ERROR | E_WARNING | E_PARSE);
	
	$link = mysql_connect('localhost',  'root', '');
	mysql_select_db('rna', $link);
		
	class Packet{
		var $packet;
		var $count;
		
		function Packet($packet){
			$this->packet = $packet;
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
		
		function perform_security_check($security, $session_id){
			$query = mysql_query('SELECT `security` FROM rna_users WHERE `session_id`=\'' . $session_id . '\'');
			$resp = mysql_fetch_array($query);
			
			$constant =  $resp['security'];
			$result = intval(0x3F*((~intval($constant, 16))+0x34), 16);
			$result = $result ^ ( $result >> 4 );
			$result = $result & 0xFF;
			
			mysql_query('UPDATE rna_users SET `security`=\'' . ((strlen($result) < 2)?'0'.$result:$result) . '\' WHERE `session_id`=\'' . $session_id . '\'');
			
			if(strtoupper(dechex($result)) != strtoupper($security))
				return 0;
			else
				return 1;	
		}
		
		function send_error(){
			echo "CCCC";	
			exit;
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
	
	$packet = new Packet($packet_data);
	
	$opcode = $packet->ReadWord();
	$size = $packet->ReadWord();
	$session_id = '';
	
	$uinfo = NULL;
	
	if(intval($size, 16) & 0xB000){
		$session_id_raw = $packet->ReadArray(16);
		for($i = 0; $i < 16; $i++)
			$session_id .= ((strlen($session_id_raw[$i]) < 2)?'0'.$session_id_raw[$i]:$session_id_raw[$i]);
			
		$query = mysql_query('SELECT * FROM rna_users WHERE session_id=\'' . $session_id . '\'');
		if(mysql_num_rows($query) <= 0){
			$packet->send_error();
			exit;
		}
		
		$uinfo = mysql_fetch_array($query);
	}
	
	switch($opcode){
		case 6000: //handshake
			include_once ('handshake.php');
		break;
	}

	switch( dechex(intval((intval($opcode, 16) & 0xFF00), 16) >> 8) ){
		//LOGIN
		case 11: // -> A1
			include_once('login.php');
		break;
	}
	
	exit;

	

	printf("%x\n", $constant);

	$result = (0x3F*(~$constant+0x34));
	$result = $result ^ ( $result >> 4 );
	$result = $result & 0xFF;
	$constant = $result;

	printf("%x\n", $constant);
	
	$result = (0x3F*(~$constant+0x34));
	$result = $result ^ ( $result >> 4 );
	$result = $result & 0xFF;
	$constant = $result;

	printf("%x\n", $constant);
	
	$result = (0x3F*(~$constant+0x34));
	$result = $result ^ ( $result >> 4 );
	$result = $result & 0xFF;
	$constant = $result;

	printf("%x\n\n", $constant);
?>