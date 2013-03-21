<?php
// $Id$

/**
 * @file
 *
 * Compile and output browse page.
 */



/************************************************ DISPLAY */

/**
 * Main browsing page.
 */
function afo_browsenav2_browse_page() {

	afo_browsenav2_runinit();

	// determine params
	$page_args = func_get_args();
	if(count($page_args) === 0) {
		$page_args = explode('/', $_SESSION['afo_browsenav2_search_args']);
	}
	$params = afo_browsenav2_args_to_params($page_args);
	$page_args = implode('/', $page_args);
	$_SESSION['afo_browsenav2_search_args'] = $page_args;

	// SET op TO EXCLUSIVE no matter what -- INCLUSIVE TO SLOW!
	$params['op'][0] = 'exclusive';

	// send $params array to JS
	drupal_add_js(array(
		'afo_browsenav2' => array(
			'args' => $page_args,
			'params' => $params,
			'schema' => afo_browsenav2_parameter_schema(),
			'browsepage' => afo_browsenav2_get_browsepage(),
			)
		), 'setting');

	/**** Create widget ****/
	module_load_include('inc', 'afo_browsenav2', 'afo_browsenav2.widget');
	$widget = afo_browsenav2_buildwidget($params);
	$content['browse-widget'] = array(
		'#value' => afo_browsenav2_widget_print($widget, $params['start'][0]),
		'#weight' => -10,
	);
		
	/**** Get query ****/
	$pager_num = 0;
	$per_page = variable_get('afo_browsenav2_results_per_page', 100);
	$range = afo_backend_pager_calculate($per_page, $pager_num);
	$range = $range['start'] . ', ' . $per_page;
	module_load_include('inc', 'afo_browsenav2', 'afo_browsenav2.db');
	$q = afo_browsenav2_build_query_optimized($params, array('range'=>$range));
	
	/**** Output query result ****/
	if(is_array($q)) {

		if(user_access('administer content')) {
			//$per_page = 200;
			//drupal_add_js(drupal_get_path('module', 'afo_browsenav2').'/afo_browsenav2.admin.js');	
		}
		
		//$result = pager_query($q[0], $per_page, $pager_num, $q[2], $q[1]);
		$result = afo_backend_pager_query($q[0], $per_page, $pager_num, $q[2], $q[1], 'normal');

		//while($row = db_fetch_array($result)) {
		while($row = db_fetch_array($result['query'])) {
			// if only a single result item, remove from array
			if(count($row) == 1) {
				foreach($row as $r) $results[] = $r;
			} // otherwise include whole array
			else {
				$results[] = $row;
			}
		}
		
		if($params['type'][0] == 'open calls') {
			$results_content = theme('afo_display_table', $results, null, $params['type'][0], array());
		}	
		else {
			$results_content = theme('afo_display_grid', $results, null, $params['type'][0], array('expandable'=>true));
		}
		
		$content['results'] = array(
			'#value' => $results_content,
			'#weight' => 0,
		);

		/*
		global $pager_page_array;
		global $pager_total_items;
		$range_start = $pager_page_array[$pager_num] * $per_page;
		$range_end = $range_start + count($results);		
		$content['results-counter'] = array(
			'#prefix' => '<div class="browse-results-count">',
			'#value' => t('Showing %start - %end of %total', array('%start' => $range_start + 1, '%end' => $range_end, '%total' => $pager_total_items[$pager_num])),
			'#suffix' => '</div>',
			'#weight' => 1,
		);
		$content['pager'] = array(
			'#value' => theme('pager', null, $per_page, $pager_num),
			'#weight' => 2,
		);
		*/
		$content['pager'] = array(
			'#value' => $result['pager'],
			'#weight' => 2,
		);

	}
		

	/**** Return content ****/

	if($content) return drupal_render($content);
	else {
		drupal_set_message(t('No results were returned.'));
		return '';
	}
}

 
/************************************************ BROWSE OPTION functions */


/**
 * Alter parameter. Send $params array and $change e.g. array('op'=>'add', 'field'=>'tags', 'value'=>'1')
 */
function afo_browsenav2_params_alter(array $params, array $change) {

	$parameter_schema = afo_browsenav2_parameter_schema();

	switch($change['op']) {
	
		case 'add':
			// check if field array exists
			if(is_array($params[$change['field']])) {
				
				// check if field allows multiple values
				if($parameter_schema[$change['field']]['multiple']) {

					// check that value is not in field array
					if(!in_array($change['value'], $params[$change['field']])) {	
						// add value to field array
						$params[$change['field']][] = $change['value'];
					}
				}
				else {
					// replace value with new one
					$params[$change['field']] = array($change['value']);
				}
			}
			// field array doesn't exist
			else {
				// create field array with value
				$params[$change['field']] = array($change['value']);
			}
			
			break;
		
		case 'remove':
			// check if field array exists
			if(is_array($params[$change['field']])) {
				
				// check if value is in field array
				if(in_array($change['value'], $params[$change['field']])) {
				
					// run through array and unset any matching values
					foreach($params[$change['field']] as $key => $val) {
						if($val == $change['value']) {
				
							// remove value form field array
							unset($params[$change['field']][$key]);
						}
					}
				}
			}
			
			break;
	
	}

	return $params;
}



/************************************************ HELPER FUNCTIONS */

/**
 * Collapse link data into class for JS.
 */
function afo_browsenav2_link_to_class($type, $val) {
	$type = str_replace(' ', '_', $type);
	$val = str_replace(' ', '_', $val);

	return "{$type}Z{$val}";
}

/**
 * Get site domain.
 */
function afo_browsenav2_get_domain() {
	global $cookie_domain;
	return "http://local-artists.org";
}

/**
 * Get browsepage url.
 */
function afo_browsenav2_get_browsepage() {
	global $base_path;
	$domain = afo_browsenav2_get_domain();
	return $domain.$base_path.arg(0).'/';
} 