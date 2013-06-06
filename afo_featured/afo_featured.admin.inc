<?php
// $Id$

/**
 * Admin callbacks for AFO Favorite module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_featured_admin_settings() {

	$options = node_get_types('names');

	$form['afo_featured_node_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Choose Node content types'),
		'#options' => $options,
		'#default_value' => variable_get('afo_featured_node_types', array()),
		'#description' => t('These content types will be available to be Featured.'),
	);

	$presets = imagecache_presets();
	$preset_names[''] = t('<none>');
	foreach($presets as $p) {
		$preset_names[$p['presetname']] = $p['presetname'];
	}

	$form['afo_featured_slideshow_imagecachepreset'] = array(
		'#type' => 'select',
		'#title' => t('Slide show imagecache preset'),
		'#options' => $preset_names,
		'#default_value' => variable_get('afo_featured_slideshow_imagecachepreset', ''),
		'#description' => t('This imagecache preset will be used for featured slide shows.'),
	);
	
	return system_settings_form($form);

}

