<?php
define('URL_PREFIX', '../../');
include '../../header.php';
?>
<div id="doxygen">
<!--
  doxygen 1.8.6 creates a closing div for id="top" within the body,
  but when the default header is disabled then the starting div
  isn't created. We're stuck with a closing, unmatched div tag.
  To fix this, let's add the stray div tag below.
-->
<div>

