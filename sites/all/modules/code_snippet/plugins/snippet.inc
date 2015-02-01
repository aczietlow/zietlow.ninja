<?php

/**
 * @file
 * Wysiwyg API integration.
 */

/**
 * Implementation of hook_wysiwyg_plugin().
 */
function code_snippet_snippet_plugin() {
  $plugins = array();
  $plugins['snippet'] = array(
    'title' => t('Code snippet'),
    'vendor url' => 'http://drupal.org/project/code_snippet',
    'icon file' => 'snippet.gif',
    'icon title' => t('Insert or update an embedded code snippet.'),
    'settings' => array(
      'dialog' => array(
        'url' => base_path() .'index.php?q=code-snippet/load/tinymce',
        'width' => 600,
        'height' => 500,
      ),
    ),
  );
  return $plugins;
}
