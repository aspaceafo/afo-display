<?php
// $Id$

/**
 * Admin callbacks for AFO Newsfeed module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_newsfeed_admin_settings() {

	$content_types = node_get_types('names');

	$form['afo_newsfeed_register_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose content types to track'),
		'#options' => $content_types,
		'#default_value' => variable_get('afo_newsfeed_register_types', array()),
		'#description' => t('The creation and editing of these content types will be registered as newsfeed events.'),
	);

	return system_settings_form($form);
}

/**
 * Form builder for admin create event page.
 */
function afo_newsfeed_admin_create() {

	$item_types = afo_newsfeed_event_types();

	$form['type_id'] = array(
		'#type' => 'select',
		'#title' => t('Select event type'),
		'#options' => $item_types,
		'#required' => TRUE,
	);
	
	$query = db_query('SELECT name, uid FROM {users} ORDER BY name');
	while($row = db_fetch_object($query)) $all_users[$row->uid] = $row->name;
	
	$form['uid'] = array(
		'#type' => 'select',
		'#title' => 'User',
		'#options' => $all_users,
		'#required' => TRUE,
	);
	
	$form['timestamp'] = array(
		'#type' => 'textfield',
		'#title' => 'UNIX Timestamp',
		'#size' => 11,
		'#maxlength' => 11,
		'#required' => TRUE,
		'#default_value' => time(),
	);
	
	$form['arguments'] = array(
		'#type' => 'fieldset',
		'#title' => 'Arguments',
	);
	
	$form['arguments']['arg0'] = array(
		'#type' => 'textfield',
		'#title' => 'Argument 1',
		'#size' => 15,
		'#maxlength' => 255,
		'#required' => FALSE,
	);
	
	$form['arguments']['arg1'] = array(
		'#type' => 'textfield',
		'#title' => 'Argument 2',
		'#size' => 15,
		'#maxlength' => 255,
		'#required' => FALSE,
	);

	$form['arguments']['arg2'] = array(
		'#type' => 'textfield',
		'#title' => 'Argument 3',
		'#size' => 15,
		'#maxlength' => 255,
		'#required' => FALSE,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Create event',
	);
	
	return $form;
	
}
/**
 * Create event submit.
 */
function afo_newsfeed_admin_create_submit($form, $form_state) {

	$uid = $form_state['values']['uid'];
	$type_id = $form_state['values']['type_id'];
	$args = array(
		$form_state['values']['arg0'],
		$form_state['values']['arg1'],
		$form_state['values']['arg2']
	);
	$timestamp = $form_state['values']['timestamp'];

	$record = afo_newsfeed_record_event($uid, $type_id, $args, $timestamp);
	
	if($record) {
		drupal_set_message('New event successfully created.');
	}
}
	
	