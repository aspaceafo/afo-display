<?php
// $Id$

/**
 * Admin callbacks for AFO Custommenu module.
 */


/**
 * Form builder for admin settings page.
 */
function afo_custommenu_admin_general() {

	$gettypes = node_get_types('names');
	$content_types[''] = '-- none --';
	$content_types = array_merge($content_types, $gettypes);
	
	/********************************** Artwork settings **/
	$form['artwork'] = array(
		'#type' => 'fieldset',
		'#title' => t('Artwork settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['artwork']['afo_settings_types_artwork'] = array(
		'#type' => 'select',
		'#title' => t('Content type'),
		'#description' => t('Select the content type to be used for artwork nodes.'),
		'#options' => $content_types,
		'#default_value' => variable_get('afo_settings_types_artwork', ''),
	);	
	if(module_exists('content')) {
		// CCK image field for artworks
		$fields_full = content_fields(null, variable_get('afo_settings_types_artwork', null));
		$artwork_fields[''] = '<none>';
		foreach($fields_full as $k=>$v) {
			$artwork_fields[$k] = $k;
		}
		// select image cck field
		$form['artwork']['afo_settings_artwork_imagefield'] = array(
			'#type' => 'select',
			'#title' => t('Image field'),
			'#description' => t('Select the CCK field that holds the artwork\'s image.'),
			'#options' => $artwork_fields,
			'#default_value' => variable_get('afo_settings_artwork_imagefield', ''),
		);
		// select video cck field
		$form['artwork']['afo_settings_artwork_videofield'] = array(
			'#type' => 'select',
			'#title' => t('Video field'),
			'#description' => t('Select the CCK field that holds the artwork\'s video.'),
			'#options' => $artwork_fields,
			'#default_value' => variable_get('afo_settings_artwork_videofield', ''),
		);	
	}

	/********************************** Blog settings **/
	$form['blog-posts'] = array(
		'#type' => 'fieldset',
		'#title' => t('Blog settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['blog-posts']['afo_settings_types_blog'] = array(
		'#type' => 'select',
		'#title' => t('Content type'),
		'#description' => t('Select the content type to be used for blog post nodes.'),
		'#options' => $content_types,
		'#default_value' => variable_get('afo_settings_types_blog', ''),
	);	

	/********************************** Event settings **/
	$form['events'] = array(
		'#type' => 'fieldset',
		'#title' => t('Event settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['events']['afo_settings_types_event'] = array(
		'#type' => 'select',
		'#title' => t('Content type'),
		'#description' => t('Select the content type to be used for event nodes.'),
		'#options' => $content_types,
		'#default_value' => variable_get('afo_settings_types_event', ''),
	);	
	if(module_exists('content')) {
		$fields_full = content_fields(null, variable_get('afo_settings_types_event', null));
		$event_fields[''] = '<none>';
		foreach($fields_full as $k=>$v) {
			$event_fields[$k] = $k;
		}
		// select event dates cck field
		$form['events']['afo_settings_event_datefield'] = array(
			'#type' => 'select',
			'#title' => t('Date field'),
			'#description' => t('Select the CCK field that holds the event\'s dates.'),
			'#options' => $event_fields,
			'#default_value' => variable_get('afo_settings_event_datefield', ''),
		);
	}
	
	/********************************** Tax vocab settings **/
	$vids[0] = '<none>';
	$vocabs = taxonomy_get_vocabularies();
	foreach($vocabs as $v) {
		$vids[$v->vid] = $v->name;
	}
	$form['vocabs'] = array(
		'#type' => 'fieldset',
		'#title' => t('Vocabulary settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['vocabs']['afo_custommenu_vocab_city'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for City field'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_custommenu_vocab_city', 0),
	);	
	$form['vocabs']['afo_custommenu_vocab_region'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Region field'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_custommenu_vocab_region', 0),
	);	
	$form['vocabs']['afo_custommenu_vocab_tags'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Tags field'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_custommenu_vocab_tags', 0),
	);	
	$form['vocabs']['afo_custommenu_vocab_medium'] = array(
		'#type' => 'select',
		'#title' => t('Vocabulary to use for Medium field'),
		'#options' => $vids,
		'#default_value' => variable_get('afo_custommenu_vocab_medium', 0),
	);

	
	/********************************** User roles settings **/
	$roles[] = '<none>';
	foreach(user_roles(true) as $k => $v) $roles[$k] = $v;
	$form['roles'] = array(
		'#type' => 'fieldset',
		'#title' => t('User roles'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['roles']['afo_custommenu_roles_artist'] = array(
		'#type' => 'select',
		'#title' => t('Artist Role'),
		'#description' => t('Non-paying artist'),
		'#options' => $roles,
		'#default_value' => variable_get('afo_custommenu_roles_artist', null),
	);
	$form['roles']['afo_custommenu_roles_artist_member'] = array(
		'#type' => 'select',
		'#title' => t('Member Role'),
		'#description' => t('Paying artist'),
		'#options' => $roles,
		'#default_value' => variable_get('afo_custommenu_roles_artist_member', null),
	);
	$form['roles']['afo_custommenu_roles_curator'] = array(
		'#type' => 'select',
		'#title' => t('Curator Role'),
		'#description' => t('Curator user'),
		'#options' => $roles,
		'#default_value' => variable_get('afo_custommenu_roles_curator', null),
	);
	
	/********************************** Node title settings **/
	$form['titles'] = array(
		'#type' => 'fieldset',
		'#title' => t('Node title settings'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['titles']['afo_custommenu_titles_hide'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Hide title'),
		'#options' => node_get_types('names'),
		'#default_value' => variable_get('afo_custommenu_titles_hide', array()),
		'#description' => t('The on page view, the node title will be hidden for these types.'),
	);
	$form['titles']['afo_custommenu_titles_icon'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Title icon'),
		'#options' => node_get_types('names'),
		'#default_value' => variable_get('afo_custommenu_titles_icon', array()),
		'#description' => t('Show title icon for these types.'),
	);

	/********************************** Limit terms settings **/
	$form['term-limits'] = array(
		'#type' => 'fieldset',
		'#title' => t('Limit terms'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['term-limits']['afo_custommenu_limit_terms_max'] = array(
		'#type' => 'textfield',
		'#title' => t('Terms maximum'),
		'#description' => t('Users will only be allowed to select this many terms for limited vocabularies.'),
		'#default_value' => variable_get('afo_custommenu_limit_terms_max', null),
		'#size' => 4,
	);
	$form['term-limits']['afo_custommenu_limit_terms_medium'] = array(
		'#type' => 'textfield',
		'#title' => t('Mediums max'),
		'#description' => t('Users will only be allowed to select this many terms for Medium.'),
		'#default_value' => variable_get('afo_custommenu_limit_terms_medium', null),
		'#size' => 4,
	);
	$form['term-limits']['afo_custommenu_limit_terms_tags'] = array(
		'#type' => 'textfield',
		'#title' => t('Tags max'),
		'#description' => t('Users will only be allowed to select this many terms for Tags.'),
		'#default_value' => variable_get('afo_custommenu_limit_terms_tags', null),
		'#size' => 4,
	);

	return system_settings_form($form);
}


/**
 * Form builder for admin settings page.
 */
function afo_custommenu_admin_settings() {

	$form['welcome_messages'] = array(
		'#type' => 'fieldset',
		'#title' => 'Welcome messages',
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['welcome_messages']['afo_custommenu_welcome_message_authenticated_user'] = array(
		'#type' => 'textarea',
		'#title' => t('Welcome message for all authenticated users'),
		'#default_value' => variable_get('afo_custommenu_welcome_message_authenticated_user', ''),
	);
	$form['welcome_messages']['afo_custommenu_welcome_message_artist'] = array(
		'#type' => 'textarea',
		'#title' => t('Welcome message for artists'),
		'#default_value' => variable_get('afo_custommenu_welcome_message_artist', ''),
	);
	$form['welcome_messages']['afo_custommenu_welcome_message_curator'] = array(
		'#type' => 'textarea',
		'#title' => t('Welcome message for curators'),
		'#default_value' => variable_get('afo_custommenu_welcome_message_curator', ''),
	);
	
	$menus[''] = '<none>';
	$menus = array_merge($menus, menu_get_menus());
	$form['afo_custommenu_manage_menuname'] = array(
		'#type' => 'select',
		'#title' => t('Manage content menu'),
		'#description' => t('Select the menu to print from the Manage Content block'),
		'#options' => $menus,
		'#default_value' => variable_get('afo_custommenu_manage_menuname', ''),
	);	

	$form['afo_custommenu_homepage_node'] = array(
		'#type' => 'textfield',
		'#title' => t('Home page node nid'),
		'#description' => t('Node nid to use as content for site home page.'),
		'#default_value' => variable_get('afo_custommenu_homepage_node', ''),
	);	
	
	return system_settings_form($form);

}

/**
 * Form builder for admin settings page.
 */
function afo_custommenu_homepage_settings() {

	return system_settings_form($form);

}