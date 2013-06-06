<?php
// $Id$

/**
 * @file
 *
 * Events functions
 */

/**
 * Assemble list of events
 */
function _afo_custommenu_events_list($options = array()) {
	$data = afo_custommenu_events_data($options['args']);
	if(!is_array($data)) return '';

	if(isset($options['title'])) $title = $options['title'];
	elseif(!$options['no title']) $title = t('Upcoming Events');
	
	if($options['theme'] == 'table') {
		$content = theme('afo_display_table', $data, $title, 'events');
	}
	else {
		foreach($data as $event) {
			$list[] = theme('afo_custommenu_event_listing', $event, $options['op']);
		}
		$content = theme('item_list', $list, $title, 'ul', array('class'=>'eventsListSmall'));	
	}
	
	return $content;
}

/**
 * Return rendered, paged list of events, with criteria options
 */
function afo_custommenu_events_filteredlist($options = array()) {

	if(!is_array($options['args'])) $options['args'] = array();

	if(!empty($_POST['js'])) {
		$ajax = true;
		$options['args'] = $_POST;
		$options['theme'] = 'table';
		$options['no title'] = true;
	}
	
	$events = afo_custommenu_events_list($options);
	
	if($events) {
		$content['events-list'] = array(
			'#value' => $events,
		);
	}
	else {
		if($options['empty as null']) return null;
		$content = array(
			'#value' => t('No events found.'),
		);
	}
	
	if($ajax) {
		drupal_json(
			array(
				'data' => drupal_render($content),
			)
		);
	}
	else return drupal_render($content);
}

/**
 * Get events data from db
 */
function afo_custommenu_events_data($args) {
	$event_type = variable_get('afo_settings_types_event', null);

	$date_field = variable_get('afo_settings_event_datefield', null);
	$date_field_2 = $date_field . '_value2';
	$date_field .= '_value';

	if(!$event_type || !$date_field) return null;


	/************************************* ASSEMBLE QUERY ARGS **/
	$q['select'][] = 'n.nid, n.title, n.uid';
	$q['select'][] = 'e.'.$date_field.' AS date';
	$q['select'][] = 'e.'.$date_field_2.' AS date_end';
	$q['count select'] = 'COUNT(n.nid) AS total';
	$q['from'] = '{node} n ';
	$q['joins'][] = 'LEFT JOIN {content_type_'.$event_type.'} e ON e.nid = n.nid ';
	
	// user
	if(!$args['uid']) {
		$q['select'][] = 'u.name AS username';
		$q['joins'][] = 'LEFT JOIN {users} u ON u.uid = n.uid';
	}
	
	// get city tid
	if(is_numeric($args['city'])) {
		$city_tid = $args['city'];
	}
	else if(strlen($args['city']) > 0) {
		$city_vid = variable_get('afo_profile_city_vid', null);
		$city_tid = db_result(db_query('SELECT tid FROM {term_data} WHERE vid = %d AND name LIKE \'%%%s%%\'', $city_vid, $args['city']));
		if(!$city_tid) return null;
	}
	// add city to query
	if(is_numeric($city_tid)) {
		$q['joins'][] = 'LEFT JOIN {term_node} citytermnode ON citytermnode.nid = n.nid';
		$q['where'][] = 'citytermnode.tid = %d AND ';
		$sql_args[] = $city_tid;
	}
	
	// where event type
	$q['where'][] = 'n.type = "%s" AND '; 
	$sql_args[] = $event_type;
		
	// date
	$date_column = 'e.' . $date_field_2;
	switch($args['date']) {
		case 'past':
			$q['where'][] = $date_column.' < %d AND '; 
			$sql_args[] = time();
			break;

		case '1 month':
			$q['where'][] = "$date_column > %d AND $date_column < %d"; 
			$sql_args[] = time();
			$sql_args[] = strtotime('+1 months');
			break;

		case '3 months':
			$q['where'][] = "$date_column > %d AND $date_column < %d"; 
			$sql_args[] = time();
			$sql_args[] = strtotime('+3 months');
			break;

		case '3-6 months':
			$q['where'][] = "$date_column > %d AND $date_column < %d"; 
			$sql_args[] = strtotime('+3 months');
			$sql_args[] = strtotime('+6 months');
			break;

		case '6 months':
			$q['where'][] = "$date_column > %d"; 
			$sql_args[] = strtotime('+6 months');
			break;
		
		default:
		// all upcoming
		$q['where'][] = $date_column.' > %d AND '; 
		$sql_args[] = time();
	}
	
	// add uid if in args
	if(is_numeric($args['uid'])) { 
		$q['where'][] = 'n.uid = %d AND '; 
		$sql_args[] = $args['uid']; 
	}
	
	// add city tid if in args
	if(is_numeric($args['citytid'])) { 
		$q['where'][] = 'citytermnode.tid = %d AND '; 
		$sql_args[] = $args['citytid']; 
	}
	
	// sort by date asc
	$q['order'][] = $date_column.' ASC';
	
	$query = afo_backend_buildquery($q);
	
	$pager_num = 1;
	$query = pager_query($query['sql'], 10, $pager_num, $query['count sql'], $sql_args);
	//$query = afo_backend_pager_query($query['sql'], 10, 1, $sql_args);

	while($row = db_fetch_object($query)) {
//	while($row = db_fetch_object($query['query'])) {
		$data[] = $row;
	}
	
	return $data;
}

/**
 * Theme event listing
 */
function theme_afo_custommenu_event_listing($event, $op = 'normal') {
	if(!is_object($event)) return null;
	
	$node_path = 'node/'.$event->nid;
	$content = '<div class="event '.$op.'">';
	
	switch($op) {
	
		default:
			$content .= theme('afo_display_node_thumbnail', $event->nid, 'artwork_tiny', $node_path);
			$content .= l($event->title, $node_path) . ' ';
			$content .= '<br /><span class="secondaryText">';
			
			$date = format_date($event->date, 'custom', 'j M') . ' ';
			$date_end = format_date($event->date_end, 'custom', 'j M') . ' ';
			if($date_end != $date) $date .= ' - ' . $date_end;
			$content .= $date;
			
			if($event->city) $content .=  l($event->city, 'city/'.$event->citytid) . ' ';
			if($event->username) $content .= '<br />' . t('Posted by ') . l($event->username, 'user/'.$event->uid) . ' ';
			$content .= '</span> ';
			break;
	
	}
	
	$content .= '</div>';

	return $content;
}

/**
 * Events calendar page
 */
function afo_custommenu_eventspage() {
	$options = array('display op'=>'normal', 'theme' => 'table', 'no title' => true, 'args' => $args);

	// valid GET arguments
	$valid_get = array('medium', 'tag', 'city');
	foreach($valid_get as $k) {
		if(is_numeric($_GET[$k])) $args[$k] = $_GET[$k];
	}
	
	$content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => -1,
		'#width' => '29%',		
	);
	
	$content['col-left']['filter-events'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Filter Events'),
		'#value' => drupal_get_form('afo_custommenu_eventspage_buildfilters', $args),
		'#weight' => 0,
	);


	$content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '70%',
	);
	
	$listing = afo_custommenu_events_filteredlist($options);
	$content['col-right']['events-listing'] = array(
		'#prefix' => '<div style="display:none" id="filters-throbber"throbber">Loading...</div><div id="afo-events-results">',
		'#value' => $listing,
		'#suffix' => '</div>',
		'#weight' => 5,
	);

	drupal_add_js(drupal_get_path('module', 'afo_backend').'/filter.js');

	return drupal_render($content);
}

/**
 * Build open call filters
 */
function afo_custommenu_eventspage_buildfilters(&$form_state, $args) {

	$form['#tree'] = true;
	
	$form['enable-filters'] = array(
		'#type' => 'hidden',
		'#value' => 1,
	);
	$form['filters-results'] = array(
		'#type' => 'hidden',
		'#value' => '#afo-events-results',
	);
	$form['filters-ajax-url'] = array(
		'#type' => 'hidden',
		'#value' => 'events/ajax',
	);
	$form['submit-type'] = array(
		'#type' => 'hidden',
		'#value' => 'button',
	);

	$city_vid = variable_get('afo_profile_city_vid', null);
	$form['event-filters']['city'] = array(
		'#type' => 'textfield',
		'#title' => t('City'),
		'#autocomplete_path' => 'taxonomy/autocomplete/'.$city_vid,
		'#maxlength' => 1024,
		'#size' => 20,
		'#element_validate' => array('afo_backend_city_validate'),
		'#default_value' => $args['city'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);

	$date_options = array(
		'upcoming' => t('All upcoming'),
		'1 month' => t('Within next month'),
		'3 months' => t('Next 3 months'),
		'3-6 months' => t('3 - 6 months'),
		'6 months' => t('6+ months'),
		'past' => t('past'),
	);
	$form['event-filters']['date'] = array(
		'#type' => 'select',
		'#title' => t('Date'),
		'#options' => $date_options,
		'#default_value' => $args['date'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);

	$form['spacer'] = array(
		'#value' => '<div style="width:65px; float: left">&nbsp;</div>',
		'#weight' => 7,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Search'),
		'#attributes' => array(
//			'class' => 'js-hide',
		),
		'#weight' => 8,
	);

	$form['reset'] = array(
		'#type' => 'markup',
		'#value' => l(t('reset'), 'calls', array('attributes'=>array('class'=>'reset-filters'))),
		'#weight' => 9,
	);

	return $form;
	
}