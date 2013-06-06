<?php
// $Id$

/**
 * Admin callbacks for AFO Back End module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_backend_admin_settings() {

	/*
	$page_args = array(
		'type=artwork',
		'op=exclusive',
		'sort=popular',
		'tags=270',
	);

	$params = afo_browsenav2_args_to_params($page_args);

	module_load_include('inc', 'afo_browsenav2', 'afo_browsenav2.db');
	//$q = afo_browsenav2_build_query_optimized($params);
	$q = afo_browsenav2_build_query($params);
	
	dsm($q);

	try_query($q);
	*/
	
	$form['query-test'] = array(
	);
	
	return system_settings_form($form);
}

function try_query($q) {

	/**** Output query result ****/
	if(is_array($q)) {
		$pager_num = 0;
		$per_page = 30;
		
		if(true) {
	//		$result = db_query($q[0], $q[1]);
			$result = db_query_range($q[0], $q[1], 0, $per_page);
	//		$result = pager_query($q[0], $per_page, $pager_num, $q[2], $q[1]);
			while($row = db_fetch_array($result)) {
				$data[] = $row;
			}
		}
	}
	
	dsm($data);
}