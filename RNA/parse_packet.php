<?php
	error_reporting(E_ERROR | E_WARNING | E_PARSE);
	
	class Packet{
		var $packet;
		var $count;
		
		function Packet($packet){
			$this->packet = $packet;
			$this->count = 0;
		}
		
		function perform_crc_check($crc){
			$safe_crc1 = $this->packet[10]; //CRC HIGH BYTE
			$safe_crc2 = $this->packet[11];	//CRC LOW BYTE
			
			$this->packet[10] = 0;
			$this->packet[11] = 0;
			
			$crc32_raw =  md5(crc32($this->packet));
			$crc32 = '';
			
			for($i = 0; $i < 16*2; $i+=2)
				$crc32 ^= ((intval($crc32_raw[$i],16) << 4) + intval($crc32_raw[$i + 1], 16));				

			$this->packet[10] = $safe_crc1;
			$this->packet[11] = $safe_crc2;
						
			if(dechex($crc32) != $crc)
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
								
			$this->count = $len*2;
			
			return $ret;	
		}
	};
	
	$packet_data = mysql_escape_string($_GET['packet']);
	$packet_data = htmlspecialchars($packet_data);
	
	$packet = new Packet($packet_data);
	
	$opcode = $packet->ReadWord();
	
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

	$constant =  0xf3;
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

	printf("%x\n", $constant);
	
	$result = (0x3F*(~$constant+0x34));
	$result = $result ^ ( $result >> 4 );
	$result = $result & 0xFF;
	$constant = $result;

	printf("%x\n\n", $constant);
?>