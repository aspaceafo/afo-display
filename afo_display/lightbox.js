// $Id$

if(Drupal.jsEnabled) {

	/************************************ Settings **/
	function afo_slidelightbox_dimensions(op) {
		if(op == 'height') return $(window).height() - 25;
		else return $(window).width() - 50;
	}

	/************************************ Init functions **/
	$(document).ready(function(){
		afo_display_lightbox_initSlides();
		afo_display_lightbox_initLightBox();	
		afo_display_lightbox_initLaunch();
	});

	function afo_display_lightbox_initLightBox() {
		$(".afo-slidelightbox").dialog({
			autoOpen: false,
			bgiframe: true,
			resizable: false,
			height:afo_slidelightbox_dimensions('height'),
			width:afo_slidelightbox_dimensions(),
			modal: true,
			overlay: {
				backgroundColor: '#fff',
				opacity: 0.95
			},
		});
	}

	function afo_display_lightbox_initSlides() {
		// slides	
		var slides = $(".afo-slidelightbox .afo-slide");
		$(slides).show('fast', function() {
			$(slides).find("img").load(function() { 
				makeImageFullsize(this); 
				$(this).parents(".afo-slide:not(.current)").hide();
				// show launch button:
				$("a.afo-slidelightbox-launch").show();
			} );
		});
		
		// click function
		$(slides).click(function() {
			afo_slidelightbox_changeSlide(this, "next");
		});
	}	
	
	function afo_display_lightbox_initLaunch() {
		$("a.afo-slidelightbox-launch").hide().click(function() {
			var targetId = getURLParam($(this).attr('href'), 'target');
			$('#'+targetId).dialog('open');			
			return false;	
		});
	}
	
	/************************************ Display functions **/

	function makeImageFullsize(img) {
		var targetWidth = afo_slidelightbox_dimensions('width') - 50;
		var targetHeight = afo_slidelightbox_dimensions('height') - 50;

		//var imgWidth = $(img).width();
		//var imgHeight = $(img).height();
		var imgWidth = $(img).attr('width');
		var imgHeight = $(img).attr('height');
		
		if(imgWidth > 0 && imgHeight > 0) {
			if(targetHeight/imgHeight * imgWidth < targetWidth) {
				// scale by height:
				var newHeight = targetHeight;
				var newWidth = targetHeight/imgHeight * imgWidth;
			}	
			else {
				// scale by width:
				var newWidth = targetWidth;
				var newHeight = targetWidth/imgWidth * imgHeight;
			}
			
			$(img).width(newWidth);
			$(img).height(newHeight);
		}
	}
	
	function afo_slidelightbox_changeSlide(slide, op) {
		if(!$(slide).hasClass("afo-slide")) return false;
		if(op == "previous") {
			$(slide).toggle("slide", { direction: "right" }, 500, function() {
				var prevSlide = $(slide).prev('.afo-slide');
				if(prevSlide.length === 0) {
					prevSlides = $(slide).siblings('.afo-slide');
					prevSlide = $(prevSlides).eq(prevSlides.length - 1);
				}
				$(prevSlide).toggle("slide", { direction: "left" }, 500, function() {
				});
			});
			return false;
		}
		else {
			$(slide).toggle("slide", { direction: "left" }, 500, function() {
				var nextSlide = $(slide).next(".afo-slide");
				if(nextSlide.length === 0) {
					nextSlide = $(slide).siblings('.afo-slide').eq(0);
				}
				$(nextSlide).toggle("slide", { direction: "right" }, 500, function() {
				});
			});
			return false;
		}
	}
}