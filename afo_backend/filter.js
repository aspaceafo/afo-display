// $Id$

if(Drupal.jsEnabled) {
	$(document).ready(function(){

		var form = $('#edit-enable-filters').parents('form');
		var results_id = $(form).find('#edit-filters-results').val();
		var results = $(results_id);
		var ajaxUrl = $(form).find('#edit-filters-ajax-url').val();

		if($(form).find('#edit-submit-type').val() == 'button') {	
			// set submit button
			$(form).submit(function () {
				return false;
			});
			$(form).find('#edit-submit').click(function () {
				return false;
			});
			$(form).find('#edit-submit').mouseup(function () {
				afo_backend_filters_update_results(form, results, ajaxUrl);
				return false;
			});
		}
		else {
			// set filters to update list when changed
			$(form).find('.filter').change(function() {
				afo_backend_filters_update_results(form, results, ajaxUrl);
			});
		}
		
		// set reset button
		$(form).find('a.reset-filters').click(function () {
			$(form).find('.filter').val(0);
			$(form).find('.form-text').val('');
			afo_backend_filters_update_results(form, results, ajaxUrl);
			return false;
		});

	});
	
	// update results based on current selection
	function afo_backend_filters_update_results(form, results, ajaxUrl) {
		$('#filters-throbber').show();
		afo_backend_filters_loading(results, 'show');
		var values = afo_backend_filters_getSelections(form);
		$.ajax({
			type: 'POST',
			url: Drupal.settings.basePath + ajaxUrl,
			dataType: 'json',
			data: 'js=1' + values,
			success: function(data) {
				if(data.data) {
					$(results).html(data.data);
					afo_backend_filters_loading(results, 'hide');
				}
			}
		});	
	}
	
	
	// alter view when loading data
	function afo_backend_filters_loading(results, op) {
		if(op == 'show') {
			$(results).fadeTo('fast', .5);
			$('#filters-throbber').show();		
		} else {
			$(results).fadeTo('fast', 1);
			$('#filters-throbber').hide();
		}
	}
	
	// assemble values into args array
	function afo_backend_filters_getSelections(form) {
		var values = '';	
		var filters = $(form).find('.filter').each(function() {
			var name = afo_backend_filters_getName($(this).attr('name'));
			
			// get val differently depending element type
			if($(this).attr('type') == 'checkbox') {
				if($(this).attr('checked') == true) var val = $(this).val();
			}
			else var val = $(this).val();

			// add val to list string
			if(name && val) values += '&' + name + '=' + val;
		});
		return values;
	}
	
	// get filter name from element name
	function afo_backend_filters_getName(elemName) {
		var name = elemName.substring(elemName.indexOf('[') + 1, elemName.indexOf(']'));
		return name;
	}
}