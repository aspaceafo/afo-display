<?php
// $Id$

/**
 * @file
 *
 * City page functions.
 */

/**
 * Build city page.
 */
function afo_custommenu_citypage($city) {

	global $base_path;

	if(is_numeric($city)) {
		$city = taxonomy_get_term($city);
	} else {
		$city = taxonomy_get_term_by_name($city);
		if(count($city) == 1) $city = $city[0];
		elseif(count($city) > 1) {
			// if multiple terms found, create list with links to proper pages:
			foreach($city as $option) {
				$cities[] = '<a href="{$base_path}city/'.$option->tid.'">'.$option->name.'</a>';
			}
			$content .= t('Multiple cities found, please choose the right term:');
			$content .= theme('item_list', $cities);
			return $content;
		}
	}
	
	if(!is_object($city)) return t('Unable to find city data.');
	
	$content['title'] = array(
		'#prefix' => '<h1 class="title">',
		'#value' => t('City » ') . $city->name,
		'#suffix' => '</h1>',
		'#weight' => -10,
	);
	
	$content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#width' => '50%',
		'#weight' => -1,
	);
	$content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '50%',
		'#weight' => 0,
	);

	if($city->description) {
		$content['col-right']['term-description'] = array(
			'#prefix' => '<div class="secondaryText">',
			'#value' => $city->description,
			'#suffix' => '</div>',
			'#weight' => -9,
		);
	}
	
	/** popular artists **/
	if(module_exists('afo_browsenav2')) {
		$search_args = 'type=artists/op=inclusive/sort=popular/city='.$city->tid;
		$content['col-left']['popular-artists'] = array(
			'#type' => 'afo_contentbox',
			'#title' => t('Popular Artists'),
			'#value' => afo_browsenav2_embed_results($search_args),
			'#weight' => 0,
		);
	}
	
	/** upcoming events **/
	$events = afo_custommenu_events_list(array('args'=>array('city'=>$city->tid), 'no label'=>true));
	if($events) {
		$content['col-right']['upcoming-events'] = array(
			'#type' => 'afo_contentbox',
			'#value' => $events,
			'#weight' => 0,
		);
	}
	
	/** open calls **/
	$listing = afo_opencall_list_calls(array('display op'=>'short', 'theme' => 'list', 'empty as null' => true, 'args' => array('city'=>$city->tid)));
	if($listing) {
		$content['col-right']['opencalls'] = array(
			'#type' => 'afo_contentbox',
			'#title' => t('Open Calls'),
			'#value' => $listing,
			'#weight' => 1,
		);	
	}
	
	/** recent activity **/
	if(module_exists('afo_newsfeed')) {
		$query = db_query('SELECT t.uid FROM {term_user} t WHERE t.tid = %d', $city->tid);
		while($row = db_fetch_object($query)) $uids[] = $row->uid;
		if(is_array($uids)) {
			$newsfeed = afo_newsfeed_get(array('uids'=>$uids, 'numrows'=>20, 'themed'=>true));
			
			if($newsfeed) {
				$content['col-right']['recent-activity'] = array(
					'#title' => t('Recent Activity'),
					'#type' => 'afo_contentbox',
					'#value' => $newsfeed,
					'#weight' => 2,
				);
			}
		}
	}
	
	return drupal_render($content);
}

/**
 * Build city list page
 */
function afo_custommenu_citylist() {

	global $base_path;
	
	$city_vid = variable_get('afo_profile_city_vid', null);
	if(!$city_vid) return null;
	
	$query = db_query('SELECT term_data.tid, term_data.name, total.total
	FROM {term_data} term_data
	LEFT JOIN {afo_portfolio_term_user_total} total ON total.tid = term_data.tid
	WHERE term_data.vid = %d
	ORDER BY term_data.weight, term_data.name
	', $city_vid);

	while($term = db_fetch_object($query)) {
		if(strlen($term->name) > 0) {
			//$countUsers = db_result(db_query('SELECT COUNT(uid) AS total FROM {term_user} WHERE tid = %d', $term->tid));
			$city_links[] = array(
				'category' => strtoupper(substr($term->name, 0, 1)),
				'link' => '<a href="'.$base_path.'city/'.$term->tid.'">'.$term->name.'</a>',
				'count' => $term->total,
				'count term' => 'users',
				'weight' => $term->total,
			);
		}
	}
	
	/*
	foreach($city_terms as $term) {
		if(strlen($term->name) > 0) {
			$first_letter = strtoupper(substr($term->name, 0, 1));		
			$countUsers = db_result(db_query('SELECT COUNT(uid) AS total FROM {term_user} WHERE tid = %d', $term->tid));
			$city_links[$first_letter][] = l($term->name, 'city/'.$term->tid)." <span class='secondaryText'>($countUsers ".t('users').')</span>';
		}
	}
	*/
	
	$index_options = array(
		'#autocomplete' => 'taxonomy/autocomplete/'.$city_vid,
		'#submit' => 'afo_custommenu_citysearch_submit',
	);
	$content = theme('afo_index', $city_links, $index_options);
	return $content;

}

/**
 * Form submit for city text search
 */
function _afo_custommenu_citysearch_formsubmit($form, &$form_state) {
	$city_name = $form_state['values']['indexsearch'];
	$city_terms = taxonomy_get_term_by_name($city_name);
	if(is_numeric($city_terms[0]->tid)) drupal_goto('city/'.$city_terms[0]->tid);
	else drupal_set_message(t('Unable to find city page for "%city".', array('%city'=>$city_name)), 'error'); 
}