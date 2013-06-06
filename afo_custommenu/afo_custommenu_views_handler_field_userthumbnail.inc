<?php
/**
 * Field handler for portfolio thumbnail.
 */
class afo_custommenu_views_handler_field_userthumbnail extends views_handler_field {
  /**
   * Override init function to provide generic option to link to user.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
	$this->additional_fields['uid'] = 'uid';
  }

  /**
   * Define default values for options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['imagecache_setting'] = array('default' => 'artwork_thumbnail', 'translatable' => FALSE);
    return $options;
  }

  /**
   * Extend the field's basic options with more image specific options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['imagecache_setting'] = array(
      '#type' => 'textfield',
      '#title' => t('Imagecache Preset'),
      '#default_value' => isset($this->options['label']) ? $this->options['label'] : '',
      '#description' => t('The Imagecache preset to use when displaying this field.'),
    );

  }
  
  function render_artworkdata($data, $values) {
	
		$imagecache_preset = $this->options['imagecache_setting'];
		$uid = $values->{$this->field_alias};

		return afo_custommenu_user_thumbnail($uid, $imagecache_preset);

  }

  function render($values) {
    return $this->render_artworkdata(check_plain($values->{$this->field_alias}), $values);
  }
}