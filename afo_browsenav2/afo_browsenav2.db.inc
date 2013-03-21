<?php
// $Id$

/**
 * @file
 *
 * AFO Browsenav2 module: db query functions.
 */
 
/**
 * Construct query optimized.
 */
function afo_browsenav2_build_query_optimized(array $params, $options = array()) {

	$parameter_schema = afo_browsenav2_parameter_schema();
	
	$tags_used = 0;
	$tags_max = 5;
	$artwork_type = variable_get('afo_settings_types_artwork', null);
	$artwork_image_field = variable_get('afo_settings_artwork_imagefield', null);
	$artist_rid = variable_get('afo_custommenu_roles_artist', 100);
	$range = isset($options['range']) ? $options['range'] : '0,100';
	$limit_clause = "LIMIT $range";
	
	foreach($parameter_schema as $key => $schema) {
		// add default value to params if no value is present
		if(!is_array($params[$key]) && $schema['default']) {
			$params[$key] = array($schema['default']);
		}
	}
	
	$query = array();

	$query['count select'] = 'COUNT(*)';
	
	$op = $params['op'][0];
	$type = $params['type'][0];
	$sort = $params['sort'][0];
	$num_tags = is_array($params['tags']) ? count($params['tags']) : 0;
	$num_cities = is_array($params['city']) ? count($params['city']) : 0;
	$num_regions = is_array($params['regions']) ? count($params['regions']) : 0;

	// get region sub categories
	$add_terms = array();
	foreach($params['regions'] as $tid) {
		if(is_numeric($tid)) {
			$term_children = taxonomy_get_tree(2, $tid);
			foreach($term_children as $child) {
				$add_terms[] = $child->tid;
			}
		}
	}
	$regions_full = array_merge($params['regions'], $add_terms);
		
	if(is_numeric($params['city'][0])) { 
		$region_tid = $params['city'][0];
		$regions_full = array($params['city'][0]);
	}
	else if (is_numeric($params['regions'][0])) {
		$region_tid = $params['regions'][0];
	}
	else {
		$region_tid = null;
	}
	
	
	// --------------------------------------------- text search
	if($params['textsearch'][0]) {

		// get search type or default to names
		$search_type = strlen($params['textsearchtype'][0]) > 0 ? $params['textsearchtype'][0] : 'names';
		// get search term
		$search_term = check_plain($params['textsearch'][0]);

		switch($search_type) {
			case 'names':
				switch($params['type'][0]) {
					case 'artists':
						$query['from'] = '{users} users';
						//$query['joins'][] = 'LEFT JOIN {afo_popularity_rankings_user} popularity ON popularity.uid = users.uid';
						$query['where'][] = array("users.name LIKE '%%%s%%' AND ", $search_term);
						//$query['order'][] = 'popularity.ranking_score DESC';
						break;
					default:
						$query['from'] = '{node} node';
						$query['where'][] = array("users.name LIKE '%%%s%%' AND ", $search_term);
						$query['where'][] = 'node.status = 1 AND ';
						$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
						break;
				}			
		}
	}

	// --------------------------------------------- members search
	elseif($params['type'][0] == 'members') {
		$query['select'][] = 'users_roles.uid';
		$query['from'] = '{users_roles} users_roles';
		$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = users_roles.uid';
		$query['where'][] = array('users_roles.rid = %d', 7);
	}
	
	// --------------------------------------------- 1 tag, no city/region
	elseif($num_tags == 1 && !$region_tid) {
		switch($params['type'][0]) {
			case 'artists':
				$query['select'][] = 'terms.uid';
				$query['from'] = '{term_user} terms';
				$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = terms.uid';
				$query['where'][] = array('terms.tid = %d AND ', $params['tags'][0]);
				break;
			default:
				$query['select'][] = 'terms.nid';
				$query['from'] = '{term_node} terms';
				$query['joins'][] = 'INNER JOIN {node} node ON node.nid = terms.nid';
				$query['where'][] = array('terms.tid = %d AND ', $params['tags'][0]);
				$query['where'][] = 'node.status = 1 AND ';
				break;
		}
		switch($params['sort'][0]) {				
			case 'popular':
				$query['order'][] = 'terms.popularity DESC, terms.created DESC';
				break;
			case 'new':
				$query['order'][] = 'terms.created DESC';
				break;
		}	
	} // end 1 tag, no city/region

	// --------------------------------------------- 1 tag, + city/region (1 or multiple)
	elseif($num_tags == 1 && $region_tid) {
		switch($params['type'][0]) {
			case 'artists':
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_user.popularity DESC, term_user.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_user.created DESC';
						break;
				}	
				$query['from'] = '(SELECT term_user.uid FROM {term_user} term_user WHERE term_user.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause.') term_users';
				$query['joins'][] = 'LEFT JOIN {term_user} term_user ON term_user.uid = term_users.uid';
				$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = term_users.uid';

				if(count($regions_full) > 1) $query['where'][] = array('term_user.tid IN ('.implode(',', $regions_full).') AND '); 
				else $query['where'][] = array('term_user.tid = %d AND ', $region_tid);
				break;

			default:
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_node.popularity DESC, term_node.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_node.created DESC';
						break;
				}	
				$query['from'] = '(SELECT node.nid, node.uid FROM {term_node} term_node LEFT JOIN node node ON node.nid = term_node.nid WHERE term_node.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause.') nodes';
				$query['joins'][] = 'LEFT JOIN {term_user} term_user ON term_user.uid = nodes.uid';
				$query['joins'][] = 'LEFT JOIN {node} node ON node.nid = nodes.nid';
				$query['where'][] = 'node.status = 1 AND ';
				$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
				
				if(count($regions_full) > 1) $query['where'][] = array('term_user.tid IN ('.implode(',', $regions_full).') AND '); 
				else $query['where'][] = array('term_user.tid = %d AND ', $region_tid);
				
				break;				
		}
	} // end 1 tag, no city/region

	// --------------------------------------------- multiple tags, no city/region
	elseif ($num_tags > 1 && !$region_tid) {
		switch($params['type'][0]) {
			case 'artists':
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_user.popularity DESC, term_user.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_user.created DESC';
						break;
				}	
				$from_clause = 'SELECT term_user.uid FROM {term_user} term_user WHERE term_user.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause;
				for($k=1; $k<(count($params['tags'])-1); $k++) {
					$from_clause = 'SELECT term_user.uid FROM ('.$from_clause.') term_users LEFT JOIN {term_user} term_user ON term_user.uid = term_users.uid WHERE term_user.tid = '.$params['tags'][$k];
				}
				$query['from'] = "($from_clause) term_users";
				$query['joins'][] = 'LEFT JOIN {term_user} term_user ON term_user.uid = term_users.uid';
				$query['joins'][] = 'LEFT JOIN {users} users ON term_user.uid = users.uid';
				$query['where'][] = array('term_user.tid = %d AND ', $params['tags'][$k]);
				break;
			default:
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_node.popularity DESC, term_node.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_node.created DESC';
						break;
				}	
				$from_clause = 'SELECT term_node.nid FROM {term_node} term_node WHERE term_node.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause;
				for($k=1; $k<(count($params['tags'])-1); $k++) {
					$from_clause = 'SELECT term_node.nid FROM ('.$from_clause.') nodes LEFT JOIN {term_node} term_node ON term_node.nid = nodes.nid WHERE term_node.tid = '.$params['tags'][$k];
				}
				$query['from'] = "($from_clause) nodes";
				$query['joins'][] = 'LEFT JOIN {term_node} term_node ON term_node.nid = nodes.nid';
				$query['joins'][] = 'LEFT JOIN {node} node ON node.nid = nodes.nid';
				$query['where'][] = array('term_node.tid = %d AND ', $params['tags'][$k]);
				$query['where'][] = 'node.status = 1 AND ';
				$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
				break;
		}	
	} // end multiple tags, no city/region

	// --------------------------------------------- no tag, 1 city/region
	elseif($num_tags == 0 && count($regions_full) == 1) {
		switch($params['type'][0]) {
			case 'artists':
				$query['select'][] = 'terms.uid';
				$query['from'] = '{term_user} terms';
				$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = terms.uid';
				$query['where'][] = array('terms.tid = %d AND ', $region_tid);
				break;
			default:
				$query['select'][] = 'node.nid';
				$query['from'] = '{term_user} terms';
				$query['joins'][] = 'RIGHT JOIN {node} node ON node.uid = terms.uid';
				$query['where'][] = array('terms.tid = %d AND ', $region_tid);
				$query['where'][] = 'node.status = 1 AND ';
				$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
				break;
		}
		switch($params['sort'][0]) {				
			case 'popular':
				$query['order'][] = 'terms.popularity DESC, terms.created DESC';
				break;
			case 'new':
				$query['order'][] = 'terms.created DESC';
				break;
		}	
	} // end 1 tag, no city/region

	// --------------------------------------------- no tags, 2+ city/regions
	elseif ($num_tags == 0 && count($regions_full) > 1) {
		switch($params['type'][0]) {
			case 'artists':
				$query['from'] = "{term_user} term_user";
				$query['joins'][] = 'LEFT JOIN {users} users ON term_user.uid = users.uid';
				$query['where'][] = 'term_user.tid IN ('.implode(',', $regions_full).') AND ';
				$query['where'][] = 'users.thumbnail_nid > 0 AND ';
				switch($params['sort'][0]) {
					case 'popular':
						$query['order'][] = 'term_user.popularity DESC, term_user.created DESC';
						break;
					case 'new':
						$query['order'][] = 'term_user.created DESC';
						break;
				}	
				break;
			default:
				switch($sort) {				
					case 'popular':
						$query['select'][] = 'popularity.nid';
						$query['from'] = '{afo_popularity_rankings_node} popularity';
						$query['joins'][] = 'INNER JOIN {node} node ON node.nid = popularity.nid';
						$query['joins'][] = 'INNER JOIN {term_user} term_user ON term_user.uid = node.uid';
						$query['where'][] = 'term_user.tid IN ('.implode(',', $regions_full).') AND ';
						$query['where'][] = 'node.status = 1 AND ';
						$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
						$query['order'][] = 'popularity.ranking_score DESC';
						break;
					case 'new':
						$query['select'][] = 'node.nid';
						$query['from'] = '{node} node';
						$query['joins'][] = 'LEFT JOIN {term_user} term_user ON term_user.uid = node.uid';
						$query['where'][] = 'term_user.tid IN ('.implode(',', $regions_full).') AND ';
						$query['where'][] = 'node.status = 1 AND ';
						$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				
						$query['order'][] = 'node.created DESC';
						break;

				}
				break;
		}	
	} // no tags, 2+ city/regions
	
	// --------------------------------------------- multiple tags, 1+ city/region
	elseif ($num_tags > 1 && $regions_full) {
		switch($params['type'][0]) {
			case 'artists':
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_user.popularity DESC, term_user.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_user.created DESC';
						break;
				}	
				$from_clause = 'SELECT term_user.uid FROM {term_user} term_user WHERE term_user.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause;
				for($k=1; $k<(count($params['tags'])); $k++) {
					$from_clause = 'SELECT term_user.uid FROM ('.$from_clause.') term_users LEFT JOIN {term_user} term_user ON term_user.uid = term_users.uid WHERE term_user.tid = '.$params['tags'][$k];
				}
				$query['from'] = "($from_clause) term_users";
				$query['joins'][] = 'LEFT JOIN {term_user} term_user ON term_user.uid = term_users.uid';
				$query['joins'][] = 'LEFT JOIN {users} users ON term_user.uid = users.uid';
				$query['where'][] = 'term_user.tid IN ('.implode(',', $regions_full).') AND ';
				break;

			default:
				switch($params['sort'][0]) {				
					case 'popular':
						$sort_clause = 'term_node.popularity DESC, term_node.created DESC';
						break;
					case 'new':
						$sort_clause = 'term_node.created DESC';
						break;
				}	
				$from_clause = 'SELECT term_node.nid, node.uid FROM {term_node} term_node LEFT JOIN {node} node ON node.nid = term_node.nid WHERE term_node.tid = '.$params['tags'][0].' ORDER BY '.$sort_clause;
				for($k=1; $k<(count($params['tags'])); $k++) {
					$from_clause = 'SELECT term_node.nid, node.uid FROM ('.$from_clause.') nodes LEFT JOIN {term_node} term_node ON term_node.nid = nodes.nid LEFT JOIN {node} node ON node.nid = term_node.nid WHERE term_node.tid = '.$params['tags'][$k];
				}
				$query['from'] = "($from_clause) nodes";
				$query['joins'][] = 'INNER JOIN {term_user} term_user ON term_user.uid = nodes.uid';
				$query['joins'][] = 'INNER JOIN {node} node ON node.nid = nodes.nid';
				$query['where'][] = 'node.status = 1 AND ';
				$query['where'][] = array('node.type = \'%s\' AND ', $artwork_type);				

				if(count($regions_full) > 1) $query['where'][] = array('term_user.tid IN ('.implode(',', $regions_full).') AND '); 
				else $query['where'][] = array('term_user.tid = %d AND ', $region_tid);

				break;
		}	
	} // end multiple tags, 1 city/region
	
	// --------------------------------------------- no tags, no city/region
	elseif ($num_tags == 0 && $num_cities == 0 && $num_regions == 0) {
		switch($type) {
			case 'artists':
				// sorts for user
				switch($sort) {				
					case 'popular':
						$query['select'][] = 'popularity.uid';
						$query['from'] = '{afo_popularity_rankings_user} popularity';
						$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = popularity.uid';
						$query['order'][] = 'popularity.ranking_score DESC';
						break;
					case 'new':					
						$query['select'][] = 'users.uid';
						$query['from'] = '{users} users';
						$query['where'][] = array('users.status = %d AND ', 1);
						$query['where'][] = array('users.thumbnail_nid > %d AND ', 0);
						$query['order'][] = 'users.created DESC';
						break;
				}
				break;
			default:
				// sorts for node
				switch($sort) {				
					case 'popular':
						$query['select'][] = 'popularity.nid';
						$query['from'] = '{afo_popularity_rankings_node} popularity';
						$query['joins'][] = 'INNER JOIN {node} node ON node.nid = popularity.nid';
						$query['where'][] = 'node.status = 1 AND ';
						$query['order'][] = 'popularity.ranking_score DESC';
						break;
					case 'new':
						$query['select'][] = 'node.nid';
						$query['from'] = '(SELECT nid FROM {node} WHERE status = 1 AND type = \'artwork\' ORDER BY created DESC '.$limit_clause.') nodes';
						$query['joins'][] = 'INNER JOIN {node} node ON node.nid = nodes.nid';						
						$limit_clause = null; // make empty now that it is used
						break;
					case 'random':
						$query['select'][] = 'RAND() AS random, node.nid';
						$query['from'] = '{node} node';
						$query['order'][] = 'random ASC';				
						break;
				}
				
				$count_query = 'SELECT count(*) FROM node node WHERE node.type = \'artwork\' AND node.status = 1';
				break;
		}
	} // end no tags, no city/region
	else {
		drupal_set_message('No query found for your selection.', 'error');
		dsm($params);
		dsm($regions_full);
		return null;
	}
	
	// add parts necessary for all queries
	switch($params['type'][0]) {
		case 'members':
			$query['select'][] = 'users.uid, users.name, users.thumbnail_nid';			
			$query['select'][] = 'files.filepath AS image';
			$query['joins'][] = 'LEFT JOIN {content_type_'.$artwork_type.'} artwork ON artwork.nid = users.thumbnail_nid';
			$query['joins'][] = 'LEFT JOIN {files} files ON artwork.'.$artwork_image_field.'_fid = files.fid';
			if($limit_clause) $query['limit'] = $limit_clause;
			break;
		case 'artists':
			$query['select'][] = 'users.uid, users.name, users.thumbnail_nid';			
			$query['select'][] = 'files.filepath AS image';
			$query['joins'][] = 'LEFT JOIN {content_type_'.$artwork_type.'} artwork ON artwork.nid = users.thumbnail_nid';
			$query['joins'][] = 'LEFT JOIN {files} files ON artwork.'.$artwork_image_field.'_fid = files.fid';
			if($limit_clause) $query['limit'] = $limit_clause;
			break;
		default:
			$query['select'][] = 'node.nid, node.title, node.uid, node.type, artwork.field_artwork_video_value AS video';
			$query['select'][] = 'users.name';
			$query['select'][] = 'files.filepath AS image';
			$query['joins'][] = 'LEFT JOIN {users} users ON users.uid = node.uid';
			$query['joins'][] = 'LEFT JOIN {content_type_'.$artwork_type.'} artwork ON node.nid = artwork.nid';
			$query['joins'][] = 'LEFT JOIN {files} files ON artwork.'.$artwork_image_field.'_fid = files.fid';
			if($limit_clause) $query['limit'] = $limit_clause;
			break;
	}
	

	/*********** build query */
	$q = 'SELECT ';
	$q_count = 'SELECT ';
	$args = array();
	
	// normal
	if(is_array($query['select'])) {
		foreach($query['select'] as $select_item) {
			$q .= $select_item . ', ';
		}	
	}
	if(substr($q, -2)==', ') $q = substr($q, 0, -2) . ' ';
	
	// count query
	$q_count .= $query['count select'];
	$select_length = strlen($q);

	$q .= 'FROM ' . $query['from'] . ' ';
	
	if(is_array($query['joins'])) {
		foreach($query['joins'] as $join_item) {
			$q .= $join_item . ' ';
		}
	}
	
	if(is_array($query['where'])) {
		$q .= 'WHERE ';
		foreach($query['where'] as $where_item) {
			if(is_array($where_item)) {
				$q .= $where_item[0];
				if(isset($where_item[1])) $args[] = $where_item[1];
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
		foreach($query['order'] as $order_item) {
			$q .= $order_item . ', ';
		}
		if(substr($q, -2)==', ') $q = substr($q, 0, -2) . ' ';
	}
	
	if($query['limit']) {
		$q .= $query['limit'];
	}
	
	if($q) {
		// if exists, override automatic count query with custom version
		//if($count_query) {
		//	$q_count = $count_query;
		//	dsm($count_query);
		//}
		//dsm(array($q, $args, $q_count));
		return array($q, $args, $q_count);
	}
	
	else return NULL;

}