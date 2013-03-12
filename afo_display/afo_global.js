// $Id$

if(Drupal.jsEnabled) {

	/************************************ Init functions **/
	$(document).ready(function(){
		afo_display_initHomePage();
		afo_display_initOpeners();
		afo_display_initCheckboxControls();
		afo_display_initRegistration();

		var t = setTimeout('afo_display_fadeMessages()', 5000);
	});
	
	/** init home page **/
	function afo_display_initHomePage() {
		// check if front page
		if($("body").hasClass("front")) {
			
			var header = $("#header");
			$(header).fadeTo("fast", .3);
			
			$("#header").hover(
				function() {
					$(header).fadeTo('fast', 1);
				}, function () {
					$(header).fadeTo('fast', .3);
				}
			);
			
		}
	}
	
	/** init openers **/
	function afo_display_initOpeners() {
		$("a.opener").click(function () {
			var href = $(this).attr('href');
			var target = getURLParam(href, 'target');
			var noscroll = getURLParam(href, 'noscroll');
			if(target.length > 0) {
				$(target).slideToggle('fast', function () {
					if(noscroll != 1) {
						var offs = $(target).offset();
						$(window).scrollTop(offs.top);
					}
				});
			}
			return false;
		});
	}
	
	/** initialize checkbox control links **/ 
	function afo_display_initCheckboxControls() {
		$('.checkboxControl').click(function() {
			var href = $(this).attr('href');
			var target = getURLParam(href, 'target');
			var op = getURLParam(href, 'op');
			
			var targets = $('input'+target);
			if(op == 'checkAll') {
				$(targets).attr('checked', 'checked');
			}
			else if(op == 'checkNone') {
				$(targets).attr('checked', '');
			}
		
			return false;
		});
	}
	
	/** fade messages block **/
	function afo_display_fadeMessages() {
		if(!$('body').hasClass('admin-user')) $('.status').fadeOut('slow');
	}
	
	/** init registration functionality **/
	function afo_display_initRegistration() {
		$("#user-register input[name='user_roles']").change(function() {
			var sel = $(this).val();
			var box = $("#edit-profile-curator-credentials").parents(".form-item");
			if(sel == 6) $(box).show();
			else $(box).hide();
		});
	}
	
	/** parse href utility **/
	function getURLParam(strHref, strParamName) {
		var strReturn = "";
		var bFound=false;
		
		var cmpstring = strParamName + "=";
		var cmplen = cmpstring.length;
		
		if ( strHref.indexOf("?") > -1 ){
		var strQueryString = strHref.substr(strHref.indexOf("?")+1);
		var aQueryString = strQueryString.split("&");
		for ( var iParam = 0; iParam < aQueryString.length; iParam++ ){
		  if (aQueryString[iParam].substr(0,cmplen)==cmpstring){
			var aParam = aQueryString[iParam].split("=");
			strReturn = aParam[1];
			bFound=true;
			break;
		  }
		  
		}
		}
		if (bFound==false) return null;
		return strReturn;
	}		
}