<?php
// $Id$

/**
 * Admin callbacks for AFO Favorite module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_favorite_admin_settings() {

	$options = node_get_types('names');

	$form['afo_favorite_node_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose Node content type'),
		'#options' => $options,
		'#default_value' => variable_get('afo_favorite_node_types', array()),
		'#description' => t('These content types will be available for favoriting.'),
	);

	/*
	$form['messages'] = array(
		'#type' => 'fieldset',
		'#title' => t('User Messages'),
		'#description' => t('The messages displayed to the user.'),
	);
	$form['messages']['afo_favorite_n'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose Node content type'),
		'#options' => $options,
		'#default_value' => variable_get('afo_favorite_node_types', array()),
		'#description' => t('These content types will be available for favoriting.'),
	);
	*/
	
	return system_settings_form($form);

}