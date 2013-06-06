// $Id$

if(Drupal.jsEnabled) {
	$(document).ready(function(){

		var form = $('#afo-opencall-callspage-buildfilters');
		var results = $('#afo-opencalls-results');
		
		// set filters to update list when changed
		$(form).find('.filter').change(function() {
			afo_opencall_filters_update_results(form, results);
		});
		
		// set reset button
		$(form).find('a.reset-filters').click(function () {
			$(form).find('.filter').val(0);
			afo_opencall_filters_update_results(form, results);
			return false;
		});

	});
	
	// update results based on current selection
	function afo_opencall_filters_update_results(form, results) {
		$('#filters-throbber').show();
		afo_opencall_filters_loading(results, 'show');
		var values = afo_opencall_filters_getSelections(form);
		$.ajax({
			type: 'POST',
			url: Drupal.settings.basePath + 'calls/ajax',
			dataType: 'json',
			data: 'js=1' + values,
			success: function(data) {
				if(data.data) {
					$(results).html(data.data);
					afo_opencall_filters_loading(results, 'hide');
				}
			}
		});	
	}
	
	
	// alter view when loading data
	function afo_opencall_filters_loading(results, op) {
		if(op == 'show') {
			$(results).fadeTo('fast', .5);
			$('#filters-throbber').show();		
		} else {
			$(results).fadeTo('fast', 1);
			$('#filters-throbber').hide();
		}
	}
	
	// assemble values into args array
	function afo_opencall_filters_getSelections(form) {
		var values = '';	
		var filters = $(form).find('.filter').each(function() {
			var name = afo_opencall_filters_getName($(this).attr('name'));
			
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
	function afo_opencall_filters_getName(elemName) {
		var name = elemName.substring(elemName.indexOf('[') + 1, elemName.indexOf(']'));
		return name;
	}
}