// $Id$

/**
 * AFO Related JS functions.
 */
if(Drupal.jsEnabled) {

	$(document).ready(function() {
	
		$(".block-afo_related .afo-related .related-item img").fadeTo('fast', .2);
		$(".block-afo_related .afo-related .related-item img").hover(
			function() {
				$(this).fadeTo('fast', 1);		
			},
			function() {
				$(this).fadeTo('fast', .1);		
			}
		);
		
	});
2	
}