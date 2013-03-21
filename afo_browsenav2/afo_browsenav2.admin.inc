<?php
// $Id$

/**
 * Admin callbacks for AFO BrowseNav2 module.
 */

/**
 * Form builder for browsenav2 admin settings page.
 */
function afo_browsenav2_admin_settings() {

	$form['afo_browsenav2_homepage'] = array(
		'#type' => 'textfield',
		'#title' => t('Node Id (nid) of Browse homepage'),
		'#default_value' => variable_get('afo_browsenav2_homepage', ''),
		'#description' => t('This page will be the home page for the Browse feature.'),
	);

	$form['afo_browsenav2_results_per_page'] = array(
		'#type' => 'textfield',
		'#title' => t('Results per page'),
		'#size' => 10,
		'#default_value' => variable_get('afo_browsenav2_results_per_page', ''),
		'#description' => t('The number of results to display per page when Browsing.'),
	);

	$vids[0] = '<none>';
	$vocabs = taxonomy_get_vocabularies();
	foreach($vocabs as $v) {
		$vids[$v->vid] = $v->name;
	}
	$form['taxonomy'] = array(
		'#type' => 'fieldset',
		'#title' => t('Taxonomy settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['taxonomy']['afo_browsenav2_medium_vid'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Medium selection'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_browsenav2_medium_vid', 0),
	);
	$form['taxonomy']['afo_browsenav2_styles_vid'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Styles selection'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_browsenav2_styles_vid', 0),
	);
	$form['taxonomy']['afo_browsenav2_tags_vid'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Tags selection'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_browsenav2_tags_vid', 0),
	);
	$form['taxonomy']['afo_browsenav2_region_vid'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Region selection'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_browsenav2_region_vid', 0),
	);
	
	$roles = user_roles(true);
	$form['roles'] = array(
		'#type' => 'fieldset',
		'#title' => t('User role settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['roles']['afo_browsenav2_roles_artist'] = array(
		'#type' => 'select',
		'#title' => t('Artist role'),
		'#description' => t('This role will act as browsenav user filter for artists.'),
		'#options' => $roles,
		'#default_value' => variable_get('afo_browsenav2_roles_artist', 0),
	);
	
	return system_settings_form($form);

}