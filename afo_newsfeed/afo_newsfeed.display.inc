<?php
// $Id$

/**
 * @file
 *
 * AFO Newsfeed display functions.
 */

/**
 * Theme newsfeed listing.
 */
function theme_newsfeed($newsfeed, $op='normal', $title=null) {

	drupal_add_js(drupal_get_path('module', 'afo_newsfeed').'/newsfeed.js');
	drupal_add_css(drupal_get_path('module', 'afo_newsfeed').'/newsfeed.css');

	$content = '<div class="afo-newsfeed"><div class="afo-newsfeed-inner">';

	if($title) $content .= "<h2>$title</h2>";

	foreach($newsfeed as $item) {
		$content .= theme('newsfeed_event', $item, $op);	
	}
	
	$content .= '<div class="horiz-clear">&nbps;</div>';
	$content .= '</div></div>';
	
	return $content;
}

/**
 * Theme individual newsfeed event.
 */
function theme_newsfeed_event($item, $op='normal') {

	$event_date = date('j M y', $item['timestamp']);
	$user_name = $item['user_name']; //l($item['user_name'], 'user/'.$item['uid']);
	$event_name = afo_newsfeed_event_convert($item['type_id']);
	
	if(strlen($item['arg0_filepath']) > 0) {
		$arg0_data = array(
			'nid' => $item['arg0_nid'],
			'title' => $item['arg0_title'],
			'image' => $item['arg0_filepath'],
			'name' => $item['name'],
		);
	}
	
	switch($event_name) {
		case 'create node':
			$nid = $item['arg0'];
			$node_type = $item['arg1'];
			$destination = "node/$nid";
			$thumb_data = is_array($arg0_data) ? $arg0_data : $nid;
			$thumbnail = theme('afo_display_node_thumbnail', $thumb_data, null, $destination);
			if($op=='short') {
				if($node_type == 'artwork') $event_description = "Added new $node_type.";
				else $event_description = "Added a new $node_type.";
			}
			else {
				$node_type_name = str_replace('_', ' ', $node_type);
				if($node_type == 'artwork') $event_description = "$user_name added new $node_type_name.";
				else $event_description = "$user_name added a new $node_type_name.";
			} 
			//$link = l(t('View »'), $destination);
			break;
			
		case 'included in exhibition':
			$item_nid = $item['arg0'];
			$exhibition_nid = $item['arg1']; 
			$destination = "node/$exhibition_nid";
			$thumb_data = is_array($arg0_data) ? $arg0_data : $item_nid;
			$thumbnail = theme('afo_display_node_thumbnail', $thumb_data, null, $destination);
			if($op=='short') {
				$event_description = "Included in exhibition.";
			}
			else {
				$event_description = "{$user_name}'s artwork was included in an exhibition.";
			} 
			//$link = l(t('View »'), $destination);
			break;

		case 'added to exhibition':
			$item_nid = $item['arg0'];
			$exhibition_nid = $item['arg1'];
			$destination = "node/$exhibition_nid";
			$thumb_data = is_array($arg0_data) ? $arg0_data : $item_nid;
			$thumbnail = theme('afo_display_node_thumbnail', $thumb_data, null, $destination);
			if($op=='short') {
				$event_description = "Added to their exhibition.";
			}
			else {
				$event_description = "{$user_name} added to an exhibition.";
			} 
			//$link = l(t('View »'), $destination);
			break;

		case 'user featured':
			$uid = $item['arg0'];
			$feature_name = $item['arg2'];
			$destination = "user/$uid";
			$thumbnail = theme('afo_display_user_thumbnail', $uid, null, $destination);
			if($op=='short') {
				$event_description = "Featured";
				if($feature_name) $event_description .= " in the $feature_name";
				$event_description .= '.';
			}
			else {
				$event_description = "{$user_name} was featured";
				if($feature_name) $event_description .= " in the $feature_name";
				$event_description .= '.';
			} 
			//$link = l(t('View »'), $destination);
			break;

		case 'node featured':
			$nid = $item['arg0'];
			$feature_name = $item['arg2'];
			$destination = "node/$nid";
			$thumb_data = is_array($arg0_data) ? $arg0_data : $nid;
			$thumbnail = theme('afo_display_node_thumbnail', $thumb_data, null, $destination);
			if($op=='short') {
				$event_description = "Featured";
				if($feature_name) $event_description .= " in the $feature_name";
				$event_description .= '.';
			}
			else {
				$event_description = "{$user_name}'s work was featured";
				if($feature_name) $event_description .= " in the $feature_name";
				$event_description .= '.';
			} 
			//$link = l(t('View »'), $destination);
			break;

	}

	/*	
	if(!$thumbnail) {
		dsm('Newsfeed Thumbnail missing for following item.');
		dsm($item);
	};
	*/
	
	$content = '';
	$content .= '<div class="item">'; 
	$content .= $thumbnail;
	$content .= '<div class="details js-hide"><div class="details-inner">';
	$content .= "<div class='date'>$event_date</div>";
	$content .= t($event_description);
	$content .= '</div></div>';		
	$content .= '</div>';		

	return $content;
}