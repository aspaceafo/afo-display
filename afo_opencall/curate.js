// $Id$

if(Drupal.jsEnabled) {
	$(document).ready(function(){

		// init ignore check boxes
//		$(".afo-opencall-submission .listing-ignore input:not(:checked)").attr('checked', false);
//		$(".afo-opencall-submission .listing-ignore input:checked").attr('checked', true);
		$(".afo-opencall-submission .listing-ignore input").change(function () {
			var subId = $(this).parents('.afo-opencall-submission').find('input.submission_id').val();
			var ignoreUrl = Drupal.settings.basePath + 'calls/ignore';
			$.ajax({
				type: 'POST',
				url: ignoreUrl,
				dataType: 'json',
				data: "js=1&submission_id="+subId,
				success: function(data) {
					//alert(data.result);
				}
			});
		});

	});
}