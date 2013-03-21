// $Id$

if(Drupal.jsEnabled) {

	/************************************ Init functions **/
	$(document).ready(function(){
		afo_browsenav2_init_content();
	});

	function afo_browsenav2_init_content() {
		// hide non-image fields and add display functions
		var hideItems = $('#browse-results .item-image ~ div');
		
		$(hideItems).hide();
		
		$('#browse-results .item-image').hover(
			function () {
				$(this).siblings().show();
			}, 
			function () {
				$(this).siblings().hide();
			}
		);	
	}
	
}