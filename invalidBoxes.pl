input: while ($hole = <>) {
	chomp $hole;
	if ($result ne '' && $hole =~ /^$result/) {
		++$invalidCount{$result};
#		print "skipping $hole($box)\n";
		next input;
	} elsif (defined $result) {
		print "$result $invalidCount{$result}\n";
	}
	($result) = grep(s/box = //, `./treecat s5P $hole 2>&1`);
	$result = $hole unless defined $result;
	chomp $result;
	++$invalidCount{$result};
}
