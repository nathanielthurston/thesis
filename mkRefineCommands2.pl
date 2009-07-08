$dest = shift;
while ($inFile = <>) {
	chomp $inFile;
	$bits = $inFile;
	$bits =~ s!.*/!!;
	$bits =~ s!.out!!;
	print "cat $inFile | ./refine -b $bits -m 36 -t 6 -i 36 -s 3000000 -w allWords5 > $dest/$bits.out 2> $dest/$bits.err\n";
}
