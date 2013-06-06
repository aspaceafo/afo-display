<?php
// $Id$

/**
 * @file
 *
 * Embed twitter feed functions.
 */

/**
 * Return embed script for twitter name
 */
function afo_profile_twitter_get_script($twitter_name) {
	$twitter_name = filter_xss($twitter_name, array());
	$twitter_name = check_plain($twitter_name);
	
	if(!$twitter_name) return null;
	
	$script = "
	<script src='http://widgets.twimg.com/j/2/widget.js'></script>
	<script>
	new TWTR.Widget({
	  version: 2,
	  type: 'profile',
	  rpp: 3,
	  interval: 6000,
	  width: 460,
	  height: 200,
	  theme: {
		shell: {
		  background: '#ffffff',
		  color: '#000000'
		},
		tweets: {
		  background: '#ffffff',
		  color: '#000000',
		  links: '#003877'
		}
	  },
	  features: {
		scrollbar: false,
		loop: false,
		live: false,
		hashtags: true,
		timestamp: true,
		avatars: false,
		behavior: 'all'
	  }
	}).render().setUser('$twitter_name').start();
	</script>
	";
	
	return $script;	
}