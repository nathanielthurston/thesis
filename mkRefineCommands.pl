$ext = shift;
while (<>) {
	chomp;
	print "echo 1 | ./refine -b $_ -V -m 36 -t 6 -i 36 -B 6 -w allWords$ext > expandFiles$ext/$_.out 2> expandFiles$ext/$_.err\n";
}
