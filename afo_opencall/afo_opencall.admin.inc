<?php
// $Id$

/**
 * Admin callbacks for AFO Open Call module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_opencall_admin_settings() {

	$gettypes = node_get_types('names');
	$content_types[''] = '-- none --';
	$content_types = array_merge($content_types, $gettypes);
	
	$form['afo_settings_types_opencall'] = array(
		'#type' => 'select',
		'#title' => t('Open call content type'),
		'#description' => t('Select the content type to be used for open call nodes.'),
		'#options' => $content_types,
		'#default_value' => variable_get('afo_settings_types_opencall', ''),
	);	

	$form['submissions'] = array(
		'#type' => 'fieldset',
		'#title' => 'Submission options',
	);
	$form['submissions']['afo_settings_opencall_limit_default'] = array(
		'#type' => 'textfield',
		'#title' => t('Default submission limit'),
		'#size' => 4,
		'#default_value' => variable_get('afo_settings_opencall_limit_default', null),
	);	

	// browse calls teaser block:
	$form['calls-page'] = array(
		'#type' => 'fieldset',
		'#title' => t('Open Calls home page'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['calls-page']['afo_opencall_callspage_intro'] = array(
		'#type' => 'textarea',
		'#title' => t('Intro text'),
		'#description' => t('Introductory paragraph for Open Calls home page'),
		'#cols' => 40,
		'#rows' => 3,
		'#resizable' => true,
		'#default_value' => variable_get('afo_opencall_callspage_intro', ''),
	);

	return system_settings_form($form);
}