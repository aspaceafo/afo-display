<?php
// $Id$

/**
 * Admin callbacks for AFO Display module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_display_admin_settings() {

	$presets = imagecache_presets();
	$preset_names[''] = t('<none>');
	foreach($presets as $p) {
		$preset_names[$p['presetname']] = $p['presetname'];
	}

	$form['thumbnails'] = array(
		'#type' => 'fieldset',
		'#title' => t('Thumbnails'),
	);
	$form['thumbnails']['afo_display_thumbnail_default_imagecache'] = array(
		'#type' => 'select',
		'#title' => t('Default imagecache'),
		'#options' => $preset_names,
		'#default_value' => variable_get('afo_display_thumbnail_default_imagecache', ''),
		'#description' => t('This imagecache preset will be used for node and user thumbnails, unless specified otherwise.'),
	);

	$form['dates'] = array(
		'#type' => 'fieldset',
		'#title' => t('Dates'),
	);
	$form['dates']['afo_display_dateformats_short'] = array(
		'#type' => 'textfield',
		'#title' => t('Date format - short'),
		'#size' => 15,
		'#default_value' => variable_get('afo_display_dateformats_short', ''),
	);
	
	return system_settings_form($form);
}