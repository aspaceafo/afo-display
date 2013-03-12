<?php
// $Id$

/**
 * @file
 *
 * AFO Content Box form element
 */

/**
 * Theme afo_contentbox function
 */
function theme_afo_contentbox($element) {

	$element['#attributes']['class'] .= ' afo-contentbox';
	
	$attribs = drupal_attributes($element['#attributes']);

	// compile content
	$content .= '<div '.$attribs.'><div class="afo-contentbox-inner">';

	if($element['#title']) $content .= '<h2>'.$element['#title'].'</h2>';

	$content .= $element['#value'];

	$content .= $element['#children'];

	$content .= '<div class="horiz-clear">&nbsp;</div>';
	$content .= '</div></div>';
	
	return $content;
}