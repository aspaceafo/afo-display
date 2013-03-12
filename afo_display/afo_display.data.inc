<?php
// $Id$

/**
 * @file
 *
 * Data functions for AFO Display module.
 */
 

/******************************************** db FUNCTIONS **/

/**
 * get node data from db
 */
function afo_display_data_get_slides($data, $op='node') {
	if(!is_array($data)) return null;

	switch($op) {
		case 'artwork':
			$key_field = 'nid';
			$artwork_type = variable_get('afo_settings_types_artwork', null);
			$artwork_image_field = variable_get('afo_settings_artwork_imagefield', null);

			$query = 'SELECT n.nid, n.title, n.uid, n.type,
			u.name, files.filepath AS image,
			details.*
			FROM {node} n
			LEFT JOIN {users} u ON n.uid = u.uid
			LEFT JOIN {afo_portfolio_artwork_data} details ON n.nid = details.nid
			LEFT JOIN {content_type_'.$artwork_type.'} artwork ON n.nid = artwork.nid
			LEFT JOIN {files} files ON artwork.'.$artwork_image_field.'_fid = files.fid
			WHERE n.nid IN ('.implode(',', $data).')
			';
			break;
			
		case 'artists':
			$key_field = 'uid';
			$artwork_type = variable_get('afo_settings_types_artwork', null);
			$artwork_image_field = variable_get('afo_settings_artwork_imagefield', null);

			$query = 'SELECT u.uid, u.name, u.thumbnail_nid, files.filepath AS image
			FROM {users} u
			LEFT JOIN {content_type_'.$artwork_type.'} artwork ON artwork.nid = u.thumbnail_nid
			LEFT JOIN {files} files ON artwork.'.$artwork_image_field.'_fid = files.fid
			WHERE u.uid IN ('.implode(',', $data).') AND u.thumbnail_nid > 0
			';
			break;
	
		case 'exhibitions':
			$key_field = 'nid';
			$query = 'SELECT n.nid, n.title, n.uid, n.type, u.name
			FROM {node} n
			LEFT JOIN {users} u ON n.uid = u.uid
			WHERE n.nid IN ('.implode(',', $data).')
			';
			break;
	
		case 'node':
			$key_field = 'nid';
			$query = 'SELECT n.nid, n.title, n.uid, n.type, u.name
			FROM {node} n
			LEFT JOIN {users} u ON n.uid = u.uid
			WHERE n.nid IN ('.implode(',', $data).')
			';
			break;
	}
	
	if(!$query) return null;

	$nodes = array();	
	$q = db_query($query);
	while($result = db_fetch_array($q)) {
		$result_data[$result[$key_field]] = $result;
	}

	// reorder based on original array
	foreach($data as $id) {
		$return_data[] = $result_data[$id];
	}
	
	return $return_data;
}