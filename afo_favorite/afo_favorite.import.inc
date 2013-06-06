<?php
// $Id$

/**
 * @file
 *
 * Import lightbox to favorites functions
 */
 
/**
 * Import user's lightbox
 */
function afo_favorite_lb_import_get($uid, $a_id = null) {
	if(!is_numeric($uid)) return null;
	
	if(!$a_id) $a_id = afo_import_get_a_id($uid);
	
	if(!is_numeric($a_id)) {
		drupal_set_message('Error: Could not find Artists File ID for user uid #'.$uid, 'error');
		return null;
	}
	
	$q = "SELECT artistid, foldername FROM lightbox WHERE uid = $a_id AND artistid > 0 ORDER BY foldername ASC";
	$lb_data = afo_import_getExternalData($q);
	
	if(!is_array($lb_data)) {
		drupal_set_message('Error: Could not find any lightbox records for artist user id #'.$uid, 'error');
		return null;
	}
	else {
		return $lb_data;
	}
}

/**
 * Display lightbox list
 */
function afo_favorite_lb_list($lb_data) {
	if(!is_array($lb_data)) return null;
	
	global $base_path;
		
	foreach($lb_data as $lb_artist) {
	
		if($lb_artist['foldername'] != $current_folder) {
			$lb_list[] = array(
				'type' => 'label',
				'label' => $lb_artist['foldername'],
			);
			$current_folder = $lb_artist['foldername'];
		}
		
		$a_id = $lb_artist['artistid'];
		if(is_numeric($a_id) && !isset($shown[$current_folder][$a_id])) {
			$q = db_query('SELECT users.name, users.uid, node.title, files.filepath, users.thumbnail_nid
			FROM {afo_import_record_users} imp 
			INNER JOIN {users} users ON users.uid = imp.uid
			LEFT JOIN {node} node ON node.nid = users.thumbnail_nid
			LEFT JOIN {content_type_artwork} art ON art.nid = users.thumbnail_nid
			LEFT JOIN {files} files ON files.fid = art.field_artwork_image_fid
			WHERE imp.source_id = %d', $a_id); 
			
			if($result = db_fetch_array($q)) {
				$lb_list[] = $result;
			}
			$shown[$current_folder][$a_id] = true;
		}
	}
	
	return $lb_list;
}