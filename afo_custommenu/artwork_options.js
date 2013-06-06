// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){

		afo_custommenu_initArtworkOptions();

	});
	
}

/**
 * Init artwork options.
 */
function afo_custommenu_initArtworkOptions() {

	$("a.show-artwork-details").click(function() {
		$(".artwork-details").slideToggle('fast');
		return false;
	});

}