// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){

		afo_portfolio_initFolders();
		afo_portfolio_initShowLinks();

	});
	
}

// initialize folders
function afo_portfolio_initFolders() {
	var folders = $("div.portfolio-folder");
	
	// hide descriptions
	$(folders).find(".portfolio-description").hide();
	// hide other images
	$(folders).find(".portfolio-other-thumbnails").hide();
	
	// make title show more
	$(folders).find("a.show-folder").click(function() {
		afo_portfolio_showfolder(this);
		return false;
	});
}

// show folder
function afo_portfolio_showfolder(link) {
	var folder = $(link).parents("div.portfolio-folder")
	// show description
	$(folder).find(".portfolio-description").eq(0).slideDown("fast");
	// show other images
	$(folder).find(".portfolio-other-thumbnails").eq(0).slideDown("fast");	
	// change arrow
	afo_portfolio_changearrow($(link).find("img"), "down");
	// change link function
	$(link).unbind('click');
	$(link).click(function() {
		afo_portfolio_hidefolder(this);
		return false;
	});
}

// hide folder
function afo_portfolio_hidefolder(link) {
	var folder = $(link).parents("div.portfolio-folder")
	// hide description
	$(folder).find(".portfolio-description").eq(0).slideUp("fast");	
	// hide other images
	$(folder).find(".portfolio-other-thumbnails").eq(0).slideUp("fast");	
	// change arrow
	afo_portfolio_changearrow($(link).find("img"), "right");
	// change link function
	$(link).unbind('click');
	$(link).click(function() {
		afo_portfolio_showfolder(this);
		return false;
	});
}

// toggle arrow
function afo_portfolio_changearrow(arrow, op) {
	var src = $(arrow).attr("src");
	if(op=="down") {
		src = src.replace("Arrow-Right", "Arrow-Down");
	}
	else if(op=="right") {
		src = src.replace("Arrow-Down", "Arrow-Right");
	}
	$(arrow).attr("src", src);
}

// init show/hide portfolio links
function afo_portfolio_initShowLinks() {
	$("a.afo-portfolio-show").click(function () {
		var portfolio = $('#'+this.id.substring(5));
		$(portfolio).slideToggle('fast', function() {
			var portfolioOffset = $(this).offset();
			$(window).scrollTop(portfolioOffset.top);
		});
		return false;
	});
}