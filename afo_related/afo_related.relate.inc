<?php
// $Id$

/**
 * get relatives
 */
function afo_related_get_relatives($val, $op) {

	$relatives = array();

	switch($op) {
		
		case 'artwork':
			afo_related_match_terms($val, $relatives);
			break;
			
		case 'user':
			afo_related_match_userterms($val, $relatives);
			break;
			
	}
	
	if(count($relatives) > 0) {
		afo_related_sort_relatives($relatives);
		afo_related_limit_relatives($relatives);
		afo_related_deassoc_relatives($relatives);
		$selected = afo_related_choose_relatives($relatives);
		
		if(is_array($selected)) return $selected;
	}
	
	return false;

}

/**
 * match taxonomy terms
 */
function afo_related_match_terms($target_nid, &$relatives, $raise_by=1) {

	$query = db_query('SELECT tid FROM {term_node} WHERE nid = %d', $target_nid);
	while($result = db_fetch_array($query)) {
		$target_terms[] = $result['tid'];
	}

	if(is_array($target_terms)) {
		foreach($target_terms as $tid) {
			$query = db_query("
			SELECT tn.nid 
			FROM {term_node} tn 
			LEFT JOIN {node} n ON n.nid=tn.nid 
			WHERE tn.tid = %d AND tn.nid <> %d AND n.type = '%s' 
			", $tid, $target_nid, 'artwork');
			while($result = db_fetch_array($query)) {
				if(!isset($relatives[$result['nid']])) $relatives[$result['nid']] = 0;
				$relatives[$result['nid']] += $raise_by;
			}
		}
	}
}

/**
 * match taxonomy terms for users
 */
function afo_related_match_userterms($target_uid, &$relatives, $raise_by=1) {

	$query = db_query('SELECT tid FROM {term_user} WHERE uid = %d', $target_uid);
	while($result = db_fetch_array($query)) {
		$target_terms[] = $result['tid'];
	}

	if(is_array($target_terms)) {
		foreach($target_terms as $tid) {
			$query = db_query("
			SELECT tu.uid 
			FROM {term_user} tu 
			WHERE tu.tid = %d AND tu.uid <> %d 
			", $tid, $target_uid);
			while($result = db_fetch_array($query)) {
				$artwork_count = db_result(db_query('SELECT COUNT(nid) AS artworks FROM {node} WHERE uid = %d AND status = 1 AND type = \'%s\'', $result['uid'], 'artwork'));
				if($artwork_count > 0) {
					if(!isset($relatives[$result['uid']])) $relatives[$result['uid']] = 0;
					$relatives[$result['uid']] += $raise_by;
				}
			}
		}
	}
}

/**
 * sort relatives by score
 */
function afo_related_sort_relatives(&$relatives) {
	arsort($relatives, SORT_NUMERIC);
}

/**
 * limit relatives to maximum
 */
function afo_related_limit_relatives(&$relatives, $max=30) {
	if(count($relatives) > $max) {
		$relatives = array_slice($relatives, 0, $max, true);	
	}
}

/**
 * turn relatives array into normal (non-associative) array
 */
function afo_related_deassoc_relatives(&$relatives) {
	$relatives = array_keys($relatives);
	//sort($relatives);
}

/**
 * randomly choose set of relatives
 */
function afo_related_choose_relatives($relatives, $total=null) {
	
	if(!$total) $total = variable_get('afo_related_block_total', 4);
	
	$selected = array();

	if(count($relatives) <= $total) {
//		foreach($relatives as $val => $score) $selected[] = $val;
		return $relatives;
	}

	for ($j=0; $j<$total; $j++){
		$key = mt_rand(0, count($relatives) - 1);
		$selected[] = $relatives[$key];
		unset($relatives[$key]);
		sort($relatives);
	}
	
	return $selected;
	
}
