<?php
// $Id$

/**
 * Admin callbacks for AFO Flag module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_flag_admin_settings() {

	$options = node_get_types('names');

	$form['afo_flag_node_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Flag content types'),
		'#options' => $options,
		'#default_value' => variable_get('afo_flag_node_types', array()),
		'#description' => t('These content types can be flagged.'),
	);
	
	return system_settings_form($form);

}

/**
 * Editor: List flagged content.
 */
function afo_flag_list() {

	$form['#tree'] = true;

	$list = afo_flag_getlist();
	foreach($list as $flag) {
		
		$flag_row = array();
		
		$flag_row['flag_id'] = array(
			'#type' => 'hidden',
			'#value' => $flag['flag_id'],
		);

		$flag_row['response'] = array(
			'#type' => 'checkbox',
			'#options' => array(1=>''),
			'#default_value' => $flag['response'],
		);

		$flag_row['title'] = array(
			'#value' => l(substr($flag['title'], 0, 50), 'node/'.$flag['nid']),
		);

		$flag_row['name'] = array(
			'#value' => l($flag['name'], 'user/'.$flag['uid']),
		);
		
		$flag_row['date'] = array(
			'#value' => date('n/j/y', $flag['updated']),
		);
		
		// add row to form array
		$form['flags'][$flag['flag_id']] = $flag_row;
	}
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}
/**
 * Theme flag list form.
 */
function theme_afo_flag_list($form) {

	$rows = array();

	foreach (element_children($form['flags']) as $key) {

		$element = &$form['flags'][$key];

		$row = array();

		// response check box
		$row[] = drupal_render($element['response']);

		// flag_id and title
		$row[] = drupal_render($element['flag_id']) . drupal_render($element['title']);

		// user name
		$row[] = drupal_render($element['name']);

		// user name
		$row[] = drupal_render($element['date']);

		// put row
		$rows[] = array(
			'data' => $row,
		);
	}
	
	if (count($rows) == 0) {
		drupal_set_message(t('No flags found.'));
	}
	
	$header = array(t('OK'), t('Flagged content title'), t('Flagged by'), t('Date'));
	$output = theme('table', $header, $rows);
	$output .= drupal_render($form);

	return $output;
}

/**
 * Get all flags and related data.
 */
function afo_flag_getlist($flag_type = 0) {

	$query = db_query('
		SELECT flag.*, n.nid, n.title, u.name
		FROM {afo_flag_flags} flag
		LEFT JOIN {users} u ON flag.uid = u.uid
		LEFT JOIN {node} n ON flag.val = n.nid
		WHERE flag.flag_type = %d
		ORDER BY flag.updated DESC
	', $flag_type);
	
	while($row = db_fetch_array($query)) $rows[] = $row;	

	return $rows;
	
}