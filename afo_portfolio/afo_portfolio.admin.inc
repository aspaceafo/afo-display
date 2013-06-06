<?php
// $Id$

/**
 * Admin callbacks for AFO Portfolio module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_portfolio_admin_settings() {

	$types = node_get_types('names');
	$options[''] = '-- none --';
	$options = array_merge($options, $types);

	$form['afo_portfolio_artwork_content_type'] = array(
		'#type' => 'select',
		'#title' => t('Choose Artwork content type'),
		'#options' => $options,
		'#default_value' => variable_get('afo_portfolio_artwork_content_type', ''),
		'#description' => t('This content type will be used as the portfolio artworks.'),
	);

	$form['afo_portfolio_folder_content_type'] = array(
		'#type' => 'select',
		'#title' => t('Choose Folder content type'),
		'#options' => $options,
		'#default_value' => variable_get('afo_portfolio_folder_content_type', ''),
		'#description' => t('This content type will be used as the portfolio folders.'),
	);

	$form['afo_portfolio_artwork_max'] = array(
		'#type' => 'textfield',
		'#size' => '10',
		'#title' => t('Artworks max'),
		'#default_value' => variable_get('afo_portfolio_artwork_max', ''),
		'#description' => t('Maximum total number of artworks that non-paying members can create.'),
	);

	$form['afo_portfolio_thumbnail_default_preset'] = array(
		'#type' => 'textfield',
		'#size' => '40',
		'#title' => t('Thumbnail default imagecache preset'),
		'#default_value' => variable_get('afo_portfolio_thumbnail_default_preset', ''),
		'#description' => t('The preset used for portfolio thumbnails, unless otherwise specified.'),
	);
	
	$menus[''] = '<none>';
	$menus = array_merge($menus, menu_get_menus());
	$form['afo_portfolio_manage_menuname'] = array(
		'#type' => 'select',
		'#title' => t('Manage portfolio menu'),
		'#description' => t('Select the menu to print from the Manage Portfolio block'),
		'#options' => $menus,
		'#default_value' => variable_get('afo_portfolio_manage_menuname', ''),
	);

	return system_settings_form($form);

}