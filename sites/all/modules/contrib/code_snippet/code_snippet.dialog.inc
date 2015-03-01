<?php

/**
 * Handles the Wysiwyg dialog windows.
 */
function code_snippet_wysiwyg_dialogue($editor, $textarea, $node, $csid, $content = NULL) {
  $path = drupal_get_path('module', 'code_snippet');

  //check if implementation for this editor has been done
  if (file_exists($path .'/code_snippet_'. $editor .'.js')) {
    drupal_add_js(
      $path .'/code_snippet_'. $editor .'.js',
      array(
        'type' => 'file',
        'group' => JS_DEFAULT,
        'weight' => -100
      )
    );
  }
  else {
    die('Not implemented yet');
  }

  //if editor is tinymce
  if ($editor == 'tinymce') {
    drupal_add_js(
      'sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce_popup.js',
      array(
        'type' => 'file',
        'group' => JS_LIBRARY,
        'weight' => 0
      )
    );
  }

  $messages = theme('status_messages');
  $content = drupal_render($content);
  $js = drupal_get_js();
  drupal_add_css(drupal_get_path('module', 'code_snippet') .'/plugins/snippet/snippet.css');
  $styles = drupal_get_css();
  // Only call on the first load
  $output = <<< END
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Insert Code Snippet</title>
  {$styles}
  {$js}
</head>
<body onload="initLoader(false)">
  {$messages}
  {$content}
</body>
</html>
END;

  echo $output;
  exit;
}

/**
 * Creates the main window HTML interface used to
 * control the popup. The $_GET parameters do not
 * contain any clues for the required logic.
 */
function code_snippet_wysiwyg_plugin_loader() {
  $path = drupal_get_path('module', 'code_snippet');
  $editor = arg(2) ? arg(2) : 'textarea';
  if (file_exists($path .'/code_snippet_'. $editor .'.js')) {
    drupal_add_js($path .'/code_snippet_'. $editor .'.js');
  }
  else {
    die('Not implemented yet');
  }
  if ($editor == 'tinymce') {
    drupal_add_js('sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce_popup.js');
  }
  $js = drupal_get_js();
  $output = <<< END
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Insert Code Snippet</title>
  {$js}
</head>
<body onload="initLoader(true)">
</body>
</html>
END;

  echo $output;
  exit;
}

/**
 * Output main img_assist interface HTML.
 */
function code_snippet_wysiwyg_page_loader($editor, $textarea, $nid, $csid) {
  $js = drupal_get_js();
  $action = arg(5) == 'list' ? 'list' : 'form';
  $node = $nid == 0 ? null : node_load($nid);
  $snippet = NULL;
  // Only show list we we have at least 1 code snippet.
  if ($action == 'list') {
    $query = db_query("SELECT count(*) FROM {code_snippet}");
    $count = $query->fetchField();

    if ($count > 0) {
      $action = 'list';
    }
    else {
      $action = 'form';
    }
  }

  if ($action == 'form' && !empty($csid) && is_numeric($csid)) {
    // edit
    $query = db_query("SELECT * FROM {code_snippet} WHERE csid = :csid", array(':csid' => $csid));
    $snippet = $query->fetch();
    //$snippet = db_fetch_object(db_query("SELECT * FROM {code_snippet} WHERE nid = %d AND csid = %d", $node->nid, $csid));
  }

  if ($action == 'form') {
    $output = drupal_get_form('code_snippet_snippet_form', $editor, $textarea, $node, $snippet);
  }
  else {
    $output = drupal_get_form('code_snippet_list_snippet_form', $editor, $textarea, $node, $snippet);
  }
  code_snippet_wysiwyg_dialogue($editor, $textarea, $node, $csid, $output);
}

/**
 * Common form elements shared by the edit / list pages.
 */
function _code_snippet_common_form(&$form_state, $editor, $textarea, $node, $snippet) {
  require_once drupal_get_path('module', 'geshifilter') .'/geshifilter.inc';

  $form = array();
  $form['#id'] = 'code-snippet-snippet-form';
  $form['#node'] = $node;
  $form['#editor'] = $editor;
  $form['#textarea'] = $textarea;
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => is_null($node) ? 0 : $node->nid
  );
  $form['csid'] = array(
    '#type' => 'value',
    '#value' => $snippet ? $snippet->csid : '',
  );

  $default_values = _code_snippet_normalise_default_values($_GET);
  $form['tagstyle'] = array(
    '#type' => 'radios',
    '#title' => t('Snippet display style'),
    '#default_value' => $default_values['tagstyle'],
    '#options' => _code_snippet_tag_styles(),
  );
  $form['lineNumbering'] = array(
    '#type' => 'textfield',
    '#title' => t('Numbering / Highlight interval'),
    '#id' => 'edit-lineNumbering',
    '#default_value' => $default_values['lineNumbering'],
  );
  $form['numberStart'] = array(
    '#type' => 'textfield',
    '#title' => t('Line numbering start'),
    '#id' => 'edit-numberStart',
    '#default_value' => $default_values['numberStart'],
  );
  $form['buttons'] = array(
    '#prefix' => '<div class="mceActionPanel">',
    '#suffix' => '</div>',
    '#weight' => 999,
  );
  $default_values['editor'] = $editor;
  $default_values['textarea'] = $textarea;
  $default_values['nid'] = is_null($node) ? 0 : $node->nid;
  $default_values['cid'] = empty($snippet->csid) ? 0 : $snippet->csid;

  /*
  if (count(code_snippet_node_snippets($node->nid))) {
    $list_url = _code_snippet_dialog_url($default_values, 'list');
    $form['buttons']['list'] = array(
      '#type' => 'markup',
      '#value' => '<input class="updateButton" type="button" id="list" name="list" value="'. t('List') .'" onclick="window.location.href = \''. $list_url .'\' ;" />',
    );
  }
  */

  $list_url = _code_snippet_dialog_url($default_values, 'list');
  $form['buttons']['list'] = array(
    '#type' => 'markup',
    '#markup' => '<input class="updateButton" type="button" id="list" name="list" value="'. t('List') .'" onclick="window.location.href = \''. $list_url .'\' ;" />',
  );

  $default_values['cid'] = 0;
  $new_url = _code_snippet_dialog_url($default_values);
  $form['buttons']['new'] = array(
    '#type' => 'markup',
    '#markup' => '<input class="updateButton" type="button" id="new" name="new" value="'. t('New') .'" onclick="window.location.href = \''. $new_url .'\' ;" />',
  );
  $form['buttons']['cancel'] = array(
    '#type' => 'markup',
    '#markup' => '<input type="button" id="cancel" name="cancel" value="'. t('Cancel') .'" onclick="cancelAction();" />',
  );
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Insert'),
    '#id' => 'edit-submit',
    '#attributes' => array('class' => array('updateButton')),
  );

  return $form;
}

/**
 * Provides the list code snippets dialog form.
 */
function code_snippet_list_snippet_form($form, &$form_state, $editor, $textarea, $node, $snippet) {
  $form2 = _code_snippet_common_form($form_state, $editor, $textarea, $node, $snippet);
  unset($form2['buttons']['list']);
  $form2['buttons']['submit']['#submit'] = array('code_snippet_list_snippet_submit');

  $options = code_snippet_node_snippets();

  $form2['snippets'] = array(
    '#type' => 'select',
    '#title' => t('Existing code snippets'),
    '#id' => 'edit-snippets',
    '#options' => $options,
    '#default_value' => $snippet ? $snippet->csid : key($options),
  );
  $form2['#theme'] = 'code_snippet_dialog_list_form';
  return array_merge($form, $form2);
}

/**
 * Provides the new / edit code snippets dialog form.
 */
function code_snippet_snippet_form($form, &$form_state, $editor, $textarea, $node, $snippet) {
  $form2 = _code_snippet_common_form($form_state, $editor, $textarea, $node, $snippet);
  unset($form2['buttons']['new']);
  $form2['buttons']['submit']['#submit'] = array('code_snippet_snippet_submit');

  $form2['#theme'] = 'code_snippet_dialog_form';

  $form2['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title *'),
    '#description' => t('The title of the code snippet. This is the title used in the Code Snippet list.'),
    '#default_value' => $snippet ? $snippet->title : '',
    '#required' => TRUE,
  );

  $form2['snippet'] = array(
    '#type' => 'textarea',
    '#title' => t('Code snippet *'),
    '#description' => t('Type your codes in this textarea. Delimiters must not be included. Example for PHP codes, ?php ?> is not required.'),
    '#default_value' => $snippet ? $snippet->snippet : '',
    '#rows' => 14,
    '#cols' => 30,
    '#required' => TRUE,
  );

  // the source code language field
  $form2['type'] = array(
    '#type' => 'select',
    '#title' => t('Syntax highlighting mode'),
    '#description' => t('Syntax highlighting mode.'),
    '#default_value' => isset($snippet->type) ? $snippet->type : variable_get('geshinode_default_language', 'php'),
    '#options' => _geshifilter_get_enabled_languages(),
    '#required' => TRUE,
  );

  if (!empty($snippet)) {
    $form2['buttons']['delete'] = array(
      '#type' => 'submit',
      '#attributes' => array('class' => array('updateButton')),
      '#value' => t('Delete'),
      '#submit' => array('code_snippet_snippet_delete_submit'),
    );
  }

  return array_merge($form, $form2);
}

/**
 * The code_snippet_snippet form submit handler.
 */
function code_snippet_snippet_submit($form, &$form_state) {
  $csid = $form_state['values']['csid'];
  if ($csid) {
    drupal_write_record('code_snippet', $form_state['values'], 'csid');
  }
  else {
    drupal_write_record('code_snippet', $form_state['values']);
  }
  code_snippet_list_snippet_submit($form, $form_state);
}

/**
 * The code_snippet_list_snippet form submit handler.
 */
function code_snippet_list_snippet_submit($form, &$form_state) {
  if (isset($form_state['values']['snippets'])) {
    $snippet = code_snippet_load($form_state['values']['snippets']);
    $form_state['values']['type'] = $snippet->type;
    $form_state['values']['csid'] = $snippet->csid;
  }
  $alt = 'type='. $form_state['values']['type'] .'|csid='. $form_state['values']['csid']
         .'|tagstyle='. $form_state['values']['tagstyle'] .'|lineNumbering='. $form_state['values']['lineNumbering']
         .'|numberStart='. $form_state['values']['numberStart'];

  $title = array_key_exists('title', $form_state['values']) ? $form_state['values']['title'] : '';
  $img = '<img src="'. base_path() .'index.php?q=code-snippet/image/'. $form_state['values']['csid']
       .'" class="code-snippet" alt="'. $alt .'" title="'. check_plain($title) .'" />';

  drupal_add_js(
    'insertToEditor('. drupal_json_encode($img) .')',
    array(
      'type' => 'inline',
      'group' => JS_DEFAULT,
      'weight' => 0
    )
  );

  code_snippet_wysiwyg_dialogue($form['#editor'], $form['#textarea'], $form['#node'], $form_state['values']['csid']);
}

/**
 * The code_snippet_snippet form submit handler for delete.
 */
function code_snippet_snippet_delete_submit($form, &$form_state) {
  db_query("DELETE FROM {code_snippet} WHERE csid = :csid", array(':csid' => $form_state['values']['csid']));

  drupal_add_js(
    'insertToEditor("")',
    array(
      'type' => 'inline',
      'group' => JS_DEFAULT,
      'weight' => 0
    )
  );

  code_snippet_wysiwyg_dialogue($form['#editor'], $form['#textarea'], $form['#node'], NULL);
}

/**
 * Helper function to manage the URL that is used to keep context within
 * the dialog based actions.
 */
function _code_snippet_dialog_url($values, $action = NULL) {
  $extras = '&tagstyle='. $values['tagstyle'] .'&lineNumbering='. $values['lineNumbering'] .'&numberStart='. $values['numberStart'];
  return base_path() .'index.php?q=code-snippet/'. $values['editor'] .'/'. $values['textarea'] .'/'. $values['nid']
          .'/'. (empty($values['csid']) ? $values['csid'] : '0') . ($action ? '/'. $action : '') . $extras;
}

/**
 * Custom form theming.
 */
function theme_code_snippet_dialog_list_form($variables) {
  $form = $variables['form'];
  $js = <<<END
(function($){
$(function() {
  $('#edit-snippets').change(function(){
    var _csid = $(this).val();
    $.getJSON(
      Drupal.settings.basePath + 'index.php?q=code-snippet/preview/' + _csid,
      {
        lineNumbering: $('#edit-lineNumbering').val(),
        numberStart: $('#edit-numberStart').val(),
        tagstyle: $('input[name=tagstyle]:checked').val(),
        csid: _csid
      },
      function(data){
        $('#snippet-preview').html(data.data);
      }
    );
  });
});
})(jQuery);
END;
  drupal_add_js($js, 'inline');

  $titles = array('tagstyle' => '', 'lineNumbering' => '', 'numberStart' => '', 'snippets' => '');
  foreach ($titles as $key => $title) {
    $titles[$key] = $form[$key]['#title'];
    unset($form[$key]['#title']);
  }
  $tagstyle = '<div class="form-item"><label for="'. $form['tagstyle']['#id'] .'">'. $titles['tagstyle'] .'</label>'. drupal_render($form['tagstyle']) .'</div>';
  $snippets = '<div class="form-item"><label for="'. $form['snippets']['#id'] .'">'. $titles['snippets'] .'</label>'. drupal_render($form['snippets']) .'</div>';
  $lineNumbering = '<div class="form-item"><label for="'. $form['lineNumbering']['#id'] .'">'. $titles['lineNumbering'] .'</label>'. drupal_render($form['lineNumbering']) .'</div>';
  $numberStart = '<div class="form-item"><label for="'. $form['numberStart']['#id'] .'">'. $titles['numberStart'] .'</label>'. drupal_render($form['numberStart']) .'</div>';
  $buttons = drupal_render($form['buttons']);

  $values = array(
    'lineNumbering' => $form['lineNumbering']['#value'],
    'numberStart' => $form['numberStart']['#value'],
    'tagstyle' => $form['tagstyle']['#value'],
    'csid' => $form['snippets']['#value'],
  );
  $preview = code_snippet_code_snippet_process($values);

  $output = <<<END
<div id="code-snippet-option">
{$tagstyle}
{$lineNumbering}
{$numberStart}
</div>
<div id="code-snippet-snippet">
{$snippets}
<div id="snippet-preview">{$preview}</div>
</div>
{$buttons}
END;

  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Custom form theming.
 */
function theme_code_snippet_dialog_form($variables) {
  $form = $variables['form'];
  $titles = array('tagstyle' => '', 'lineNumbering' => '', 'numberStart' => '', 'title' => '', 'snippet' => '', 'type' => '');
  foreach ($titles as $key => $title) {
    $titles[$key] = $form[$key]['#title'];
    unset($form[$key]['#title']);
  }
  $title = '<div class="form-item form-item-block"><label for="'. $form['title']['#id'] .'">'. $titles['title'] .'</label>'. drupal_render($form['title']) .'</div>';
  $tagstyle = '<div class="form-item"><label for="'. $form['tagstyle']['#id'] .'">'. $titles['tagstyle'] .'</label>'. drupal_render($form['tagstyle']) .'</div>';
  $snippet = '<div class="form-item form-item-block"><label for="'. $form['snippet']['#id'] .'">'. $titles['snippet'] .'</label></div>' . drupal_render($form['snippet']);
  $lineNumbering = '<div class="form-item"><label for="'. $form['lineNumbering']['#id'] .'">'. $titles['lineNumbering'] .'</label>'. drupal_render($form['lineNumbering']) .'</div>';
  $numberStart = '<div class="form-item"><label for="'. $form['numberStart']['#id'] .'">'. $titles['numberStart'] .'</label>'. drupal_render($form['numberStart']) .'</div>';
  $type = '<div class="form-item"><label for="'. $form['type']['#id'] .'">'. $titles['type'] .'</label>'. drupal_render($form['type']) .'</div>';
  $buttons = drupal_render($form['buttons']);
  $output = <<<END
<div id="code-snippet-option">
{$tagstyle}
{$type}
{$lineNumbering}
{$numberStart}
</div>
<div id="code-snippet-snippet">
{$title}
{$snippet}
</div>
{$buttons}
END;

  $output .= drupal_render_children($form);
  return $output;
}
