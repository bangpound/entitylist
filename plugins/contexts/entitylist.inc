<?php
/**
 * @file
 * Entity list context.
 */

$plugin = array(
  'title' => t('Entity list'),
  'description' => t('Adds an entity list to the page context.'),
  'context' => 'entitylist_ctools_context',
  'keyword' => 'list',
  'defaults' => array(
    'mustache' => array(
      'inline' => FALSE,
      'ajax' => array(
        'endpoint' => '',
        'initial' => FALSE,
        'interval' => 0,
      ),
    ),
  ),
  'context name' => 'entitylist',
  'convert list' => array(
    'name' => t('Name'),
    'title' => t('Title'),
    'description' => t('Description'),
  ),
  'convert' => 'entitylist_ctools_context_convert',
  'get child' => 'entitylist_ctools_context_get_child',
  'get children' => 'entitylist_ctools_context_get_children',
);

function entitylist_ctools_context_get_child($plugin, $parent, $child, $foo = NULL) {
  $list = entitylist_list_load($child);
  $info = entitylist_ctools_context_info($list, $parent, FALSE);
  return array_merge($plugin, $info);
}

function entitylist_ctools_context_get_children($plugin, $parent, $foo = NULL) {
  $children = array(
    'entitylist' => $plugin,
  );
  $children['entitylist']['no ui'] = TRUE;

  $lists = entitylist_list_load_all();
  foreach ($lists as $list) {
    $info = entitylist_ctools_context_info($list, $parent, FALSE);
    if (!empty($info)) {
      $children[$info['name']] = array_merge($plugin, $info);
    }
  }
  return $children;
}

function entitylist_ctools_context($empty, $data = NULL, $conf = FALSE) {
  if ($empty) {
    return ctools_context_create_empty('enititylist');
  }

  $context = new ctools_context('entitylist');
  $context->plugin = 'entitylist';

  // This is a configured context, i.e. one without other required
  // arguments (contexts).
  if ($conf) {
    if (is_array($data) && isset($data['name'])) {
      list($plugin, $name) = explode(':', $data['name']);
      $data = entitylist_list_load($name);
    }
  }

  if (!empty($data)) {
    $context->data = $data;
    $context->title = $data->title;
    // TODO: Make sure that this gives enough granularity when setting up
    // caching per context. Probably a lot of cache lifetime to save right here.
    $context->argument = $data->name;
    return $context;
  }
}

function entitylist_ctools_context_convert($context, $type) {
  switch ($type) {
    case 'name':
      return $context->data->name;
    case 'title':
      return $context->data->title;
    case 'description':
      return $context->data->description;
  }
}
