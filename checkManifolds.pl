while (<>) {
	chomp;
	($box, $name, $area) = split(/ /);
	print "A\n";
	open(TC, "./treecat beyondVar $box |");
	print "B\n";
	$code = <TC>;
	chomp $code;
	print "$box $name $area $code\n";
	print "C\n";
}
