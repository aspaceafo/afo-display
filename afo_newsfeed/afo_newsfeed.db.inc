<?php
// $Id$

/**
 * @file
 *
 * AFO Newsfeed db functions.
 */
 
 /**
 * Return item array for single event.
 */
function afo_newsfeed_get_event($event_id) {
	$sql = "SELECT ev.*, u.name FROM {afo_newsfeed_events} ev LEFT JOIN {users} u ON ev.uid=u.uid WHERE event_id = %d";
	$result = db_query_range(db_rewrite_sql($sql), $event_id, 0, 1);
	if ($event = db_fetch_array($result)) {
		return $event;
	}
	return NULL;
}

/**
 * Return newsfeed for multiple users.
 */
function afo_newsfeed_output($args=array()) {

	if(is_array($args['uids']) && count($args['uids']) === 0) return null;

	if(!$args['startrow']) $args['startrow'] = 0;
	if(!$args['numrows']) $args['numrows'] = 12;

	$query_args = array();

	// create query
	if(is_array($args['uids']) && count($args['uids']) > 0) {
		$where_uid = '(';
		foreach($args['uids'] as $uid) {
			$where_uid .= 'ev.uid=%d OR ';
			$query_args[] = $uid;
		}
		$where_uid = substr($where_uid, 0, -4).')';
	}	
	if($where_uid) $where = "WHERE $where_uid";
	else $where = '';
	
	$sql = "SELECT ev.*, u.name, node.nid as arg0_nid, node.title as arg0_title, node.type as arg0_type, files.filepath as arg0_filepath
	FROM {afo_newsfeed_events} ev 
	LEFT JOIN {users} u ON ev.uid=u.uid
	LEFT JOIN {node} node ON node.nid = ev.arg0
	LEFT JOIN {content_type_artwork} art ON art.nid = node.nid
	LEFT JOIN {files} files ON files.fid = art.field_artwork_image_fid
	$where 
	ORDER BY ev.timestamp DESC";

	// init arrays
	$newsfeed = array();
	$uids = array();
	
	// execute query
	$query = db_query_range($sql, $query_args, 0, 200);	
	while (count($newsfeed) < $args['numrows']) {
		$row = db_fetch_array($query);
		
		// only show latest events per uid per type 
		if(is_numeric($row['uid']) && (!isset($uids[$row['uid']][$row['type_id']]) || $args['show all']==true)) {

			// track this uid/type pair		
			$uids[$row['uid']][$row['type_id']] = true;

			$row['user_name'] = $row['name'];
			$newsfeed[] = $row;				
		}
		if(!$row) break;
	}

	return $newsfeed;
}

/**
 * Return newsfeed for individual user.
 */
function afo_newsfeed_output_ind($uid, $args=array()) {

	if(!$args['startrow']) $args['startrow'] = 0;
	if(!$args['startrow']) $args['numrows'] = 12;	

	if(is_numeric($uid)) {
		$user_name = db_result(db_query('SELECT name FROM {users} WHERE uid=%d', $uid));
	
		$sql = 'SELECT * FROM {afo_newsfeed_events} ev WHERE ev.uid = %d ORDER BY ev.timestamp DESC';
		$result = db_query_range(db_rewrite_sql($sql), $uid, $args['startrow'], $args['numrows']);
		while ($row = db_fetch_array($result)) {
			$row['user_name'] = $user_name;
			$newsfeed[] = $row;				
		}
		
		return $newsfeed;
	}
	
	return NULL;
}