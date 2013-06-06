<?php
// $Id$

/**
 * @file
 *
 * Display functions for AFO Featured module.
 */


/**
 * Theme featured item
 */
function theme_afo_featured_item($item_id, $op, $options = array()) {

	$item = afo_featured_getitem($item_id, $op);
	
	if(!$item) return null;
	
	switch($op) {
		case 'node':
			if(!is_numeric($item->nid)) return null;
			$featured_node = node_load($item->nid);
			$featured_item_title = l($item->title, 'node/'.$item->nid) . '<br />' . $item->name;
			$destination = 'node/' . $item->nid;
			if(module_exists('afo_exhibition')) {
				if($featured_node->type == variable_get('afo_exhibition_content_type', 'none')) {
					// is exhibition
					foreach($featured_node->exhibition_items as $i) {
						// get exhibition item nids
						$nids[] = $i->item_nid;
					}
				}
			}
			break;

		case 'user':
			if(!is_numeric($item->uid)) return null;
			$featured_item_title = l($item->name, 'user/'.$item->uid);
			$destination = 'user/' . $item->uid;
			break;
	}

	if(is_array($nids)) {
	
		// required stuff
		drupal_add_css(drupal_get_path('module', 'afo_featured').'/featured.css');
	
		// featured div
		$output .= '<div class="afo-featured"><div class="afo-featured-inner">';

		// feature and featured titles
		if(strlen($item->feature_name) > 0 || strlen($item->featured_name) > 0) { 
			$output .= '<div class="feature-label">';
				if(strlen($item->feature_name) > 0) $output .= '<span class="feature-name">'.t($item->feature_name).': </span>';
				if(strlen($item->feature_name) > 0) $output .= '<span class="featured-name">'.l(t($item->featured_name), $destination).'</span>';
			$output .= '</div>';
		}
		// note
		//if(strlen($item->featured_note) > 0) $output .= '<div class="featured-note">'.t($item->featured_note).'</div>';		
		
		// slide show
		if(!isset($options['imagecache_preset'])) $options['imagecache_preset'] = variable_get('afo_featured_slideshow_imagecachepreset', null);
		$output .= theme('afo_display_slideshow', $nids, null, 'artwork', $options); // replace null with $featured_item_title for title
		
		// close div
		$output .= '</div></div>';
	}
	
	return $output;
}

/**
 * Theme recent featured items.
 */
function theme_afo_featured_mostrecent($feature_id, $options = array()) {
	$items = afo_featured_mostrecent($feature_id, 1);
	$output = theme('afo_featured_item', $items[0], 'node', $options);
	return $output;
}


