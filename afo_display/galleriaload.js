//try wrapping in document ready


$(document).ready(function() {
	Galleria.loadTheme('http://local-artists.org/prod/sites/all/modules/custom/afo_display/galleria/themes/azur/galleria.azur.min.js');
	Galleria.run('#galleria', {
		showinfo: false,
		_showTooltip: false,
		maxScaleRatio: '1',
		_locale: {
			show_captions: ' ',
				
		},


		extend: function(options) {
	        var gallery = this; // this is the gallery instance
	        this.bind(Galleria.IMAGE, function(e) {
	            var current = gallery.getData(gallery.getIndex());
	            var currImg = current.original;
	            var imgclass = $(currImg).attr('class'); //imgclass retreives current image's orignal classes
				imgclass += ' ui-draggable';
	            console.log(imgclass);
				$(e.imageTarget).addClass(imgclass) //adds current image's original classes to current image 
			 
			    afo_queue_initQueueBox();
				afo_queue_initAddLinks();
				afo_queue_initDraggable();
				afo_queue_initDroppable();
				console.log('afo_queue_init');
			 
	        });
	    },		
	});


	
});	