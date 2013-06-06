<?php
// $Id$

/**
 * @file
 *
 * AFO Hierarchy functions
 */

/**
 * theme hierarchy element
 */ 
function theme_afo_backend_hierarchy(&$element) {

	// determine vid:
	$vid = _afo_backend_get_vid($element['#name']);

	// if found vid
	if(is_numeric($vid)) {
		// get vocab details
		$vocab = taxonomy_vocabulary_load($vid);
		$total_levels = 3;

		// check if value set in post
		if(is_array($element['#post'])) {
			$post_val = _afo_backend_hierarchy_getDeepestFromPost($vid, $element['#post']);
			if(is_numeric($post_val)) $element['#default_value'][0] = $post_val;
		}
		
		// default value
		if(is_numeric($element['#default_value'][0])) {
			$selected = taxonomy_get_parents_all($element['#default_value'][0]);
			$selected = array_reverse($selected);
		}		
		
		// get vocabulary as stacked array
		$top = taxonomy_get_tree($vid, 0, -1, 1);
		foreach($top as $term) {
			_afo_backend_get_children($term);
			$tree[$term->tid] = $term;
		}
		
		// create select of top level
		//$top_level = array('' => t('choose...'));
		//foreach($tree as $term) {
		//	$top_level[$term->tid] = $term->name;
		//}

		$content .= '<div id="afo-hierarchy-vid-'.$vid.'" class="afo-hierarchy js-show">';
		for($l = 0; $l < $total_levels; $l++) {
			$content .= '<select name="afo-hierarchy-vid-'.$vid.'-level-'.$l.'" id="level-'.$l.'">';
			
				$options = null;
				if($l == 0) $options = $tree;
				elseif(is_array($nextOptions)) $options = $nextOptions;
				$nextOptions = null;

				if(!isset($selected[$l])) $sel = 'SELECTED="selected"';
				else $sel = '';
				$content .= '<option value="" '.$sel.'></option>';

				if($options) {
					foreach($options as $term) {
						if($selected[$l]->tid == $term->tid) {
							$sel = 'SELECTED="selected"';
							$nextOptions = $term->children;
						}
						else $sel = '';
						$content .= '<option value="'.$term->tid.'" '.$sel.'>'.$term->name.'</option>';
					}
				}
			
			$content .= '</select><br />';
		}
		$content .= '</div>'; // close afo-hierarchy

		$select = array(
			'#type' => 'markup',
			'#title' => t($vocab->name),
			'#description' => t($vocab->help),
			'#required' => $vocab->required,
		);
		
		// send $tree array to JS
		drupal_add_js(array(
			'afo_backend_hierarchy' => array(
					$vid => $tree,
				)
			), 'setting');
		// add js script
		drupal_add_js(drupal_get_path('module', 'afo_backend').'/hierarchy.js');
		
		// render & return element
		$select = theme('form_element', $select, $content);
		return $select;
	}
	
	// else return regular rendering
	else {
		return drupal_render($element);
	}
}

/**
 * hierarchy select validate function
 */
function _afo_backend_hierarchyvalidate(&$element, &$form_state) {

	// determine vid:
	$vid = _afo_backend_get_vid($element['#name']);

	// if found vid
	if(is_numeric($vid)) {
		/*
		// run though post vals to find extra selections
		foreach($element['#post'] as $key => $val) {
			if(substr($key, 0, 18) == 'afo-hierarchy-vid-') {
				$key = explode('-', $key);
				// check if post vals for this vid
				if($key[3] == $vid) {
					$level = $key[5];
					if(is_numeric($level) && $level > $deepest && is_numeric($val)) {
						$deepest = $level;
						$deepest_val = $val;
					}
				}
			}
		}*/
		
		$deepest_val = _afo_backend_hierarchy_getDeepestFromPost($vid, $element['#post']);
		
		if(is_numeric($deepest_val)) {
			// set element value to deepest val;
			$form_state['values'][$element['#name']] = array($deepest_val => $deepest_val);
			$element['#default_value'] = array(0 => $deepest_val);
			$element['#value'] = array(0 => $deepest_val);
		}
		else {
			// set error if required, because required has been removed when hierarchy added
			$vocab = taxonomy_vocabulary_load($vid);
			if($vocab->required) {
				form_set_error("afo-hierarchy-vid-$vid-level-0", t('@name is required.', array('@name' => $vocab->name)));
			}
		}
			
	}
}

/**
 * Run through post and get deepest level value for given VID
 */
function _afo_backend_hierarchy_getDeepestFromPost($vid, $post) {
	foreach($post as $key => $val) {
		if(substr($key, 0, 18) == 'afo-hierarchy-vid-') {
			$key = explode('-', $key);
			// check if post vals for this vid
			if($key[3] == $vid) {
				$level = $key[5];
				if(is_numeric($level) && $level > $deepest && is_numeric($val)) {
					$deepest = $level;
					$deepest_val = $val;
				}
			}
		}
	}
	
	return $deepest_val;
}

/**
 * hierarchy select submit function
 */
function _afo_backend_hierarchysubmit($form, &$form_state) {
}

/**
 * get vocab helper function
 */
function _afo_backend_get_children(&$term) {
	if(is_numeric($term->tid)) {
		// get children
		$term->children = taxonomy_get_children($term->tid);
		if(is_array($term->children)) {
			foreach($term->children as $child) {
				_afo_backend_get_children($child);
			}
		}
	}
}

/**
 * get vid from element name
 */
function _afo_backend_get_vid($name) {
	if(substr($name, 0, 11) == 'user_terms_') $vid = substr($name, 11);
	return $vid;
}