<?php
	$size = $packet->ReadWord();
	
	switch($size){
		case 0:
			$sesion_id = md5(time());
			break;
		
	}
?>