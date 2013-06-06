<?php
// $Id$

/**
 * Admin callbacks for AFO Profile module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_profile_admin_settings() {

	$vids[0] = '<none>';
	$vocabs = taxonomy_get_vocabularies();
	foreach($vocabs as $v) {
		$vids[$v->vid] = $v->name;
	}
	$form['afo_profile_city_vid'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for City field'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_profile_city_vid', 0),
	);	

	return system_settings_form($form);
}