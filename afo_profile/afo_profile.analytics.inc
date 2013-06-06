<?php
// $Id$

/**
 * @file
 *
 * Adds google analytics features to member user profiles.
 */

/**
 * Implementation of hook_footer() to insert Javascript at the end of the page.
 */
function afo_profile_embed_analytics($id) {
	if (empty($id)) return null;

    // Site search tracking support.
    /*
    $url_custom = '';
    if (module_exists('search') && variable_get('googleanalytics_site_search', FALSE) && arg(0) == 'search' && $keys = search_get_keys()) {
      $url_custom = drupal_to_js(url('search/'. arg(1), array('query' => 'search='. $keys)));
    }
	*/
	
    // If this node is a translation of another node, pass the original
    // node instead.
    if (module_exists('translation') && variable_get('googleanalytics_translation_set', 0)) {
      // Check we have a node object, it supports translation, and its
      // translated node ID (tnid) doesn't match its own node ID.
      $node = menu_get_object();
      if ($node && translation_supported_type($node->type) && isset($node->tnid) && ($node->tnid != $node->nid)) {
        $source_node = node_load($node->tnid);
        $languages = language_list();
        $url_custom = drupal_to_js(url('node/'. $source_node->nid, array('language' => $languages[$source_node->language])));
      }
    }

    // Track access denied (403) and file not found (404) pages.
    if (function_exists('drupal_get_headers')) {
      $headers = drupal_get_headers();
      if (strstr($headers, 'HTTP/1.1 403 Forbidden')) {
        // See http://www.google.com/support/analytics/bin/answer.py?answer=86927
        $url_custom = '"/403.html?page=" + document.location.pathname + document.location.search + "&from=" + document.referrer';
      }
      elseif (strstr($headers, 'HTTP/1.1 404 Not Found')) {
        $url_custom = '"/404.html?page=" + document.location.pathname + document.location.search + "&from=" + document.referrer';
      }
    }


    // Build tracker code for footer -- cleaned up for new google analytics
    $script .= "var _gaq = _gaq || [];";
	$script .= "_gaq.push(['_setAccount', ". drupal_to_js($id) ."]);";
  	$script .= "_gaq.push(['_trackPageview']);";
	$script .= "(function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();"; //redownload google ga code, I don't know if I trust the analytics plugin....
 
    /* Old code
    $script = 'try{';
    $script .= 'var pageTracker = _gat._getTracker('. drupal_to_js($id) .');';
    if (!empty($segmentation)) {
      $script .= $segmentation;
    }
    if (!empty($codesnippet_before)) {
      $script .= $codesnippet_before;
    }
    $script .= 'pageTracker._trackPageview('. $url_custom .');';
    if (!empty($codesnippet_after)) {
      $script .= $codesnippet_after;
    }
    $script .= '} catch(err) {}';
	*/
	
	
    drupal_add_js($script, 'inline', 'footer');
  
}