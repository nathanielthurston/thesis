while (<>) {
	chomp;
	($box, $name, $area, $special, $xf) = split(/ /);
	$refine_result = `echo 1 | ./refine -w sortedWords -i 0 -a 8 -b $box 2>&1 | egrep 'HOLE |variety'`;
	chomp $refine_result;
	$refine_result =~ s/$box//;
	print "$box $name $area $refine_result\n";
}
