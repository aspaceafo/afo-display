// $Id$

/**
 * Preview user portfolio.
 */
if(Drupal.jsEnabled) {

	function afo_cv_edit(entryid) {

		$("#content").append("<div id='afo_cv_edit_form'><div id='afo_cv_edit_form-inner'></div></div");

		var editbox = $("#afo_cv_edit_form");
		var editinner = $("#afo_cv_edit_form-inner");

		editinner.html("Loading form...");
		editinner.load(Drupal.settings.basePath+"afo-cv/edit-entry-form/"+entryid);

		editbox.dialog({
			bgiframe: true,
			resizable: false,
			height:500,
			width:800,
			modal: true,
			overlay: {
				backgroundColor: '#fff',
				opacity: 0.85
			},
		});
		
	}

}