<?php
	switch($opcode){
		case 1201:
			$gamename_raw = $packet->ReadArray(16);
			$gamename = '';			
			
			for($i = 0; $i < 16; $i++)
				$gamename .= chr(intval((strlen($gamename_raw[$i]) < 2)?'0'.$gamename_raw[$i]:$gamename_raw[$i], 16));
		
			$query = mysql_query("SELECT id FROM rna_trophies WHERE name='" . $gamename . "'");
			
			$packet = '01A20100';
			if(mysql_num_rows($query) > 0)
				$packet .= '01';
			else
				$packet .= '00';
				
			echo $packet;
			exit;
		break;
		case 1202:{			
			$gamename_raw = $packet->ReadArray(16);
			$gamename = '';			
			
			for($i = 0; $i < 16; $i++)
				$gamename .= chr(intval((strlen($gamename_raw[$i]) < 2)?'0'.$gamename_raw[$i]:$gamename_raw[$i], 16));
					
			$max_trophies = $packet->ReadByte();
			
			//fputs($fp, $_POST['ICON0'], strlen($_POST['ICON0']));
							
			for($i = 0; $i < intval($max_trophies,16); $i++){
				//fputs($fp, $_POST['TROP' . $i], strlen($_POST['TROP' . $i]));
					
			}
		}
		default:
		break;
	}
?>