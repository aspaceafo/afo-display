<?php
// $Id$

/**
 * @file
 *
 * Build Query functions
 *
 * $query['select'][] =
 * $query['from'] =
 * $query['joins'][] =
 * $query['where'][] = '... AND ' / '... OR '
 * $query['order'][] =
 *
 * returns:
 * array('sql' => $query, 'count sql' => $count_query);
 */
function _afo_backend_buildquery($query) {
	if(!is_array($query)) return null;

	/*********** build query */
	$q = 'SELECT ';
	$q_count = 'SELECT ';
	$args = array();
	
	// normal
	$q .= implode(', ', $query['select']);
	$q .= ' ';
	
	// count query
	$q_count .= $query['count select'];
	$select_length = strlen($q);

	$q .= 'FROM ' . $query['from'] . ' ';
	
	if(is_array($query['joins'])) {
		$q .= implode(' ', $query['joins']);
		$q .= ' ';
	}
	
	if(is_array($query['where'])) {
		$q .= 'WHERE ';
		foreach($query['where'] as $where_item) {
			if(is_array($where_item)) {
				$q .= $where_item[0];
				$args[] = $where_item[1];
			}
			else {
				if(substr($where_item, 0, 1) == ')') {
					if(substr($q, -5) ==' AND ') $q = substr($q, 0, -5);
					elseif(substr($q, -4) ==' OR ') $q = substr($q, 0, -4);
				}
				$q .= $where_item;
			}
		}
		if(substr($q, -2)==', ') $q = substr($q, 0, -2) . ' ';
		elseif(substr($q, -5)==' AND ') $q = substr($q, 0, -5);
		elseif(substr($q, -4)==' OR ') $q = substr($q, 0, -4);
	}
	
	if(substr($q, -1)!=' ') $q .= ' ';

	// add rest of query to count query, without order by clause
	$q_count .= ' '.substr($q, $select_length);

	if(is_array($query['order'])) {
		$q .= 'ORDER BY ';
		$q .= implode(', ', $query['order']);
		$q .= ' ';
	}
	
	if($q) {
		$query = array(
			'sql' => $q,
			'count sql' => $q_count,
		);
		//dsm($query);
		return $query;
	}
	
	else return NULL;

}