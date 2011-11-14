<?php

$plugin = array(
  'title' => t('List item'),
  'category' => t('Content list'),
  'description' => t('Displays an item from a list.'),
  'required context' => new ctools_context_required(t('List'), 'entitylist'),
  'defaults' => array(
    'index' => 1,
    'view_mode' => '',
  ),
  'admin title' => 'entitylist_ctools_entity_admin_title',
  'admin info' => 'entitylist_ctools_entity_admin_info',
  'render callback' => 'entitylist_ctools_entity_render',
  'edit form' => 'entitylist_ctools_entity_form',
);

/**
 * Render callback.
 */
function entitylist_ctools_entity_render($subtype, $conf, $panel_args, $context) {
  // Get the list.
  $list = entitylist_context_get_list($context);

  $block = new stdClass();
  $block->module  = 'entitylist';
  // Provide integration with the cachetags plugin.
  $tags = $list->getCacheTags();
  $block->cache_tags = $block->request_tags = $tags;

  // The "readable" index starts at 1 while the "real" index starts at 0.
  $index = $conf['index'] - 1;
  $entity = $list->getEntity($index);
  if (empty($entity)) {
    $block->content = '';
    return $block;
  }

  $entity_type = $list->entityType();
  // TODO: Would be nice to avoid entity_get_info() here. The query handler
  // should theoritically already know about the entity id. Can we pass that
  // along somehow?
  $entity_info = entity_get_info($entity_type);
  $entity_id = $entity->{$entity_info['entity keys']['id']};

  // TODO: Is there a more sustainable way to render entities in a generic way,
  // without depending on entity.module and entity_view()?
  $view_function = $entity_type . '_view';
  list(, $view_mode) = explode(':', $conf['view_mode']);

  $block->content = $view_function($entity, $view_mode);

  // Also add the entity that is being rendered.
  $block->cache_tags[$entity_type][] = $entity_id;
  $block->request_tags[$entity_type][] = $entity_id;

  return $block;
}

/**
 * Edit form callback.
 */
function entitylist_ctools_entity_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $index_options = array();
  for ($i = 1; $i <= 50; $i++) {
    $index_options[$i] = t("$i");
  }
  $form['index'] = array(
    '#type' => 'select',
    '#title' => t('Index'),
    '#options' => $index_options,
    '#default_value' => $conf['index'],
  );

  $view_mode_options = array();
  $entity_infos = entity_get_info();
  foreach ($entity_infos as $entity_type => $entity_info) {
    $view_modes = array();
    foreach ($entity_info['view modes'] as $view_mode => $view_mode_info) {
      $view_modes[$entity_type . ':' . $view_mode] = $view_mode_info['label'];
    }
    if (!empty($view_modes)) {
      $view_mode_options[$entity_info['label']] = $view_modes;
    }
  }

  $form['view_mode'] = array(
    '#type' => 'select',
    '#title' => t('View mode'),
    '#options' => $view_mode_options,
    '#default_value' => $conf['view_mode'],
  );
  return $form;
}

function entitylist_ctools_entity_form_submit($form, &$form_state) {
  foreach (array('index', 'view_mode') as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

/**
 * Admin title callback.
 */
function entitylist_ctools_entity_admin_title($subtype, $conf, $context) {
  return t('Item @index from "@list"', array('@index' => $conf['index'], '@list' => $context->identifier));
}

/**
 * Admin info callback.
 */
function entitylist_ctools_entity_admin_info($subtype, $conf, $context) {
  list($entity_type, $view_mode) = explode(':', $conf['view_mode']);
  $entity_info = entity_get_info($entity_type);

  $block = new stdClass();
  $block->title = check_plain($entity_info['view modes'][$view_mode]['label']);
  $block->content = t('@mode for @type', array('@mode' => $block->title, '@type' => $entity_info['label']));
  return $block;
}
