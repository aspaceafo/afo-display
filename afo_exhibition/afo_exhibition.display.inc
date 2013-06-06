<?php
// $Id$

/**
 * @file
 *
 * Display functions for Exhibition nodes.
 */
 
/**
 * Alter node object to include exhibition content.
 */
function afo_exhibition_nodedisplay(&$node) {
	
	global $user;
	
	// curated by
	$node->content['curator_label'] = array(
		'#weight' => -14,
		'#value' => t('Exhibition curated by ').l($node->name, 'user/'.$node->uid),
	);
	
	// public/private
	if($node->field_visibility[0]['value'] == 2) {
		// is private
		if($user->uid != $node->uid && !user_access('access private exhibitions')) {
			drupal_set_message(t('This exhibition is set to private.'), 'warning');
			$node->content = array();
			return null;
		}
		else {
			drupal_set_message(t('Note: this exhibition is set to private; other users will only be able to view it if you switch it to Public.'));
		}
	}

	if(is_array($node->exhibition_items)) {
		foreach($node->exhibition_items as $item) {
			$exhibition_items[] = $item->item_nid;
		}
	}
	
	if(count($exhibition_items) > 0) {

		// slide show
		$slideshow = theme('afo_display_slideshow', $exhibition_items, null, 'artwork', array('id'=>'afo-exhibition-slides', 'classes'=>array('afo-exhibition-display')));
		if($slideshow) {
			$node->content['slideshow']['#weight'] = -5;
			$node->content['slideshow']['#value'] = $slideshow;
		}
	
		// slides grid
		$slidesgrid = theme('afo_display_grid', $exhibition_items, null, 'artwork', array('id'=>'afo-exhibition-grid', 'classes'=>array('hide', 'afo-exhibition-display')));
		if($slidesgrid) {
			$node->content['slidesgrid']['#weight'] = -4;
			$node->content['slidesgrid']['#value'] = $slidesgrid;
		}
	
		/*
		// slides lightbox
		$slideslightbox = theme('afo_display_lightbox', $exhibition_items, null, 'artwork', array('id'=>'afo-exhibition-fullscreen'));
		if($slideslightbox) {
			$node->content['slideslightbox']['#weight'] = -3;
			$node->content['slideslightbox']['#value'] = $slideslightbox;
		}
		*/
		
		// page details
		// move body to details
		$node->details['body'] = $node->content['body']; unset($node->content['body']);
		// add exhibition options to details
		$node->details['options'] = array(
			'#value' => afo_exhibition_build_options($node->nid),
			'#weight' => 5,
		);
		
		// page controls
		if($slideshow && $slidesgrid) {
			drupal_add_js(drupal_get_path('module', 'afo_exhibition').'/afo_exhibition.js');
			$page_controls = afo_exhibition_build_controls($exhibition_items);
			$node->content['controls']['#weight'] = -13;
			$node->content['controls']['#value'] = $page_controls;
		}

	}
	
}

/**
 * Create switch between slide and grid view.
 */
function afo_exhibition_build_controls($exhibition_items = null) {

	$options[] = array(
		'#type' => 'slides',
		'#target' => 'afo-exhibition-slides',
		'#close' => 'afo-exhibition-display',
	);
	$options[] = array(
		'#type' => 'grid',
		'#target' => 'afo-exhibition-grid',
		'#close' => 'afo-exhibition-display',
	);
	if(is_array($exhibition_items)) {
		$options[] = array(
			'#type' => 'fullscreen',
			'#target' => 'afo-exhibition-fullscreen',
			'#class' => array('afo-slidelightbox-launch'),
			'#nids' => $exhibition_items,
		);
	}
	$options[] = array(
		'#type' => 'details',
		'#target' => 'afo-exhibition-details',
		'#close' => 'afo-exhibition-display',
	);
	
	$content .= theme('afo_display_view_options', $options);

	return $content;
}

/**
 * Create exhibition options list
 */
function afo_exhibition_build_options($nid) {
	if(!is_numeric($nid)) return null;

	global $user;

	// FAVORITES
	if(module_exists('afo_favorite')) {
		if(!afo_favorite_get($user->uid, 'node', $nid)) {
			$links[] = l(t('Add Exhibition to Favorites'), 'favorites/add/node/'.$nid);
		}
	}

	if(is_array($links)) {
		$content .= '<div class="artwork-links">';
		$content .= theme('item_list', $links, null, 'ul', array('class'=>'tightList'));
		$content .= '</div>';
	}	

	return $content;
}


