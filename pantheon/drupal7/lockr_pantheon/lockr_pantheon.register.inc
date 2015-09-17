<?php

/**
 * @file
 * Form callbacks for Lockr register form.
 */

/**
 * Form constructor for Prisondog registration form.
 *
 * @see lockr_pantheon_register_form_validate()
 * @see lockr_pantheon_register_form_submit()
 *
 * @ingroup forms
 */
function lockr_pantheon_register_form($form, &$form_state) {
  $form = array();

  $site = lockr_pantheon_check_registration();
  if (!$site['exists']) {
    $form['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email address'),
      '#required' => TRUE,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Sign up'),
    );

    $next = isset($_GET['next'])
      ? $_GET['next']
      : '/admin/config/system/keys';
    $form_state['storage']['redirect'] = $next;
  }
  else {
    $form['registered'] = array(
      '#title' => t("You're already registered"),
      '#prefix' => '<p>',
      '#markup' => t(
        "This site is already registered with the Lockr Key Management Service. " .
        "There's nothing left for you do to here, " .
        'your keys entered in the key settings are already protected. ' .
        'If you registered with the wrong account, you can ' .
        'click <a href="@link" target="_blank">here</a> to go to Lockr and manage your sites.',
        array('@link' => 'http://getlockr.io/user/login')
      ),
      '#suffix' => '</p>',
    );
  }

  return $form;
}

/**
 * Form validation handler for lockr_pantheon_register_form().
 *
 * @see lockr_pantheon_register_form_submit()
 */
function lockr_pantheon_register_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('Please enter a valid email address'));
  }
}

/**
 * Form submission handler for lockr_pantheon_register_form().
 *
 * @see lockr_pantheon_register_form_validate()
 */
function lockr_pantheon_register_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $return = \Lockr\SiteClient::create()->register($values['email']);

  $next = $form_state['storage']['redirect'];
  if ($return === \Lockr\SiteClient::ALREADY_EXISTS) {
    drupal_set_message(t('This email is already registered with Lockr. ' .
                         'Please login to register a new site.'));
    drupal_goto('admin/config/system/lockr/login', array(
      'query' => array(
        'next' => $next,
        'email' => $form_state['values']['email'],
      ),
    ));
  }

  if ($return === \Lockr\SiteClient::UNKNOWN_ERROR) {
    form_set_error('', t('An unknown error has occurred, please try again later.'));
  }
  else {
    drupal_set_message(t("That's it! You're signed up with Lockr; your keys are now safe."));
    $form_state['redirect'] = $next;
  }
}