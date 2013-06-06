<?php
// $Id$

/**
 * @file
 *
 * Browse and list functions for AFO Featured module.
 */

/***************************************** display functions **/

/**
 * Browse featured page
 */
function afo_featured_browsepage() {

	$content['column-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => 1,
	);
	$content['column-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#weight' => 2,
	);
	
	$intro_text = variable_get('afo_featured_curatingpage_intro', null);
	if($intro_text) {
		$content['column-left']['intro'] = array(
			'#type' => 'afo_contentbox',
			'#value' => t($intro_text),
			'#weight' => -10,
		);
	}
	
	/******* curators *********/
	/*
	$content['column-left']['curators'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('The Curators'),
		'#value' => afo_featured_getcurators(),
		'#weight' => -1,
	);
	*/

	/******* features *********/
	
	$data = afo_featured_getarchive();

	$current_col = 'column-right';
	//$current_col = 'column-left';
	foreach($data as $feature) {
		// check if featured content
		if(is_array($feature['data']['nodes']) || is_array($feature['data']['users'])) {
			$content[$current_col]['feature-'.$feature['feature_name']] = array(
				'#type' => 'afo_contentbox',
				'#value' => theme('afo_featured_feature_type', $feature),
			);
			
			//if($current_col == 'column-right') $current_col = 'column-left';
			//else $current_col = 'column-right';
		}
	}
	
	return drupal_render($content);
}

/**
 * Theme feature type function
 */
function theme_afo_featured_feature_type($feature) {
	if(!is_array($feature)) return null;

	$thumbnail_for_first = 3;
	
	$content .= '<h3 class="title">'.$feature['feature_name'].'</h3>';
	if($feature['feature_description']) $content .= '<div class="secondaryText">'.$feature['feature_description'].'</div>';

	$types = array('nodes', 'users');
	foreach($types as $type) {
		if(is_array($feature['data'][$type])) {
			$list = array();
			foreach($feature['data'][$type] as $featured) {
				
				$list[] = theme('afo_featured_featured_listing', $featured, $type, 'full');
				
			}
			$content .= theme('item_list', $list, null, 'ul', array('class'=>''));
		}
	}
	
	return $content;
}

/**
 * Theme featured item listing function
 */
function theme_afo_featured_featured_listing($featured, $type = 'node', $op = 'short') {
	if(!is_array($featured)) return null;

	switch($type) {
		case 'nodes':
			$label = $featured['title'];
			$dest = 'node/'.$featured['nid'];		
			if($op == 'full') $thumbnail = theme('afo_display_node_thumbnail', $featured['nid'], null, $dest);
		break;
			
		case 'users':
			$label = $featured['name'];
			$dest = 'user/'.$featured['uid'];
			if($op == 'full') $thumbnail = theme('afo_display_user_thumbnail', $featured['uid'], null, $dest);
			break;
	}

	if($label && $dest) {

		$content .= "<div class='featured-item-$op'>";

		switch($op) {

			case 'full':
				if($thumbnail) $content .= '<div class="featured-thumbnail">'.$thumbnail.'</div>';
				$content .= '<div class="featured-link">' . l($label, $dest) . '</div>';
				if($type == 'nodes') $content .= '<div class="featured-byline">' . 'by ' . $featured['name'] . '</div>';
				if(strlen($featured['featured_name'])>0) $content .= '<div class="featured-name">'.$featured['featured_name'].'</div>';
				if(strlen($featured['featured_note'])>0) $content .= '<div class="featured-note secondaryText">'.$featured['featured_note'].'</div>';					
				break;
	
			default:
				$content .= '<div class="featured-link">'.l($label, $dest).'</div>';	
				if(strlen($featured['featured_name'])>0) $content .= '<div class="featured-name">'.$featured['featured_name'].'</div>';
				if(strlen($featured['featured_note'])>0) $content .= '<div class="featured-note secondaryText">'.$featured['featured_note'].'</div>';		
		}
		
		$content .= '</div>';
	}
	
	return $content;
}

/***************************************** db functions **/

/**
 * Get feature types and items array
 */
function afo_featured_getarchive() {

	$query = db_query('SELECT * FROM {afo_featured_types} ORDER BY feature_name ASC');
	while($row = db_fetch_array($query)) $data[] = $row;
	
	if(!is_array($data)) return null;
	
	foreach($data as $k => $feature) {
		$data[$k]['data'] = afo_featured_getfeature($feature['feature_id']);
	}
	
	return $data;

}
/**
 * Get feature data
 */
function afo_featured_getfeature($feature_id) {
	if(!is_numeric($feature_id)) return null;

	// add featured nodes
	$query = db_query('
		SELECT f.*, n.title, u.name 
		FROM {afo_featured_nodes} f 
		LEFT JOIN {node} n ON n.nid = f.nid
		LEFT JOIN {users} u ON u.uid = n.uid
		WHERE f.feature_id = %d 
		ORDER BY f.updated DESC
	', $feature_id);
	while($row = db_fetch_array($query)) $data['nodes'][] = $row;

	// add featured users
	$query = db_query('SELECT f.*, u.name FROM {afo_featured_users} f LEFT JOIN {users} u ON u.uid=f.uid WHERE f.feature_id = %d ORDER BY f.updated DESC', $feature_id);
	while($row = db_fetch_array($query)) $data['users'][] = $row;	
	
	return $data;
}

/**
 * Get curators and their exhibitions
 */
function afo_featured_getcurators() {

	// get data
	$curator_rid = variable_get('afo_custommenu_roles_curator', null);
	if($curator_rid) {
		$query = db_query('SELECT u.name, u.uid FROM {users} u
		LEFT JOIN {users_roles} r ON r.uid = u.uid
		WHERE r.rid = %d
		ORDER BY u.created DESC', $curator_rid);
		
		while($row = db_fetch_object($query)) {
			$row->exhibitions = afo_exhibition_user_exhibitions($row->uid, 'select');			
			$curators[] = $row;
		}
		
	}
	
	// format list
	$curatorLinks = array();
	foreach($curators as $c) {
		$thisCurator = l($c->name, 'user/'.$c->uid);
		
		$thisCuratorEx = array();
		if(is_array($c->exhibitions)) {
			foreach($c->exhibitions as $nid => $title) {
				$thisCuratorExhibitions[] = l($title, 'node/'.$nid);
			}
		}
		$thisCurator .= theme('item_list', $thisCuratorExhibitions, null, 'ul', array('class'=>'arrow tightList'));
		$curatorLinks[] = $thisCurator;
	}
	$content = theme('item_list', $curatorLinks);
	
	return $content;
}