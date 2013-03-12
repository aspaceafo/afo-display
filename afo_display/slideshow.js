// $Id$

if(Drupal.jsEnabled) {

	/************************************ Init functions **/
	$(document).ready(function(){
		afo_display_slideshow_initSlides();
		afo_display_slideshow_play();
	});

	function afo_display_slideshow_initSlides() {
	
		// fade buttons to 0
		$(".afo-slide-controls").fadeTo('fast', 0).show();
	
		// SHOW INFO ON HOVER
		$(".afo-slideshow").hover(function() {
			$(this).find(".afo-slide-controls").fadeTo('fast', .8).find(".afo-slide-button").fadeTo('fast', .6);
		}, function() {	
			$(this).find(".afo-slide-controls").fadeTo('fast', 0);
		});

		// SHOW BUTTONS ON HOVER
		$(".afo-slideshow .afo-slide-controls .afo-slide-button").hover(
			function() {
				$(this).fadeTo('fast', 1);
			}, function() {
				$(this).fadeTo('fast', .6);
			}
		);
	/*
		$(".afo-slideshow .afo-slide .afo-slide-image img").click(function() {
			var current = $(this).parents('.afo-slide');
			afo_display_slideshow_switch(current, 'next');
			return false;
		});
	*/
		// NEXT BUTTON
		$(".afo-slideshow .afo-slide-controls .afo-slide-button-next").click(function() {
			afo_display_slideshow_pause(this, 'pause');
			var current = $(this).parents('.afo-slideshow').find(".current");
			afo_display_slideshow_switch(current, 'next');
			return false;
		});
		
		// PREVIOUS BUTTON
		$(".afo-slideshow .afo-slide-controls .afo-slide-button-previous").click(function() {
			afo_display_slideshow_pause(this, 'pause');
			var current = $(this).parents('.afo-slideshow').find(".current");
			afo_display_slideshow_switch(current, 'previous');
			return false;
		});

		// PLAY/PAUSE BUTTON
		$(".afo-slideshow .afo-slide-controls .afo-slide-button-play").click(function() {
			afo_display_slideshow_pause(this, 'toggle');
			return false;
		});

	}
	
	function afo_display_slideshow_play() {
		var slideShowTimer = setInterval("afo_display_slideshow_playAdvance()", 5000);
	}
	function afo_display_slideshow_playAdvance() {
		$('.afo-slideshow:not(.paused)').each(function () {
			var current = $(this).find('.current');
			afo_display_slideshow_switch(current, 'next');
		});	
	}
	function afo_display_slideshow_pause(button, op) {
		var slideshow = $(button).parents('.afo-slideshow');
		var pauseClass = 'paused';
		
		// change class
		if(op == 'pause') $(slideshow).addClass(pauseClass);
		else if(op == 'play') $(slideshow).removeClass(pauseClass);
		else $(slideshow).toggleClass(pauseClass);
		
		// change image
		$(slideshow).find('.afo-slide-controls .afo-slide-button-play').each(function() {
			if($(slideshow).hasClass(pauseClass)) {
				$(this).find('img.pause').hide();
				$(this).find('img.play').show();				
			}
			else {
				$(this).find('img.pause').show();
				$(this).find('img.play').hide();				
			}
		});

	}
	
	function afo_display_slideshow_switch(current, op) {
	
		if(op == 'previous') {
			/********** previous **/
			var prevSlide = $(current).prev('.afo-slide');
			if(prevSlide.length === 0) {
				prevSlides = $(current).siblings('.afo-slide');
				prevSlide = $(prevSlides).eq(prevSlides.length - 1);
			}
			var to = prevSlide;
		} 
		else {
			/********** next **/
			var nextSlide = $(current).next('.afo-slide');
			if(nextSlide.length === 0) {
				nextSlide = $(current).siblings('.afo-slide').eq(0);
			}
			var to = nextSlide;
		}

/*	
		if(op == 'previous') {
			/********** previous **/
/*
			$(current).toggle("slide", { direction: "right" }, 500, function() {
				var prevSlide = $(this).prev('.afo-slide');
				if(prevSlide.length === 0) {
					prevSlides = $(this).siblings('.afo-slide');
					prevSlide = $(prevSlides).eq(prevSlides.length - 1);
				}
				var to = prevSlide;
				/*
				$(prevSlide).toggle("slide", { direction: "left" }, 500, function() {
					$(this).addClass('current');
					return false;
				});
				*/
			//});		
//		} 
//		else {
			/********** next **/
			//$(current).toggle("slide", { direction: "left" }, 500, function() {
/*				var nextSlide = $(this).next('.afo-slide');
				if(nextSlide.length === 0) {
					nextSlide = $(this).siblings('.afo-slide').eq(0);
				}
				var to = nextSlide;
				/*
				$(nextSlide).toggle("slide", { direction: "right" }, 500, function() {
					$(this).addClass('current');
					return false;
				});
				*/
			//});			
//		}
		
		afo_display_slideshow_doswitch(current, to);
		return false;		
	}
	
	function afo_display_slideshow_doswitch(from, to) {
		$(from).fadeOut('normal', function() {
			$(this).removeClass('current');
			$(to).fadeIn('normal', function() {
				$(this).addClass('current');
				return false;
			});
		});
	}
	
}