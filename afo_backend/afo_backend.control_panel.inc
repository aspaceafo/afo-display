<?php
// $Id$

/**
 * @file
 *
 * Control panel output
 */

/**
 * Theme control panel
 */
function theme_afo_backend_control_panel($cp, $options = array()) {

	/*********************** output control panel **/

	$content .= '<div class="afo-cp">';	
	foreach($cp as $section) {

		// run through items
		$section_items = array();
		foreach(element_children($section) as $k) {
			$item = $section[$k];
			// check if no access req or user passes access req
			if(!$item['#access'] || user_access($item['#access'])) {
				// check if path valid and user has access to path
				if(menu_valid_path(array('link_path'=>$item['#href']))) {
					$section_item = '';
					$section_item .= '<div class="afo-cp-item">';
					
					$button = '';
					if($item['#icon']) $button .= theme('afo_display_icon', $item['#icon']) . '<br />';
					$button .= $item['#title'];
					$section_item .= l($button, $item['#href'], array('html'=>true));
					
					$section_item .= '</div>';
					$section_items[] = $section_item;
				}
			}
		}

		// only show section if items present
		if(count($section_items) > 0) {
			$content .= '<div class="afo-cp-section">';
			$content .= '<div class="title">' . $section['#title'] . '</div>';
			$content .= implode('', $section_items);
			$content .= '<div class="horiz-clear">&nbsp;</div>';
			$content .= '</div>';
		}
	}
	$content .= '</div>';
	
	return $content;

}