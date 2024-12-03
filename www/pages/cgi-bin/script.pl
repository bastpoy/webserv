#!/usr/bin/perl

# Corps de la réponse HTML
print "<!DOCTYPE html>\n";
print "<html>\n";
print "<head><title>CGI Test in Perl</title></head>\n";
print "<body style=\"text-align:center\">";
print "<nav style=\"padding:15px;\">";
print "<a href=\"../../index.html\" style=\"font-size: 20px;margin:1rem;width:150px;padding:5px;\">Home</a>";
print "<a href=\"../../cgi.html\" style=\"font-size: 20px;margin:1rem;width:150px;padding:5px;\">CGI</a>";
print "</nav>";
print "<h1>Perl CGI executed with success !</h1>";
print "<p>Server has correctly handle .pl file</p>";
print "</body>\n";
print "</html>\n";