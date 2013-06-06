<?php
// $Id$

/**
 * @file
 *
 * Adds block of options for artworks.
 */
 
/**
 * Build artwork options block.
 */
function afo_custommenu_artwork_options($nid, $type=null, $uid=null) {
	if(!is_numeric($nid)) return null;
	
	if($type && $uid) {
		$node_data = new stdclass;
		$node_data->type = check_plain($type);
		$node_data->uid = $uid;
	}
	else {
		$node_data = db_fetch_object(db_query('SELECT n.type, n.uid FROM {node} n WHERE n.nid = %d', $nid));
	}
	if($node_data->type != variable_get('afo_settings_types_artwork', 'none')) return null;
	
	global $user;
	drupal_add_js(drupal_get_path('module', 'afo_custommenu').'/artwork_options.js');

	// ARTWORK details
	$links[] = l(t('Artwork details'), '#', array('attributes'=>array('class'=>'js-show show-artwork-details')));

	// PORTFOLIO functions
	if(module_exists('afo_portfolio') && is_numeric($node_data->uid)) {
		// build full portfolio
		include_once(drupal_get_path('module', 'afo_portfolio').'/afo_portfolio.classes.inc');
		$portfolio = new portfolioPortfolio(array('uid'=>$node_data->uid));

		// print level (folder or base portfolio)
		$links[] = $portfolio->printItem('level', array('nid' => $nid, 'collapsed' => true));

		// print previous and next links
		$links[] = $portfolio->printPrevNext($nid);
	}

	// I LIKE THIS placeholder
	if(afo_portfolio_checkmember($node_data->uid)) {
		if (in_array($node_data->type, variable_get('plus1_nodetypes', array()))) {
			if(user_access('own content can be liked', user_load($node_data->uid))) {
				// Show the widget.
				if(!$artwork_node) $artwork_node = node_load($nid);
				$links[] = plus1_jquery_widget($artwork_node, null, null);
			}
		}
	}

	
	// FAVORITES
	if(module_exists('afo_favorite')) {
		if(!afo_favorite_get($user->uid, 'node', $nid)) {
			$links[] = l(t('Add to Favorites'), 'favorites/add/node/'.$nid);
		}
		/*
		if(is_numeric($node_data->uid) && !afo_favorite_get($user->uid, 'user', $node_data->uid)) {
			$links[] = l(t('Follow Artist'), 'favorites/add/user/'.$node_data->uid);
		}
		*/
	}

	// EXHIBITIONS
	if(module_exists('afo_exhibition')) {
		// included in
		$exhibitions = afo_exhibition_getexhibitions($nid, 'nids');
		if(is_array($exhibitions)) {
			$included_in .= l(t('Included in...'), '', array('query'=>'target=#includedIn', 'attributes'=>array('class'=>'opener js-show')));
			$included_in .= '<div id="includedIn" class="hide">';
			$included_in .= theme('afo_display_grid', $exhibitions, t('Included in exhibitions'), 'exhibitions');
			$included_in .= '</div>';
			
			$links[] = $included_in;
//			dsm($exhibitions);$items, $title = null, $op = null, $options = array()
		}

		// add to exhibition
		if(user_access(afo_exhibition_getperm())) {
			$links[] = l(t('Add to Exhibition'), "exhibition/add-form/$nid", array());
		}
	}

	
	if(is_array($links)) {
		$content .= '<div class="artwork-links">';
		$content .= theme('item_list', $links, null, 'ul', array('class'=>'tightList'));
		$content .= '</div>';
	}
	
	return $content;
}