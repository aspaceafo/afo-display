// $Id$

/**
 * AFO Newsfeed JS functions.
 */
if(Drupal.jsEnabled) {

	$(document).ready(function() {
		afo_newsfeed_initFeed();
	});
	
	/**
	 * Initialize newsfeed items
	 */
	function afo_newsfeed_initFeed() {
		$(".afo-newsfeed a.thumbnail").hover(
			function () {
				$(this).parents('.item').find('.details').show();
			}, 
			function () {
				$(this).parents('.item').find('.details').hide();
			}
		);	
	}
	
}