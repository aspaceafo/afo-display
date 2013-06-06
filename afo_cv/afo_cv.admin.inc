<?php
// $Id$

/**
 * Admin callbacks for AFO CV module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_cv_admin_settings() {

	
	return system_settings_form($form);

}

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions for existing venues.
 */
function afo_cv_venue_autocomplete($string = '') {
  $matches = array();
  if ($string) {
    $result = db_query_range("SELECT name FROM {afo_cv_venues} WHERE LOWER(name) LIKE LOWER('%%%s%%')", $string, 0, 10);
    while ($venue = db_fetch_object($result)) {
      $matches[$venue->name] = check_plain($venue->name);
    }
  }

  drupal_json($matches);
}