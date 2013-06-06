// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){

		$("#afo-exhibition-controls #afo-exhibition-slideview").click(function() {
			$("#afo-exhibition-controls a").removeClass('active');
			$(this).addClass('active');
			$(".afo-grid").slideUp('normal', function() {
				$(".afo-slideshow").slideDown('normal');
			});
			return false;
		});

		$("#afo-exhibition-controls #afo-exhibition-gridview").click(function() {
			$("#afo-exhibition-controls a").removeClass('active');
			$(this).addClass('active');
			$(".afo-slideshow").slideUp('normal', function() {
				$(".afo-grid").slideDown('normal');
			});
			return false;
		});
		
		$("#afo-exhibition-controls").show();


	});
	
}

