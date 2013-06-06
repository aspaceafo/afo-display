// $Id$

if(Drupal.jsEnabled) {
	$(document).ready(function(){

		afo_import_initLinks();
		
		$('a.start-import').click(function() {
			var startId = $('input#edit-import-start').val();
			if(startId > 0) {
				afo_import_do2(startId);
			}
			return false;
		});

	});
	
	// init import page links
	function afo_import_initLinks() {
		
		// import links
		$('a.import-do').click(function() {
			var aId = getURLParam($(this).attr('href'), 'a_id');
			afo_import_do_import(aId, false);
			return false;
		});

		// preview links
		$('a.import-preview').click(function() {
			
			var a_id = getURLParam($(this).attr('href'), 'a_id');
			var sendData = 'op=preview&a_id='+a_id;
			var results = $(this).siblings('.import-preview-results');

			$(results).html('loading...');
			
			$.ajax({
				type: 'POST',
				url: afo_import_ajaxurl(),
				dataType: 'json',
				data: sendData,
				success: function(data) {
					$(results).html(data.preview);
				}
			});
			
			return false;
		});
		
		// batch import link
		$('a.import-batch').click(function() {
		
			afo_import_batch_import_next();
			return false;
		
		});
		
	}
	
	// ajax do import
	function afo_import_do_import(aId, batch) {
		
		var tableRow = $('.afo-import-table tr#a_id-'+aId);	
		var sendData = 'op=import&a_id='+aId;
		var results = $(tableRow).find('.import-do-results');

		$(results).html('importing...');
		
		$.ajax({
			type: 'POST',
			url: afo_import_ajaxurl(),
			dataType: 'json',
			data: sendData,
			success: function(data) {
				$(results).html(data.result);
				if(data.imported == true) {
					$(tableRow).addClass('in-db');
				}
				if(batch == true) afo_import_batch_import_next();
			}
		});
		
	}
	
	// ajax do import 2
	function afo_import_do2(aId) {

		$('#import-status').html('importing...');
		var sendData = 'op=import&a_id='+aId;
		
		$.ajax({
			type: 'POST',
			url: afo_import_ajaxurl(),
			dataType: 'json',
			data: sendData,
			success: function(data) {
				$('#import-status').html('finished');
				if(data.result) {
					$('#import-results').prepend(data.summary+'<br />');
					var nextId = Number(aId) + 1;
					if(nextId < $('input#edit-import-end').val()) {
						afo_import_do2(nextId);
					}
					else {
						alert('Finished importing!');
					}
				}
			}
		});
		
	}
	
	// batch helper function
	function afo_import_batch_import_next() {
		var stillSelected = $('.afo-import-table input:checked:first.import-selected').each(function() {		
			var aId = $(this).val();
			
			// do the import
			afo_import_do_import(aId, true)
			
			// uncheck this one
			$(this).attr('checked', false);	
		});	

		if(stillSelected.length == 0) {
			// announce finished
			alert('Finished importing!');
		}
	}
	
	// helper function: return ajax link
	function afo_import_ajaxurl() {
		return Drupal.settings.basePath + 'afo-import/do';
	}
	
}