<?php
// $Id$

/**
 * @file
 *
 * AFO Column form element
 */

/**
 * Theme column function
 */
function theme_afo_column($element) {

	// classes
	if(isset($element['#position'])) $classes[] = 'position-'.$element['#position'];
	if(isset($classes)) $addClasses = implode(' ', $classes);
	
	// width
	if(isset($element['#width'])) $width = 'style="width:'.$element['#width'].'"';
	
	// compile content
	$content .= '<div class="afo-column '.$addClasses.'" '.$width.'><div class="afo-column-inner">';

	if($element['#title']) $content .= '<h2>'.$element['#title'].'</h2>';

	$content .= $element['#children'];

	$content .= '</div></div>';
	
	return $content;
}