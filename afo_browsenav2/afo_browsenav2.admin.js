// $Id$

if(Drupal.jsEnabled) {

	/************************************ Init functions **/
	$(window).load(function () {
		$('#content-area .pager-next a').each(function() {
			var dest = $(this).attr("href");
			window.location = "http://local-artists.org"+dest;
		});
	});
	
}