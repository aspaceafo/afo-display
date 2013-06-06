<?php
// $Id$

/**
 * @file
 *
 * Home page functions for AFO Custom Menu module.
 */

/**
 * Build home page.
 */
function afo_custommenu_homepage() {

	global $user;

	$home['column-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '350px',
		'#weight' => -1,
	);
	$home['column-center'] = array(
		'#type' => 'afo_column',
		'#position' => 'center',
		'#width' => '80px',
		'#weight' => 0,
	);
	$home['column-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '600px',
		'#weight' => 1,
	);
	$home['column-bottom'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '100%',
		'#weight' => 10,
	);

	$home['column-left']['logo'] = array(
		'#prefix' => '<div class="logo">',
		'#value' => theme('afo_display_icon', 'logo'),
		'#suffix' => '</div>',
		'#weight' => -10,
	);		

	/**
	if($user->uid > 0) {
		// welcome
		$welcome = t('Welcome, @name.', array('@name'=>$user->name));
		$welcome .= '<br />';
		$welcome .= l(t('» see your Control Panel'), 'control-panel');
	}
	else {
		// login / register
		$welcome_links[] = l(t('Member log in'), 'user', array('query'=>'target=#login-box&noscroll=1', 'attributes'=>array('class'=>'opener buttonLink'))) . '<div id="login-box" class="hide">' . drupal_get_form('user_login') . '</div>';
		$welcome_links[] = l(t('Not a member? Click here to register.'), 'sign-up');
		$welcome = theme('item_list', $welcome_links);
	}	
	$home['column-left']['login'] = array(
		'#title' => $welcome_title,
		'#value' => $welcome,
		'#suffix' => '<div style="height: 12px">&nbsp;</div>',
		'#weight' => -2,
	);
	**/
	
	$mission = afo_custommenu_site_statement();
	if($mission) {
		//$mission .= '<br />' . l(t('» read more'), 'node/32495');
		$home['column-left']['intro'] = array(
			'#value' => $mission,
			'#suffix' => '<div style="height: 12px">&nbsp;</div>',
			'#weight' => -3,
		);	
	}
	
	/*
	module_load_include('inc', 'afo_custommenu', 'afo_custommenu.blog');
	$home['column-left']['blog'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Blog'),
		'#value' => afo_custommenu_blogpage(array('display' => 'short', 'per page' => 3, 'no pager' => true)) . l(t('See more »'), 'community/blog'),
		'#weight' => 0,
	);
	*/

	if(module_exists('afo_featured')) {
		$feature_id = variable_get('afo_featured_homepage_type', null);
		if(is_numeric($feature_id)) {
			$home['column-right']['featured_slideshow'] = array(
				'#prefix' => '<br /><br /><br />',
				'#value' => theme('afo_featured_mostrecent', $feature_id, array('imagecache_preset' => 'artwork_full')),
				'#weight' => -1,
			);
		}		
	}

	if(module_exists('afo_newsfeed')) {
		module_load_include('inc', 'afo_newsfeed', 'afo_newsfeed.db');
		$newsfeed = afo_newsfeed_output(array('numrows'=>50));
		if(count($newsfeed > 0)) {
			$home['column-bottom']['newsfeed'] = array(
				'#value' => theme('newsfeed', $newsfeed, 'normal', t('Recent Activity')),
				'#weight' => 10,
			);	
		}		
	}
	
	return drupal_render($home);
}