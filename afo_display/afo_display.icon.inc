<?php
// $Id$

/**
 * @file
 *
 * Display functions for icons
 */
 
 /**
  * theme icon function
  */
function theme_afo_display_icon($type, $options = array()) {
	
	$type = str_replace('_', ' ', $type);

	$icons = array(
		'expand' => 'icon_view_expand',
		'event' => 'icon_event',
		'open call' => 'icon_opencall',
		'user' => 'icon_user',
		'logo' => 'logo',
		'arrow' => 'Arrow-Right',
		'arrow down' => 'Arrow-Down',
		'scroll left' => 'icon_scroll_left',
		'scroll right' => 'icon_scroll_right',
		'portfolio' => 'cp-portfolio',
		'add artwork' => 'cp-artwork',
		'add folder' => 'cp-folder',
		'add event' => 'cp-new-event',
		'profile' => 'cp-profile',
		'cv' => 'cp-cv',
		'settings' => 'cp-settings',
		'membership' => 'cp-membership',
		'star' => 'cp-star',
		'grid' => 'cp-grid',
		'city' => 'cp-city',
		'calendar' => 'cp-calendar',
	);
	
	if(!isset($icons[$type])) return null;

	$images_folder = drupal_get_path('module', 'afo_display').'/images/';
	$image_path = $images_folder . $icons[$type];
	if($options['size']) $image_path .= '-' . $options['size'];
	$image_path .= '.png';

	if($options['path only']) return $image_path;

	if(!$options['alt']) $alt = t($type);

	$img = theme('image', $image_path, $alt, $alt, array('class' => 'icon'));

	return $img;
}