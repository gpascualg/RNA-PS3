<?php
	switch($opcode){
		case 1201:{
			$gamename_raw = $packet_parser->ReadArray(16);
			$gamename = '';			
			
			for($i = 0; $i < 16; $i++)
				$gamename .= chr(intval((strlen($gamename_raw[$i]) < 2)?'0'.$gamename_raw[$i]:$gamename_raw[$i], 16));
		
			$gamename = trim($gamename);
				
			$query = mysql_query("SELECT id FROM rna_trophies WHERE appid='" . $gamename . "'");
			
			$packet = '01A20100';
			if(mysql_num_rows($query) > 0)
				$packet .= '01';
			else
				$packet .= '00';
				
			echo $packet;
			exit;
		}break;
		case 1202:{			
			$gamename_raw = $packet_parser->ReadArray(16);
			$gamename = '';			
			
			for($i = 0; $i < 16; $i++)
				$gamename .= chr(intval((strlen($gamename_raw[$i]) < 2)?'0'.$gamename_raw[$i]:$gamename_raw[$i], 16));
					
			$gamename = trim($gamename);	
				
			$max_trophies = $packet_parser->ReadByte();
							
			mysql_query("INSERT INTO rna_trophies (appid, max_num) VALUES ('" . $gamename . "', " . intval($max_trophies,16) . ")");
			list($gid) = mysql_fetch_row(mysql_query("SELECT id FROM rna_trophies WHERE appid='" . $gamename . "'"));
			
			for($i = 0; $i < intval($max_trophies,16); $i++){
				$id = $packet_parser->ReadByte();
				$unlocked = $packet_parser->ReadByte();
				$pl_unlocked = $packet_parser->ReadByte();
				
				mysql_query("INSERT INTO rna_trophies_ex (gid, inid) VALUES (" . $gid . ", " . intval($id, 16) . ")");
				if($unlocked == 0x1){
					list($tid) = mysql_fetch_row(mysql_query("SELECT id FROM rna_trophies_ex WHERE gid=" . $gid . " and inid=" . $id));
					mysql_query("INSERT INTO rna_user_trophies (gid, tid, uid, platinum_unlocked) VALUES (" . $gid . ", " . $tid . ", " . $uinfo['id'] . ", " . $pl_unlocked . ")");	
				}
			}
		}break;
		case 1203:{			
			$gamename_raw = $packet_parser->ReadArray(16);
			$gamename = '';			
			
			for($i = 0; $i < 16; $i++)
				$gamename .= chr(intval((strlen($gamename_raw[$i]) < 2)?'0'.$gamename_raw[$i]:$gamename_raw[$i], 16));
					
			$gamename = trim($gamename);
			
			$user_trophies = $packet_parser->ReadByte();
							
			list($gid) = mysql_fetch_row(mysql_query("SELECT id FROM rna_trophies WHERE appid='" . $gamename . "'"));
					
			for($i = 0; $i < intval($user_trophies,16); $i++){
				$id = intval($packet_parser->ReadByte(), 16);
				$unlocked = $packet_parser->ReadByte();
				$pl_unlocked = $packet_parser->ReadByte();				
							
				list($tid) = mysql_fetch_row(mysql_query("SELECT id FROM rna_trophies_ex WHERE gid=" . $gid . " and inid=" . $id));				
									
				$query = mysql_query("SELECT * FROM rna_user_trophies WHERE gid=" . $gid . " AND tid=" . $tid . " AND uid=" . $uinfo['id']);
				
				if(mysql_num_rows($query) <= 0)
					mysql_query("INSERT INTO rna_user_trophies (gid, tid, uid, platinum_unlocked) VALUES (" . $gid . ", " . $tid . ", " . $uinfo['id'] . ", " . $pl_unlocked . ")");
				else
					mysql_query("UDPATE rna_user_trophies set platinum_unlocked=". $pl_unlocked . " WHERE gid=" . $gid . " AND tid=" . $tid . " AND uid=" . $uinfo['id']);
					
			}
		}break;
		case 1204:{		
			$xml_data = '';
			foreach($_POST as $key => $value){
				if($key == 'info')
					$xml_data = $value . $xml_data;
				else
					$xml_data .= '&' . str_replace('_', ' ', $key) . '=' . $value;
			}	

			mysql_escape_string($xml_data);
			htmlspecialchars($xml_data);
			
			$xml = simplexml_load_string($xml_data);
			
			$appid = $xml->npcommid;
			$appid = trim($appid[0]);
			
			$appname = $xml->{'title-name'};
			$appname = str_replace("'", "\\'", trim($appname[0]));
			
			list($gid) = mysql_fetch_row( mysql_query("SELECT id FROM rna_trophies WHERE appid='" . $appid . "'") );
			mysql_query("UPDATE rna_trophies SET name='" . $appname . "' WHERE id=" . $gid);
			
			foreach($xml->trophy as $trophy){					
				$trophyid = intval($trophy['id']);
				$trophyhid = (($trophy['hidden']=='no')?0:1);
				$trophytype = (($trophy['ttype']=='P')?3:(($trophy['ttype']=='G')?2:(($trophy['ttype']=='S')?1:0)));
								
				$trophyname = $trophy->name;
				$trophyname = trim($trophyname[0]);
				
				$trophydetail = $trophy->detail;
				$trophydetail = trim($trophydetail[0]);
				
				mysql_query("UPDATE rna_trophies_ex SET `name`='" . str_replace("'", "\\'", $trophyname) . "', `desc`='" . str_replace("'", "\\'", $trophydetail) . "', `hidden`=" . $trophyhid . ", `type`=" . $trophytype . " WHERE `gid`=" . $gid . " AND `inid`=" . $trophyid);						
			}
		}break;
		default:
		break;
	}
?>