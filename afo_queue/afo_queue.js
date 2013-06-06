// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){
		afo_queue_initQueueBox();
		afo_queue_initAddLinks();
		afo_queue_initDraggable();
		afo_queue_initDroppable();
	});

	/**
	 * Init add links
	 */
	function afo_queue_initDraggable() {
		// the dragged item
		$('.queueable').draggable({ 
			helper: function() {
				if($(this).hasClass('hit-area')) {
					return $(this).parents('.expanded-view').find('.item-image-expanded img').clone();
				}
				else {
					return $(this).clone();
				}
			}, 
			zIndex: 2000,
			
			appendTo: "body",
			
		});
		// the queued items
		$('#afo-queue .queueable').draggable("destroy").draggable({
			helper: 'original',
			zIndex: 2000,
			stop: function(event, ui) {
				var itemData = afo_queue_getDataFromClass(ui.helper);
				if(itemData.id && itemData.type) {
					afo_queue_editItem(itemData.id, itemData.type, 'remove');
				}
				ui.helper.remove();
			}
		});
	}
	/**
	 * Init droppable
	 */
	function afo_queue_initDroppable() {
		// the drop zone
		$('#afo-queue-box, #afo-queue-button').droppable({
			accept: '.queueable',
			tolerance: 'touch',
			hoverClass: 'active-highlight',
			drop: function(event, ui) {
				var itemData = afo_queue_getDataFromClass(ui.draggable);
				if(itemData.id && itemData.type) {
					afo_queue_editItem(itemData.id, itemData.type, 'add');
				}
			}
		});	
	}
		
	/**
	 * Init add links
	 */
	function afo_queue_initAddLinks() {
		$('a.afo-queue-add').click(function() {
			var href = $(this).attr('href');
			var id = getURLParam(href, 'id');
			var type = getURLParam(href, 'type');

			afo_queue_editItem(id, type, 'add');
						
			return false;
		});
	}

	/**
	 * Init box
	 */
	function afo_queue_initQueueBox() {
	
		afo_queue_updateEmpty();
	
		// open button
		$('a.afo-queue-toggle, #afo-queue-button').click(function() {
			afo_queue_toggleOpen();
			return false;
		});

		// clear button
		$('a.afo-queue-clear').click(function() {
			afo_queue_editItem(0, 'node', 'clear');
			return false;
		});
		
		// scroll buttons
		$('.afo-queue-scroll img').fadeTo('fast', .3);
		$('.afo-queue-scroll').hover(function() {
			$(this).find('img').fadeTo('fast', 1);
			if($(this).hasClass('afo-queue-scroll-left')) var dir = 'left';
			else var dir = 'right';
			window.queueScroll = setInterval(function() { afo_queue_slideQueue(dir) }, 10);
		}, function() {
			$(this).find('img').fadeTo('fast', .3);
			window.clearInterval(queueScroll);
		});
		
	}

	/**
	 * Update queue based on full/empty
	 */
	function afo_queue_updateEmpty() {
		var inner = $('#afo-queue-inner');
		var box = $('#afo-queue-box');
		
		if($(inner).hasClass('empty')) {
			if(!$(box).hasClass('closed')) afo_queue_toggleOpen()
		}
	}

	/**
	 * Update queue count
	 */
	function afo_queue_updateCount(count) {
		$('.queue-count').text(count);
	}
	
	/**
	 * Toggle queue open / closed
	 */
	function afo_queue_toggleOpen() {
		$('#afo-queue-box').slideToggle('fast', function() {

			$('#afo-queue-box').toggleClass('closed');

			var openVal = 'yes';
			if($('#afo-queue-box').hasClass('closed')) openVal = 'no';

			$.ajax({
				type: 'POST',
				url: afo_queue_ajax_url('op'),
				dataType: 'json',
				data: 'op=open&value='+openVal
			});
			
		});
	}
	
	/**
	 * Rebuild queue
	 */
	function afo_queue_rebuild() {
		$.ajax({
			type: 'POST',
			url: afo_queue_ajax_url('build'),
			dataType: 'json',
			success: function(data) {
				afo_queue_replace(data.queue);
				afo_queue_updateCount(data.count);
			}
		});				
	}
	/**
	 * Replace queue
	 */
	function afo_queue_replace(html) {
		$('#afo-queue').html(html);
		afo_queue_initDraggable();
		//afo_queue_updateEmpty();
	}
	
	/**
	 * Slide queue
	 */
	function afo_queue_slideQueue(dir) {
		var q = $('#afo-queue');
		var newOffset = $(q).position();
		var jump = 3;
		
		if(dir == 'left') {
			newOffset.left += jump; 
			if(newOffset.left < 20) var moveAllowed = true;
		}
		else if(dir == 'right') {
			newOffset.left -= jump;
			if(newOffset.left > ($(window).width() - $(q).width() - 65)) var moveAllowed = true;
		}
		
		if(moveAllowed === true) $('#afo-queue').css('left', String(newOffset.left+'px'));
	}
	
	/**
	 * Add item to queue
	 */
	function afo_queue_editItem(id, type, op) {
		afo_queue_updateCount('...');
		$.ajax({
			type: 'POST',
			url: afo_queue_ajax_url('edit'),
			data: 'id='+id+'&type='+type+'&op='+op,
			dataType: 'json',
			success: function(data) {
				if(data.result) {
					afo_queue_updateCount(data.count);
					if(data.queue) afo_queue_replace(data.queue);
					else afo_queue_rebuild();
				}
			}
		});		
	}
	
	/**
	 * Translate queue data class into values
	 */
	function afo_queue_getDataFromClass(obj) {
		var classList =$(obj).attr('class').split(' ');
		var itemData = new Object;
		$.each(classList, function(index, c) {
			if (c.substr(0, 6) == 'queue-') {
				itemData.type = c.substr(6, 4);
				itemData.id = c.substr(11);
			}
		});
		return itemData;
	}

	/**
	 * Get ajax queue url
	 */
	function afo_queue_ajax_url(op) {
		var url = Drupal.settings.basePath + 'queue/' + op;
		return url;
	}
}