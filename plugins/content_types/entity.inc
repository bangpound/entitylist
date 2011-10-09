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
  $list = $context->data;

  // The "readable" index starts at 1 while the "real" index starts at 0.
  $index = $conf['index'] - 1;
  $entity = $list->getEntity($index);
  if (empty($entity)) {
    return;
  }

  // TODO: Is there a more sustainable way to render entities in a generic way,
  // without depending on entity.module and entity_view()?
  $entity_type = $list->entityType();
  $view_function = $entity_type . '_view';
  list(, $view_mode) = explode(':', $conf['view_mode']);

  $block = new stdClass();
  $block->module  = 'entitylist';
  $block->content = $view_function($entity, $view_mode);
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
