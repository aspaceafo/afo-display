<?php
// $Id$

/**
 * Admin callbacks for AFO Exhibition module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_exhibition_admin_settings() {

	$types = node_get_types('names');
	$options[''] = '-- none --';
	$options = array_merge($options, $types);

	$form['afo_exhibition_content_type'] = array(
		'#type' => 'select',
		'#title' => t('Choose Exhibition content type'),
		'#options' => $options,
		'#default_value' => variable_get('afo_exhibition_content_type', ''),
		'#description' => t('This content type will be used as the Exhibition. The module will extend its functionality.'),
	);

	$form['afo_exhibition_item_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose item content types'),
		'#options' => $types,
		'#default_value' => variable_get('afo_exhibition_item_types', array()),
		'#description' => t('Select the content types that can be attached to exhibitions.'),
	);

	return system_settings_form($form);

}