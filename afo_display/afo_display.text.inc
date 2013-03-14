<?php
// $Id$

/**
 * @file
 *
 * Theme text output.
 */

/**
 * Theme text function
 */
function theme_afo_display_text($text, $options = array()) {
	if(!is_string($text)) return null;
	
	// defaults
	if(!isset($options['size'])) $options['size'] = 'normal';
	if(!isset($options['output'])) $options['output'] = 'html';
	
	// special chars
	$text = str_replace(array('`', '´', '‘', '’', '‛', '❛', '❜', '', '', ''), "'", $text);
	$text = str_replace(array('', '', '“', '”', '„', '‟', '❝', '❞', '‶', '″', '‶', '〞', '〝', '〝', '〟'), '"', $text);
	$text = str_replace(array('', ''), '-', $text);
	
	// filter based on output setting
	switch($options['output']) {
		case 'plain':
			$text = strip_tags($text);
			$text = check_plain($text);
			break;
		
		default:
			// filter tags
			$allowed_tags = array(
				'a', 
				'b',
				'br',
				'em', 
				'i', 
				'p',
				'strong',
			);
			$text = filter_filter('process', 1, null, $text);
			$text = filter_xss($text, $allowed_tags);
			break;
			
	}

	// HTML corrector
	$text = filter_filter('process', 3, null, $text);
	
	// limit to size
	$limits = array(
		'tiny' => 75,
		'small' => 250,
		'normal' => 500,
		'large' => 1000,
	);
	if($options['size'] != 'full') {
		$limit = $limits[$options['size']];
		if(strlen($text) > $limit) {
			$text = substr($text, 0, $limit) . '...';	
		}
	}
	
	// read more link
	if(isset($options['read more'])) {
		$text .= ' ' . l(t('» read more'), $options['read more'], array('attributes'=>array('class'=>'moreLink'))); 
	}
	
	return $text;
}