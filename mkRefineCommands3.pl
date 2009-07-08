$dest = shift;
while ($bits = <>) {
	chomp $bits;
	print "./treecat allExpand $bits | ./refine -b $bits -m 36 -t 6 -i 36 -s 3000000 -w allWords5 -B 6 > $dest/$bits.out 2> $dest/$bits.err\n";
}
