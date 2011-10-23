<?php

$plugin = array(
  'title' => t('Admin link'),
  'category' => t('Content list'),
  'description' => t('Displays an admin link for a given list.'),
  'required context' => new ctools_context_required(t('List'), 'entitylist'),
  'defaults' => array(
    'text' => 'Edit list',
  ),
  'admin title' => 'entitylist_ctools_admin_link_admin_title',
  'admin info' => 'entitylist_ctools_admin_link_admin_info',
  'render callback' => 'entitylist_ctools_admin_link_render',
  'edit form' => 'entitylist_ctools_admin_link_form',
);

function entitylist_ctools_admin_link_render($subtype, $conf, $panel_args, $context) {
  // Get the list.
  $list = entitylist_context_get_list($context);
  $path = $list->adminPath();

  // This means that the user doesn't have access.
  if ($path === FALSE) {
    return;
  }

  $block = new stdClass();
  $block->module  = 'entitylist';
  $block->content = theme('entitylist_admin_link', array('text' => $conf['text'], 'path' => $path));
  return $block;
}

function entitylist_ctools_admin_link_admin_title($subtype, $conf, $context) {
  return t('Admin link for "@list"', array('@list' => $context->identifier));
}

function entitylist_ctools_admin_link_admin_info($subtype, $conf, $context) {
  $block = new stdClass();
  $block->title = t('Link text: @text', array('@text' => $conf['text']));
  $block->content = '';
  return $block;
}

function entitylist_ctools_admin_link_form($form, &$form_state) {
  $form['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Link text'),
    '#default_value' => $form_state['conf']['text'],
  );
  return $form;
}

function entitylist_ctools_admin_link_form_submit($form, &$form_state) {
  foreach (array('text') as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

function theme_entitylist_admin_link($variables) {
  return '<div class="entitylist-admin">' . l($variables['text'], $variables['path']) . '</div>';
}
