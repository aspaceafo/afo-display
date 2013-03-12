    
    var theme = window.location.pathname;
    theme += '/galleria/themes/azur/galleria.azur.min.js';
    
    alert (theme);
    
    Galleria.loadTheme(theme);
    Galleria.run('#galleria', {
		showinfo: false,
		_showTooltip: false,
		_locale: {
			show_captions: 'i',
		}
 	}
	);