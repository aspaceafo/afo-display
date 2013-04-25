// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){
		afo_display_initViewOptions();
	});

	// init view options
	function afo_display_initViewOptions() {
		
		$(".afo-viewoptions img").hover(function() {
			afo_display_imgActive(this, 'toggle');
		}, function () {
			afo_display_imgActive(this, 'toggle');		
		});
	
	
		$(".afo-viewoptions a").click(function() {

			$(".afo-viewoptions a").removeClass('active');
			$(this).addClass('active');
			
		 	var href = $(this).attr('href');
		 	var targetId = getURLParam(href, 'target');
		 	var closeClass = getURLParam(href, 'close');
		 	
		 	if(targetId.length > 0 && closeClass.length > 0) {
			
			
				if(targetId == "afo-exhibition-slides") {
					$("."+closeClass).hide();
					$("div#galleria").show();
				}
				else{
					$("."+closeClass).hide();
					$("#"+targetId).show();
				}	
			}
		 	
			return false;		
		});
	}

	function afo_display_imgActive(img, op) {
		var act = "-active";
		var src = $(img).attr('src');
		var ext = src.substring(src.length - 4);
		src = src.substring(0, src.length - 4);
		if(src.substring(src.length - act.length) == act) var active = true;
		
		if(op == 'toggle') var yes = true;
		else if(active == false && op == 'add') var yes = true;
		else if(active == true && op == 'remove') var yes = true;
				
		if (yes == true && active == true) var newsrc = src.substring(0, src.length - act.length) + ext;
		else if (yes == true && active != true) var newsrc = src + act + ext;
		
		if(newsrc.length > 0) $(img).attr('src', newsrc);		
	}
	
}

