<?php
/**
 * @file
 * zietlow_taxonomy_blog_topic.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function zietlow_taxonomy_blog_topic_taxonomy_default_vocabularies() {
  return array(
    'blog_topics' => array(
      'name' => 'Blog Topics',
      'machine_name' => 'blog_topics',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
