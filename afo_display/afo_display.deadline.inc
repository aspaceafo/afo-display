<?php
// $Id$

/**
 * @file
 *
 * Deadline display.
 */
 
/**
 * Theme deadline field.
 */
function theme_afo_display_deadline($timestamp) {
	if(!is_numeric($timestamp)) return false;
	
	if($timestamp < time()) {
		$notice = t('Already passed');
	}
	else {
		$days_left = floor(($timestamp - time()) / 60 / 60 / 24);
		if($days_left < 5) { 
			$notice = format_interval($timestamp-time(), 1) . ' left';
		}
	}
	
	$content .= '<div class="afo-deadline">';
	$content .= t('Deadline: ');
	$content .= t(format_date($timestamp, 'custom', variable_get('afo_display_dateformats_short', 'j M')));
	if($notice) $content .= ' <span class="afo-deadline-notice">'.$notice.'</span>';
	$content .= '</div>';
	return $content;
}