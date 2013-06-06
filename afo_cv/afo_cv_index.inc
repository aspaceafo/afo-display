<?php
// $Id$

/**
 * @file
 *
 * CV index functions.
 */

/**
 * Index page builder.
 */
function afo_cv_indexpage($searchtype='all', $op='normal') {

	$types = afo_cv_entrytypes();

	$data = afo_cv_get_venuelist($searchtype);
	
	if(is_array($data)) {
		$options = array(
			'#autocomplete' => 'afo-cv/autocomplete/venue',
			'#submit' => 'afo_cv_index_formsubmit',
			'#no cloud' => true,
		);
		
		$output = theme('afo_index', $data, $options);
		
		if($op=='overlay') print $output;
		else return $output;
	}
	
	return '';
}

/**
 * Return list of CV venues.
 */
function afo_cv_get_venuelist($searchtype='all') {
	if(is_string($searchtype)) {
		$searches['schools'] = 2;

		// gallery search
		if($searchtype=='galleries') {	
			$query = db_query('SELECT DISTINCT venues.venueid, venues.*
			FROM {afo_cv_venues} venues
			INNER JOIN {afo_cv_entries} entries ON venues.venueid = entries.venueid
			WHERE entries.typeid = 0 OR entries.typeid = 1
			ORDER BY venues.name ASC'); 
		}
		// other searches
		elseif($searches[$searchtype]) {
			$query = db_query('SELECT DISTINCT venues.venueid, venues.*
			FROM {afo_cv_venues} venues
			INNER JOIN {afo_cv_entries} entries ON venues.venueid = entries.venueid
			WHERE entries.typeid = %d
			ORDER BY venues.name ASC'
			, $searches[$searchtype]); 
		}
	
		// show all
		else {
			$query = db_query('SELECT venues.* FROM {afo_cv_venues} venues ORDER BY venues.name ASC'); 
		}

		while($row = db_fetch_array($query)) {
			$data[] = array(
				'link' => theme('afo_cv_create_link', $row['name'], $searchtype),
				'category' => substr($row['name'], 0, 1),
			);
		}

	}	
	return $data;
}