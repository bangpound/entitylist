<?php

$plugin = array(
  'title' => t('List'),
  'description' => t('Create a list context.'),
  'context' => 'entitylist_ctools_context',
  'keyword' => 'list',
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

function entitylist_ctools_context_get_child($plugin, $parent, $child) {
  $list = entitylist_list_load($child);
  $child = entitylist_build_context_info($list, $parent, FALSE);
  return $child;
}

function entitylist_ctools_context_get_children($plugin, $parent) {
  $children = array(
    'entitylist' => $plugin,
  );
  $children['entitylist']['no ui'] = TRUE;

  $lists = entitylist_list_load_all();
  foreach ($lists as $list) {
    $info = entitylist_build_context_info($list, $parent, FALSE);
    if (!empty($info)) {
      $children[$info['name']] = $info;
    }
  }
  return $children;
}

function entitylist_ctools_context($empty, $data = NULL, $conf = FALSE) {
  $context = new ctools_context('entitylist');
  $context->plugin = 'entitylist';

  if ($empty) {
    return $context;
  }

  // This is a configured context, i.e. one without other required
  // arguments (contexts).
  if ($conf) {
    if (is_array($data) && isset($data['name'])) {
      list($plugin, $name) = explode(':', $data['name']);
      $data = entitylist_list_load($name);
    }
  }

  // $data is already the list itself. It probably came from a relationship
  // where it already was loaded and configured with some arguments (contexts).
  if (is_object($data)) {
    // We don't want to pass along the whole list object.
    $context->data = array(
      'name' => $data->name,
      'arguments' => $data->getArguments(),
    );
  }

  if (!empty($context->data)) {
    $context->title = $data->title;
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
