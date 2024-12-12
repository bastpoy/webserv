#!/usr/bin/php
<?php
// En-têtes nécessaires pour une réponse CGI correcte
header("Content-Type: text/html");

// Afficher les informations de l'environnement CGI
echo "<html>";
echo "<head><title>CGI Test</title></head>";
echo "<body style=\"text-align:center\">";
echo "<nav style=\"padding:15px;\">";
echo "<a href=\"../../index.html\" style=\"font-size: 20px;margin:1rem;width:150px;padding:5px;\">Home</a>";
echo "<a href=\"../../cgi.html\" style=\"font-size: 20px;margin:1rem;width:150px;padding:5px;\">CGI</a>";
echo "</nav>";
echo "<h1>PHP CGI executed with success !</h1>";
echo "<p>Server has correctly handle .php file</p>";
echo "</body>";
echo "</html>";
?>