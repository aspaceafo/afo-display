<?php
// $Id$

/**
 * Admin callbacks for AFO History module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_history_admin_settings() {

	$form['afo_history_block_total'] = array(
		'#type' => 'textfield',
		'#title' => t('How many thumbnails in Viewing History block?'),
		'#size' => 2,
		'#default_value' => variable_get('afo_history_block_total', 6),
	);

	$types = node_get_types('names');
	$form['afo_history_record_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose history content types'),
		'#options' => $types,
		'#default_value' => variable_get('afo_history_record_types', array()),
		'#description' => t('Nodes of these types will be recorded in the user\'s viewing history.'),
	);	

/*
	$roles = user_roles(true);
	$form['afo_history_record_roles'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose history user roles'),
		'#options' => $roles,
		'#default_value' => variable_get('afo_history_record_roles', array()),
		'#description' => t('Users with one of these roles will be recorded in the user\'s viewing history.'),
	);	
*/
	return system_settings_form($form);

}