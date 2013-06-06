<?php
// $Id$

/**
 * Admin callbacks for AFO Related module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_related_admin_settings() {

	$form['afo_related_block_total'] = array(
		'#type' => 'textfield',
		'#title' => t('How many thumbnails in Related block?'),
		'#size' => 2,
		'#default_value' => variable_get('afo_related_block_total', 6),
	);
	
	return system_settings_form($form);
}